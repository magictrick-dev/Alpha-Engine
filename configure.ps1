cmake -B ./build `
    -G Ninja `
    -DCMAKE_EXPORT_COMPILE_COMMANDS=on `
    -DCMAKE_CXX_STANDARD=23 `
    -DCMAKE_CXX_COMPILER=clang-cl `
    -DCMAKE_C_COMPILER=clang-cl `
    -DCMAKE_BUILD_TYPE=Release
