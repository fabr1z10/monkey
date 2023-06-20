# test

```bash
git clone git@github.com:fabr1z10/monkey.git
pip install ./monkey
```
# Ubuntu
Install all require dependencies:

```bash
sudo apt-get update -y 
sudo apt-get install -y libglew-dev libglfw libglfw-dev libpng-dev
```


# Windows
## libpng
You can download the source files here: https://sourceforge.net/projects/libpng/files/. At the time of writing latest version is 1.6.39. Download the zip file, unzip and type:

```bash
mkdir build & cd build
cmake ..
msbuild INSTALL.vcxproj /p:Configuration=Release /p:Platform=x64
```

This will install libpng in your system. Typically, it will copy the dll in `c:\windows\system32` and the development files (include & libs) in `c:\program files (x86)\libpng`.
