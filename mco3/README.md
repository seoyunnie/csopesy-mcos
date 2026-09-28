# Marquee Console

## Compilation

Compile the source code using [CMake](https://cmake.org/) (platform independent):

```shell
cmake -S . -B ./build -DCMAKE_CXX_STANDARD=20
cmake --build ./build --target marquee-console
```

> [!NOTE]
> The source code can alse be compiled directly using a compiler (e.g., LLVM Clang, GCC):
>
> ```shell
> mkdir bin
> clang++ ./src/main.cpp -std=c++20 -o ./bin/marquee-console
> ```

## Usage

```sh
./bin/marquee-console
```

## Group Members

- Panaligan, Louis Raphael
- Lopez, Kent Xavier
