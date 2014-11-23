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
  Serial.print("\t->alloc called: ");
  Serial.print((size_t) ud, HEX);
  Serial.print(" ");
  Serial.print((size_t) ptr, HEX);
  Serial.print(" ");
  Serial.print(osize, DEC);
  Serial.print(",");
  Serial.println(nsize, DEC);

  (void)ud;  (void)osize;  /* not used */
  if (nsize == 0) {
    free(ptr);
    return NULL;
  }
  else
    return realloc(ptr, nsize);
}

void l_init() {
  L = lua_newstate(l_alloc, 0);

  Serial.println("Importing libraries");
  luaopen_table(L);
  luaopen_string(L);
  luaopen_math(L);
  luaopen_bit32(L);
  luaopen_coroutine(L); // should this be optional?
  Serial.println("Libraries imported\nLoading hello world");
  //luaopen_debug(L);

  luaL_dostring(L, "function hello()\n" \
                   "    return 2\n" \
                   "end");
}

void l_frame() {
    lua_Integer d;
  Serial.print("Executing hello world: ");
  Serial.println(millis(), DEC);
  lua_getglobal(L, "hello");
  
  if (lua_pcall(L, 0, 1, 0) != 0) {
    Serial.print("error calling hello: ");
    Serial.println(lua_tostring(L, -1));
  }
  
  if (!lua_isnumber(L, -1)) {
    Serial.println("Return value must be number");
  }
  
  d = lua_tointeger(L, -1);
  lua_pop(L, 1);
}