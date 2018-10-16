# SNiPER

The SNiPER is a open-source general purpose software framework for High Energy Physics data processing. It is dedicated for needs of rare-process search experiments, such as neutrino interactions, dark matter, double beta and other.

For more info please visit [the SNiPER home at GitHub](https://github.com/SNiPER-Framework).

## Prerequisities

Required packages:
* CMT (v1r26 or later) - configuration management environment, main build system;
* Compiler with C++11 support (GCC 4.8 or LLVM 3.4 or other);
* Boost (1.53 or later, with Boost.Python) - portable C++ source libraries, used for interface with python scripts;
* Python (2.7 or later) - interpreted, interactive, object-oriented programming language, used as main script engine for SNiPER modules.

Optional software:
* git (1.8 or later) - needed to get source code with repository;
* cmake (2.8 or later) - alternative build system;
* ROOT (5.18 or later) - needed for ROOT file I/O;
* Intel TBB - for parallel computing.

Note that in RHEL6 system (and derivatives) an old GCC 4.4.7 is used. To enable C++11 features, we recommend to use [devtoolset](http://linux.web.cern.ch/linux/devtoolset/) packages, where GCC 4.8 or better are available.

## Build

### Using CMT

You need installed and working CMT environment. Also you'll need external packages Boost, Python (and ROOT) available under Externals/.

Get the source gode using `git clone https://github.com/SNiPER-Framework/sniper.git` or downloading a [ZIP file](https://github.com/SNiPER-Framework/sniper/archive/master.zip) from GitHub and extracting it.

Add source code directory to CMT path:

```
$ cd sniper
$ export CMTPROJECTPATH=`pwd`:$CMTPROJECTPATH
```

Actually build SNiPER:

```
$ cd SniperRelease/cmt
$ cmt br cmt config
$ source setup.sh
$ cmt br cmt make
```

### Using CMake

Get the source gode using `git clone https://github.com/SNiPER-Framework/sniper.git` or downloading a [ZIP file](https://github.com/SNiPER-Framework/sniper/archive/master.zip) from GitHub and extracting it.

Make build directory and change to it:

```
$ mkdir build && cd build
```

Run CMake to configure your build. By default it installs to `/usr/local`:

```
$ cmake -DCMAKE_INSTALL_REFIX=<install_dir> ..
```

Then build and install result:

```
$ make
$ make install
```

Your build is now installed to the directory you provided to CMake, shell script `setup.sh` will help you to set the required environment.
