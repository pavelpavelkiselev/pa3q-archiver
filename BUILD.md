## How to build:
### Windows:
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    cmake --build . --config Release --target pa3q
    cmake --build . --config Debug --target pa3q_tests
### Linux/MacOS:
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    cmake --build .
### Linux (Make):
    make
On Windows executable file is located in **.\build\bin\Release**  <br>
On Linux and MacOS executable file is located in **./build/bin/**  <br>
On Linux (if Make used) executable file is located in **./bin/**  <br>
<br>

## How to test:
### On Windows:
    .\build\bin\Debug\pa3q_tests.exe
### On Linux, MacOS:
    ./build/bin/pa3q_tests
### Linux (Make, gtest module needs to be installed):
    make test MODE=debug

## Verifying integer arithmetic module with Frama-C:
    cd src
    frama-c arithmetic.h -wp -wp-rte -wp-timeout 10 -wp-prover alt-ergo -then -report

<br>From 57 specifications 56 are Completely validated and 1 is Considered valid
(Stretch LUT monotonicity is taken as an axiom). Instead of Alt-Ergo, any other SMT solver can be used (e.g., CVC5 and Z3).

## How to use:
### To compress:
    pa3q.exe -c <input file> <output file>
### To decompress:
    pa3q.exe -d <input file> <output file>

## Compression example:
    ./pa3q -c enwik9 enwik9.p3q
    Compressing: ...
    Compression successful!
    Compression time (seconds):	658.799
    File compressed from 1000000000 to 407770751 bytes.