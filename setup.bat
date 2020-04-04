del /s /q build
mkdir build
cd build
mklink /d Resources ..\Resources
cmake -G "Visual Studio 14 Win64" ..
cd ..