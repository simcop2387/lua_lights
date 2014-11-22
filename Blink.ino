/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  This example code is in the public domain.
 */

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

// I want to replace this with an arena allocator so that i can get better memory performance in the future
     static void *l_alloc (void *ud, void *ptr, size_t osize,
                                                size_t nsize) {
       (void)ud;  (void)osize;  /* not used */
       if (nsize == 0) {
         free(ptr);
         return NULL;
       }
       else
         return realloc(ptr, nsize);
     }

lua_State *L;
     
void setup() {
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  L = lua_newstate(l_alloc, 0);

/*  luaopen_table(L);
  luaopen_string(L);
  luaopen_math(L);
  luaopen_bit32(L);
  luaopen_coroutine(L); // should this be optional?*/
  //luaopen_debug(L);
//  luaL_openlibs(L);  // Can't init this way, need to use something else.

/*  luaL_dostring(L, "function hello()\n" \
                   "    return 4200\n" \
                   "end"); */
  
  lua_close(L);
}

void loop() {
  lua_Integer d;
  
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
  
  digitalWrite(13, HIGH);   // set the LED on
  delay(1000);              // wait for a second
  digitalWrite(13, LOW);    // set the LED off
  delay(1000);              // wait for a second
}
