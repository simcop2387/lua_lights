#include "log.h"
#include "leds.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
  
/*These three are needed for stubs because something for some god awful reason wants them.  Can't find the source of them and they're easier to fix this way*/
int _kill(pid_t pid, int sig) {return 0;}
int _getpid() {return 1;}
int _gettimeofday(struct timeval *tv, struct timezone *tz) {return -1;}
int _open(const char *pathname, int flags) {return -1;}
ssize_t _write(int fd, const void *buf, size_t count) {return -1;}
}

lua_State *L;

// I want to replace this with an arena allocator so that i can get better memory performance in the future
 static void *l_alloc (void *ud, void *ptr, size_t osize, size_t nsize) {
  LogOut.print("\t->alloc called: ");
  LogOut.print((size_t) ud, HEX);
  LogOut.print(" ");
  LogOut.print((size_t) ptr, HEX);
  LogOut.print(" ");
  LogOut.print(osize, DEC);
  LogOut.print(",");
  LogOut.println(nsize, DEC);

  (void)ud;  (void)osize;  /* not used */
  if (nsize == 0) {
    free(ptr);
    return NULL;
  }
  else
    return realloc(ptr, nsize);
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
  L = lua_newstate(l_alloc, 0);

  LogOut.println("Importing libraries");
  
  l_openlibs(L);
  luaopen_array(L);
  LogOut.println("Libraries imported\nLoading hello world");
  //luaopen_debug(L);
  //getarray(L);
  
  int e = luaL_dostring(L, 
                   //"coroutine = require(\"coroutine\")\n"
                   "function run_frame()\n"
                   "    coroutine.resume(corun)\n"
                   "    collectgarbage()\n"
                   "end\n"
                   "function coro_loop()\n"
                   "    while true do\n"
                   "        user_func()\n"
                   "    end\n"
                   "end\n"
                   "function setup_frame()\n"
                   "    corun = coroutine.create(coro_loop)\n"
                   "    debug.sethook(corun, coroutine.yield, \"\", 50)\n"
                   "    \n"
                   "end\n"
                   "offset = 0\n"
                   "function user_func()\n"
                   "    local leds = led_data()\n"
                   "    local i = 0\n"
                   "    for i = 0, 10 do\n"
                   "        leds[i] = offset\n"
                   "    end\n"
                   "    offset = offset + 10\n"
                   "end\n"
                   "setup_frame()");

  if (e) {
    LogOut.print("error evaling default sub: ");
    LogOut.println(lua_tostring(L, -1));
  }
}

void l_reinit() {
  lua_close(L);
  // clear memory when I have allocator writte
  l_init(); // reload system
}

void l_frame() {
  lua_Integer d;
  LogOut.println("Executing hello world:");
//  LogOut.println(millis(), DEC);
  lua_getglobal(L, "run_frame");
  
  if (lua_pcall(L, 0, 1, 0) != 0) {
    LogOut.print("error calling hello: ");
    LogOut.println(lua_tostring(L, -1));
  }
  
  // NTS: DO NOT REMOVE THIS.
  // If it's removed you end up with gigantic memory leaks and an exponentially growing stack
  lua_pop(L, 1); // remove return from function, just ignore it
}