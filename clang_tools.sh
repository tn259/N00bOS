#!/bin/bash
find . -name '*.cpp' -o -name '*.h' -o -name '*.hpp' | xargs clang-format -i
make clean && bear make all # general clang compile database - https://github.com/rizsotto/Bear
find . -name '*.cpp' -o -name '*.h' -o -name '*.hpp' | xargs clang-tidy -p . --checks='*' > clang_tidy.out
