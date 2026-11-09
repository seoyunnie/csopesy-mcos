# OS Mockup

The `main()` function for this program is located at [`./src/main.cpp`](./src/main.cpp). The last commit before
submission is tagged by [`mco1-submission`](https://github.com/seoyunnie/csopesy-mcos/tree/mo1-submission).

## Compilation

Compile the source code using [CMake](https://cmake.org/) (platform independent):

```shell
cmake -B ./build -DCMAKE_CXX_STANDARD=20
cmake --build ./build --target process-scheduler
```

> [!NOTE]
> The source code can alse be compiled directly using a compiler (e.g., LLVM Clang, GCC):
>
> ```shell
> mkdir bin
> clang++ ./src/*.cpp -I ./src -std=c++20 -o ./bin/process-scheduler
> ```

## Usage

```sh
cd ./bin
./process-scheduler
```

## Group Members

- Panaligan, Louis Raphael
- Lopez, Kent Xavier
