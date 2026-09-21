# pa3q
pa3q - a simple, lightweight and high-ratio file archiver written in C++ (C++20).<br>
It utilizes a context mixing compression algorithm combined with arithmetic (range) coding and dynamic modeling.

## Building and Testing
Detailed instructions on how to build and test the project on Windows, Linux, and macOS are in [BUILD.md](BUILD.md)
<br>

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

#### The project is divided into modules:
* Integer Stretch/Squash arithmetic (written in C99 and verified with Frama-C)
* Predictor
* Encoder
* Compressor
* Decompressor
* Command line interface

<br>
The project targets 64-bit systems.

***Note***: The development is still in progress (the project version is 0.3). Compression ratio and speed might be improved in the future.
