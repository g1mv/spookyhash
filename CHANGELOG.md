1.0.5
-----
*April 21, 2015*

* Switched to cmake for improved windows compatibility
* Now using builtin memory functions (memcpy, memset) when available

1.0.4
-----
*April 3, 2015*

* Removed memory allocation casts

1.0.3
-----
*March 24, 2015*

* Fixed string.h include problem
* Improved compilation switches speed-wise

1.0.2
-----
*February 6, 2015*

* Now using single Makefile

1.0.1
-----
*February 4, 2015*

* Added inlining for all functions
* Reverted to using uintN_t instead of uint_fastN_t due to bitwise shift problems
* Defined rotate function as macro
* Added makefiles

1.0.0
-----
*February 2, 2015*

* Support for big endian platforms
* Support for multi-threaded environments