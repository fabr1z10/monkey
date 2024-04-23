# Build guide
In order to build `monkey` library you need to have `cmake` installed. In order to build the module, you need to have the following packages installed in your system:

* OpenGL 
* GLEW (OpenGL Extension Wrangler Library)
* GLFW 
* PNG library (as well as Zlib which is required by the PNG library)
* yaml-cpp
* Python 3

In the next sections we provide installation tips specific for the different platforms.

To clone the project in your system use the following command line (`--recurse-submodules` is required as it will bring in the required `Pybind11` project)
```bash
git clone --recurse-submodules git@github.com:fabr1z10/monkey.git
```

# Ubuntu
Install all required dependencies:

```bash
sudo apt-get update -y 
sudo apt-get install -y libglew-dev libglfw libglfw-dev libpng-dev libyaml-cpp-dev
```

# Windows

## zlib
Download the latest version of zlib here: https://www.zlib.net/. Download the zip file, unzip it and type:

```bash
mkdir build & cd build
cmake ..
```
This will create the Visual Studio solution and project files. Open Visual Studio and build the projects (in build and release configurations). This operation will create the Debug/ and Release/ directories where the binary files will be added. To install, you need to run VS with administrator privileges. Installing will typically create a subdirectory 
`zlib` in `c:\program files (x86)\` where the `include` folder and binary files are stored.

## libpng
You can download the source files here: https://sourceforge.net/projects/libpng/files/. At the time of writing latest version is 1.6.39. Download the zip file, unzip and type:

```bash
mkdir build & cd build
cmake ..
```

This will create the Visual Studio solution and project files. Open Visual Studio and build the projects (in build and release configurations). This operation will create the Debug/ and Release/ directories where the binary files will be added. To install, you need to run VS with administrator privileges as with Zlib.

