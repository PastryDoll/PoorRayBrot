# #!/bin/sh

set -xe

clang++ -std=c++11 -O3 -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL ./lib/libraylib.a main.cpp -o PoorDelBrot -I./lib