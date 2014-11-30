#include "log.h"
#include "leds.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lua-alloc.h"

/*These three are needed for stubs because something for some god awful reason wants them.  Can't find the source of them and they're easier to fix this way*/
int _kill(pid_t pid, int sig) {return 0;}
int _getpid() {return 1;}
int _gettimeofday(struct timeval *tv, struct timezone *tz) {return -1;}
int _open(const char *pathname, int flags) {return -1;}
ssize_t _write(int fd, const void *buf, size_t count) {return -1;}
}

lua_State *L, *t1;

// predeclarations
void l_start(const char *prgm);
void l_initthread();

extern char __llalloc_sbrk_start[];
static void l_dumpmem() {
  uint8_t byte = 0;
  
  LogOut.print("\t\t");
  for (int i = 0; i < LL_ARENA_SIZE; i++) {
    byte = __llalloc_sbrk_start[i];
    LogOut.print(byte, HEX);
  }
  LogOut.println();
}

static void l_memstats() {
  struct mallinfo c= ll_mallinfo();
  char buff[64];
  sprintf(buff, "->mallinfo: t:%d f:%d u:%d", c.arena, c.fordblks, c.uordblks);
  debug_log(buff);
}

// I want to replace this with an arena allocator so that i can get better memory performance in the future
 static void *l_alloc (void *ud, void *ptr, size_t osize, size_t nsize) {
  l_memstats();
  
  //"\t->alloc called: 12345678 12345678 12345 12345";
  char buff[64];
  sprintf(buff, "\t->alloc called: %08X %08X %05d %05d", (size_t) ud, (size_t) ptr, osize, nsize);
  debug_log(buff);
 
  (void)ud;  (void)osize;  /* not used */
  if (nsize == 0) {
    ll_free(ptr);
    return NULL;
  }
  else
    return ll_realloc(ptr, nsize);
}

// CREATE ARRAY TYPE AND EXPOSE led_data TO LUA.
// metatable method for handling "array[index]"
static int led_data_index (lua_State* L) { 
   CRGB** parray = (CRGB **) luaL_checkudata(L, 1, "led_data");
   int index = luaL_checkint(L, 2);
   lua_pushnumber(L, (*parray)[index-1]);
   return 1; 
}

// metatable method for handle "array[index] = value"
static int led_data_newindex (lua_State* L) { 
   CRGB** parray = (CRGB **) luaL_checkudata(L, 1, "led_data");
   int index = luaL_checkint(L, 2);
   int value = luaL_checkint(L, 3);
   (*parray)[index-1] = value;
   return 0; 
}

void l_yield(lua_State *L, lua_Debug *ar) {
  lua_yield(L, 0);
}

static int l_sethook(lua_State *L) {
  lua_sethook(L, &l_yield, LUA_MASKCOUNT, 2);
  return 0;
}

int full_run(lua_State *L, lua_State *t) {
  int rc = lua_resume(t, L, 0);

  // TODO this is the all important part where i handle errors, the main function exiting, and make sure yielding went properly
  char buff[24];
  
  if (rc == 0) {
    // Should this be output log?
    debug_log("User supplied function exited successfully.  Restarting thread");
    l_initthread();
  } else if (rc == LUA_ERRRUN) {
    output_log("Runtime error: ");
    output_log(lua_tostring(t1, -1));
    output_log("Halting lua runtime");
    t1 = NULL;
  } else if (rc != LUA_YIELD) {
    sprintf(buff, "rc = %d", rc);

    output_log("Unknown error: ");
    output_log(buff);
    output_log(lua_tostring(t1, -1));
    output_log("Halting lua runtime");
    t1 = NULL;
  } 

  return rc;
}

static const struct luaL_Reg led_data_metamethods[] = {
    { "__index",  led_data_index  },
    { "__newindex",  led_data_newindex  },
    {NULL, NULL}
};


// create a metatable for our array type
static void create_array_type(lua_State* L) {
  luaL_newmetatable(L, "led_data");
  luaL_setfuncs(L, led_data_metamethods, 0);
  //lua_setglobal(L, "led_data");
}


