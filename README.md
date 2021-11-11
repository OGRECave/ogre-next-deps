OGRE-next DEPENDENCIES SOURCE PACKAGE
==================================

This package is provided as a quick route to compile the core
dependencies of [OGRE-next](https://github.com/OGRECave/ogre-next) on most supported
platforms. For a list of the included libraries and their 
versions, see versions.txt

CHECKOUT
========

This repo contains submodules. Therefore it needs to be cloned using the following command:

`git clone --recurse-submodules --shallow-submodules https://github.com/OGRECave/ogre-next-deps`

Checkout the [Quick Start Scripts](https://www.ogre3d.org/download/sdk/sdk-ogre-next) to see scripts
that automatically clone this repo and build it (and also clone the main OGRE-next repo and build it).


COMPILATION
=============

You need CMake (http://www.cmake.org). In a console, type:

```
cd /path/to/ogredeps
mkdir build
cd build
cmake ..
make
make install
```

If you are on a Windows system or prefer graphical interfaces,
launch cmake-gui instead. Enter as the source code path the
path where you extracted ogredeps (i.e. where this readme 
resides). Select any directory to build the binaries. Hit
'Configure', choose your compiler set and click Ok. Click
'Generate' twice. CMake will generate a set of project files
for your IDE (e.g. Visual Studio or XCode) in the chosen build
directory. Open them, compile the target 'BUILD_ALL'. Also build
the target 'install'.

**Note: Python 3.X is required to build the "shaderc" project.** Download link: [Python 3.X](https://www.python.org/downloads/)

USAGE
=======

When compilation was successful and the install target was built,
you should find a new directory 'ogredeps' in your build path.
This contains the final include and lib files needed. Copy it
to your Ogre source directory, rename it to Dependencies (or
iOSDependencies or AndroidDependencies for those platforms)
then rerun CMake for your Ogre build.
It should pick up the dependencies automatically.

Check the [manual](https://ogrecave.github.io/ogre-next/api/2.3/_setting_up_ogre.html) for more information.
