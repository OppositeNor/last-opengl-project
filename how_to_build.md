# How to Build

Clone the repository with all the sobmodules

```shell
git clone --recursive https://github.com/OppositeNor/last-opengl-project.git
cd part1
```

Install build utilities and dependencies:

- Arch Linux

```shell
sudo pacman -S gcc ninja cmake
yay -S icu
```

Build the project using the local dependencies

```shell
# Debug
cmake -B build -G Ninja -DUSE_LOCAL_DEPS=ON
cmake --build build

# Release
cmake -B release -G Ninja -DUSE_LOCAL_DEPS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build release --config Release
```
