Known issues
* CC3000 related lockups happen sometimes.  Working on a solution but have not found it yet.
* HTTP server is rather slow since it sends 1 byte at a time.  This is related to the above problem.  sending data faster seems to cause more lockups.
* Lua evaluation is rather slow as it uses emulated floating point by default.  Need to recompile with integer only math.
* No way to find out what the IP of the lights are.  Need to add mDNS so that you can use "lights.local."

TODO
* mDNS support
* Better lua apis for setting large amounts of lights and calculating colors. (expose fastled apis).
* Lua http dispatching.  Provide a way for lua code to create urls and respond to them.  This might need an SD card for storage to do more than just an API layer.
* Maybe an http client for lua?  That way you can get data from other sources?
* Add configuration options in the Makefile, to let you select what type of leds and how many you want to support
* Setup git submodules for the Arduino-mk makefile to make building easier
* Get LTO working to reduce binary size.

