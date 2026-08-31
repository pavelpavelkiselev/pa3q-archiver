# pa3q
pa3q - a simple, lightweight and high-ratio file archiver written in C++ (C++20).<br>
It utilizes a context mixing compression algorithm combined with arithmetic (range) coding and dynamic modeling.

### How to build:
### Windows/MacOS/Linux:
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    cmake --build .
### Linux (Make):
    make
On Windows executable file is located in .\build\bin\  <br>
On Linux and MacOS executable file is located in ./build/bin/  <br>
On Linux (if Make used) executable file is located in ./bin/  <br>
<br>
### How to use:
### To compress:
    pa3q.exe -c <input file> <output file>
### To decompress:
    pa3q.exe -d <input file> <output file>

### How to test:
### On Windows:
    .\build\bin\pa3q_tests.exe
### On Linux, MacOS:
    ./build/bin/pa3q_tests
### Linux (Make, gtest module needs to be installed):
    make test MODE=debug

### Compression example:
    ./pa3q -c enwik9 enwik9.p3q
    Compressing: ...
    Compression successful!
    Compression time (seconds):	333.603
    File compressed from 1000000000 to 229659357 bytes.

<br>The project is divided into modules:
* Predictor
* Encoder
* Compressor
* Decompressor
* Command line interface
<br>
The project targets 64-bit systems.

***Note***: Current version (0.2) is experimental. The predictor module relies heavily on floating-point arithmetic ('double' type), which may cause compression errors due to precision loss and differences across platforms.
