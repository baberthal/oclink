# oclink
A port of [Audiohacked's OpenCorsairLink][upstream] to C
Written using information from [this thread][cl-forum].

## WARNING
This is a **Work in Progress**, and may not work. Pull requests welcome!

## Components
There are several components to this software:
* The [library](/clink/)
* The [cli](/oclink/)
* The [tui](/tui/)
* The [OSX GUI][osx-gui]
* The [QT GUI](/qt-gui/)

## Supported CorsairLink Systems
* [H80i][h80i]
* [H100i][h100i]
* [H110i][h110i]

## Building
All products are built using [CMake][cmake], with the exception of the [OSX
GUI][osx-gui], which is built using [Xcode][xcode].

[HIDAPI][hidapi] is a dependency and must be installed separately. See
[here][hidapi] for instructions.

On most systems, you can build the products using the following commands
```shell
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
```

To install:
```shell
$ cmake --build . --target install
```

The following options are available
* `-DCMAKE_INSTALL_PREFIX=/where/you/want/to/install`
  > to change the installation prefix

* `-DBUILD_SHARED_LIBS=[ON,OFF]`
  > whether or not to build shared libraries (default is static)

* `-DHIDAPI_PREFIX=/path/to/hidapi/install/location`
  > the prefix of the directory where hidapi is installed (default is
  > /usr/local)

* `-DBUILD_DOCUMENTATION=[ON,OFF]`
  > to build the doxygen documentation. Doxygen is required.

For more advanced use or questions about CMake, please read the [CMake
FAQ][cmake-faq].

## Documentation
After [configuring the build](#building), run
```
$ make doc
```
to build the Doxygen documentation. Documentation is also available online
[here][ocl-gh-pages]

## Using the CLI
TODO

## Using the GUI
TODO

[upstream]: https://github.com/audiohacked/OpenCorsairLink "Upstream Repo"
[osx-gui]: /osx-gui/ "OSX GUI Folder"
[h80i]: http://www.corsair.com/en-us/hydro-series-h80i-v2-high-performance-liquid-cpu-cooler
[h100i]: http://www.corsair.com/en-us/hydro-series-h100i-extreme-performance-liquid-cpu-cooler
[h110i]: http://www.corsair.com/en-us/hydro-series-h110i-280mm-extreme-performance-liquid-cpu-cooler
[cmake]: https://cmake.org
[cmake-faq]: https://cmake.org/Wiki/CMake_FAQ
[xcode]: https://developer.apple.com/xcode/
[hidapi]: https://github.com/signal11/hidapi
[cl-forum]: http://forum.corsair.com/forums/showthread.php?t=120092


[//]: # ( vim: set ts=8 sw=2 tw=80 ft=markdown noet :)
