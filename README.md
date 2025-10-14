# isplitter

Image Splitter is a simple Qt-based tool to split individual images from a 
image collage. It can rotate and resize the collage before removing borders
that surround the entire collage or individual images in a collage. It 
initially assumes all images in the collage are of same size and alignment,
but cropping, and horizontal and vertical offsets, can be adjusted per image
before saving.

## Building

### Windows

Image Splitter should build without changes under Windows 11 with Microsoft
Visual C++ 2019-2022, vcpkg, CMake 3.16 or later, and Qt 6.10 or later. If CMake can't find zlib headers, rename
`CMakeUserPresets.json.sample` to `CMakeMakeUserPresets.json` and update `VCPKG_ROOT` in
the file to reflect installation path of standalone version of vcpkg. vcpkg can be
downloaded from [GitHub](https://github.com/microsoft/vcpkg/releases). Pass the vcpkg
toolchain file to CMake when configuring the build.

For example:
```
md build
cd build
cmake -D CMAKE_TOOLCHAIN_FILE=c:/vcpkg/scripts/buildsystems/vcpkg.cmake ..
cmake --build . --config Release
```

### Linux

It should also build with minimal changes under most Linux distributions with Qt6 development packages installed.