// expose an array to lua, by storing it in a userdata with the array metatable
static int expose_array(lua_State* L, CRGB array[]) {
   CRGB** parray = (CRGB **) lua_newuserdata(L, sizeof(CRGB**));
   *parray = array;
   luaL_getmetatable(L, "led_data");
   lua_setmetatable(L, -2);
   return 1;
}

// test routine which exposes our test array to Lua 
static int getarray (lua_State* L) { 
   return expose_array( L, led_data );
}

int luaopen_array (lua_State* L) {
   create_array_type(L);

   // make our test routine available to Lua
   lua_register(L, "led_data", getarray);
   return 0;
}

// This code blatantly stolen and borrowed from lbaselib.c luaB_print
static int l_print (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  int i;
  lua_getglobal(L, "tostring");
  for (i=1; i<=n; i++) {
    const char *s;
    size_t l;
    lua_pushvalue(L, -1);  /* function to be called */
    lua_pushvalue(L, i);   /* value to print */
    lua_call(L, 1, 1);
    s = lua_tolstring(L, -1, &l);  /* get result */
    if (s == NULL)
      return luaL_error(L,
         LUA_QL("tostring") " must return a string to " LUA_QL("print"));
    //if (i>1) luai_writestring("\t", 1);
    output_log(s);
    lua_pop(L, 1);  /* pop result */
  }
  //luai_writeline();
  return 0;
}

static const luaL_Reg loadedlibs[] = {
  {"_G", luaopen_base},
//  {LUA_LOADLIBNAME, luaopen_package},
  {LUA_COLIBNAME, luaopen_coroutine},
  {LUA_TABLIBNAME, luaopen_table},
  {LUA_STRLIBNAME, luaopen_string},
  {LUA_BITLIBNAME, luaopen_bit32},
  {LUA_MATHLIBNAME, luaopen_math},
  {LUA_DBLIBNAME, luaopen_debug},
  {NULL, NULL}
};

void l_openlibs(lua_State *L) {
  const luaL_Reg *lib;
  /* call open functions from 'loadedlibs' and set results to global table */
  for (lib = loadedlibs; lib->func; lib++) {
    luaL_requiref(L, lib->name, lib->func, 1);
    lua_pop(L, 1);  /* remove lib */
  }
}

// INIT AND FRAME CODE.
void l_init() {
  l_start("function main()\n"
          "    local leds = led_data()\n"
          "    print \"Hello World\"\n"
          "    while true do\n"
          "        \n"
          "        for g = 0, 255, 5 do\n"
          "                  leds[1] = 255 * 65536 + g * 256\n"
          "        end\n"
          "        for r = 255, 0, -5 do\n"
          "                  leds[1] = r * 65536 + 255 * 256\n"
          "        end\n"
          "        for b = 0, 255, 5 do\n"
          "                  leds[1] = 255 * 256 + b\n"
          "        end\n"
          "        for g = 255, 0, -5 do\n"
          "                  leds[1] = g * 256 + 255\n"
          "        end\n"
          "        for r = 0, 255, 5 do\n"
          "                  leds[1] = r * 65536 + 255\n"
          "        end\n"
          "        for b = 255, 0, -5 do\n"
          "                  leds[1] = 255 * 65536 + b\n"
          "        end\n"
          "    end\n"
          "end\n");
  
  //l_dumpmem();
}

void l_initthread() {
  // Create the coroutine
  t1 = lua_newthread(L);
  l_sethook(t1);
  lua_getglobal(t1, "main");
}

void l_stop() {
  if (L != NULL) 
    lua_close(L);
  t1 = NULL;
}

void l_start(const char *prgm) {
  ll_clean_arena();
  t1 = NULL;
  L = lua_newstate(l_alloc, 0);

  debug_log("Importing libraries");
  
  l_openlibs(L);
  luaopen_array(L);
  lua_pushcfunction(L, l_print);
  lua_setglobal(L, "print");

  debug_log("Libraries imported");
  debug_log("Loading program");

  int e = luaL_dostring(L, prgm);

  if (e) {
    output_log("error evaling lua code: ");
    output_log(lua_tostring(L, -1));
  }
  
  l_initthread();
}

void l_frame() {
  full_run(L, t1);
}