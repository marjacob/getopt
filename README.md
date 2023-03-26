A getopt implementation for Windows
===================================

This repository contains a modified variant of [Kim Grasman]'s [`kimgr/getopt_port`][getopt_port] project.

> An original implementation of `getopt` and `getopt_long` with limited GNU extensions. Provided under the BSD license, to allow non-GPL projects to use `getopt`-style command-line parsing.
>
> Built with Visual C++ and Clang on FreeBSD, but has no inherently non-portable constructs.
>
> Intended to be embedded into your code tree -- `getopt.h` and `getopt.c` are self-contained and should work in any context.
>
> Comes with a reasonable unit test suite.
>
> See also:
> * [Full Win32 getopt port](http://www.codeproject.com/Articles/157001/Full-getopt-Port-for-Unicode-and-Multibyte-Microso) -- LGPL licensed.
> * [XGetOpt](http://www.codeproject.com/Articles/1940/XGetopt-A-Unix-compatible-getopt-for-MFC-and-Win32) -- No `getopt_long` support.
> * [Free Getopt](https://sourceforge.net/projects/freegetopt/) -- No `getopt_long` support.
>
> For license terms, see [LICENSE.txt](src/getopt/LICENSE.txt).

How is this variant different?
------------------------------

This version supports the [`wWinMain`][wWinMain] and [`wmain`][wmain] entry points because it uses `wchar_t` instead of `char`.

### Other differences

* The CMake build system has been replaced with a Visual Studio _solution_.
* The unit test suite has been rewritten for the Microsoft C++ Unit Testing Framework.
  * Run it from the _Test Explorer_ in Visual Studio.

### Known issues

The internal `optcursor` variable appears to not always be reset to `NULL`, and this causes some issues with unit tests executing after other tests (because `getopt` and `getopt_long` uses global state by design). I introduced the `getopt_reset` function to work around this problem.

To trigger this issue: Find the `setup` function in each unit test file and comment out the call to `getopt_reset`. Then execute the test suite.

[Kim Grasman]: mailto:kim.grasman@gmail.com
[getopt_port]: https://github.com/kimgr/getopt_port
[wWinMain]: https://learn.microsoft.com/en-us/windows/win32/learnwin32/winmain--the-application-entry-point
[wmain]: https://learn.microsoft.com/en-us/cpp/c-language/using-wmain

