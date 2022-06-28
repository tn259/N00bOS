#!/bin/bash
echo "Running clang-format"
find . -name '*.cpp' -o -name '*.h' -o -name '*.hpp' | xargs clang-format -i
make clean && bear make all # generate clang compile database - https://github.com/rizsotto/Bear
echo "Running clang-tidy"
find . -name '*.cpp' -o -name '*.h' -o -name '*.hpp' | xargs clang-tidy -p . > clang_tidy.out