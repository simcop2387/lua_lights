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

// INIT AND FRAME CODE.
void l_init() {
  L = lua_newstate(l_alloc, 0);

  LogOut.println("Importing libraries");
  luaopen_table(L);
  luaopen_string(L);
  luaopen_math(L);
  luaopen_bit32(L);
  luaopen_coroutine(L); // should this be optional?
  luaopen_array(L);
  LogOut.println("Libraries imported\nLoading hello world");
  //luaopen_debug(L);
  //getarray(L);
  
  luaL_dostring(L, "function hello()\n"
                   "    leds = led_data()\n"
                   "    leds[11] = 255\n"
                   "    return 2\n"
                   "end");
}

void l_frame() {
  lua_Integer d;
  LogOut.println("Executing hello world:");
//  LogOut.println(millis(), DEC);
  lua_getglobal(L, "hello");
  
  if (lua_pcall(L, 0, 1, 0) != 0) {
    LogOut.print("error calling hello: ");
    LogOut.println(lua_tostring(L, -1));
  }
  
  if (!lua_isnumber(L, -1)) {
    LogOut.println("Return value must be number");
  }
  
  d = lua_tointeger(L, -1);
  lua_pop(L, 1);
}