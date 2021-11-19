# SNiPER

The SNiPER is an open-source general purpose software framework for High Energy Physics data processing. It is dedicated for needs of rare-process search experiments, such as neutrino interactions, dark matter, double beta and other.

For more info please visit [the SNiPER home at GitHub](https://github.com/SNiPER-Framework).

## Prerequisities

Required packages:
* cmake (3.12 or later) - build the project;
* Compiler with C++11 support (GCC 4.8 or LLVM 3.4 or other);

Recommended packages:
* Boost (1.67 or later, with Boost.Python) - portable C++ source libraries, used for interface with python scripts;
* Python (2.7 or later) - interpreted, interactive, object-oriented programming language, used as main script engine for SNiPER modules.

Optional software:
* git (1.8 or later) - needed to get source code with repository;
* ROOT (5.18 or later) - needed for ROOT file I/O;

Note that in RHEL6 system (and derivatives) an old GCC 4.4.7 is used. To enable C++11 features, we recommend to use [devtoolset](http://linux.web.cern.ch/linux/devtoolset/) packages, where GCC 4.8 or better are available.

## Build and test

Get the source code using `git clone https://github.com/SNiPER-Framework/sniper.git` or downloading a [ZIP file](https://github.com/SNiPER-Framework/sniper/archive/master.zip) from GitHub and extracting it.

Make build directory and change to it:

```
$ mkdir build && cd build
```

Run CMake to configure your build. You can change your settings with `ccmake` or other tools. CMake has powerful ability to find software, but if it fails to locate yours, you can help it by defining `<NAME>_DIR` variables. By default installation goes to `/usr/local`. Use `-DCMAKE_INSTALL_PREFIX` to specify another one:

```
$ cmake -DCMAKE_INSTALL_PREFIX=<install_dir> ..
```

There are more cmake options we can use while configuring the build:
* `-DDEBUG=OFF` : Switch the debug option. The default value is `OFF`.
* `-DUSE_PYTHON=ON` : Build the project with Python support. A pure C++ version will be built if it is turned off. The default value is `ON`.
    * `-DUSE_PYTHON2=OFF` : Turn it on to use Python2, otherwise Python3 is used by default.
* `-DUSE_ROOT=ON` : Build components that depend on ROOT. The default value is `ON`.
* `-DCMAKE_CXX_STANDARD=11|14|17` : specify the C++ standard 11, 14, 17 or newer.

Then build and install the result. Default build agent is `make` and you can call it directly.

```
$ cmake --build . --target install
```

Your build is now installed to the directory you provided to CMake. For the best experience one needs to set the proper environment. This can be done with the help of shell script `<install_dir>/share/sniper/setup.sh`, or, a modern solution, using an Environment Modules file `<install_dir>/share/sniper/modulefiles/sniper`.

Suppose the environment has been set, then we can run the tests in the build directory:

```
$ make test
```

The test uses the components in `Examples`, which covers the main functionalities of SNiPER.

## Contributing

Contributions and bug reports are welcome!
