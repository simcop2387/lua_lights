#include "log.h"

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

void l_init() {
  L = lua_newstate(l_alloc, 0);

  LogOut.println("Importing libraries");
  luaopen_table(L);
  luaopen_string(L);
  luaopen_math(L);
  luaopen_bit32(L);
  luaopen_coroutine(L); // should this be optional?
  LogOut.println("Libraries imported\nLoading hello world");
  //luaopen_debug(L);

  luaL_dostring(L, "function hello()\n" \
                   "    return 2\n" \
                   "end");
}

void l_frame() {
    lua_Integer d;
  LogOut.print("Executing hello world: ");
  LogOut.println(millis(), DEC);
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