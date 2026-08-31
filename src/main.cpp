/**
 * @file main.cpp
 * @brief Archiver CLI.
 */

#include "compressor.hpp"
#include "decompressor.hpp"
#include <chrono>
#include <filesystem>
#include <iostream>

/**
 * @brief Main execution function and a CLI.
 * @param argc Argument count.
 * @param argv Command line arguments ("-c"/"-d", input_path, output_path).
 * @return 0 on success, 1 on failure.
 */
int main(int argc, char **argv) {
    if ((argc != 4) or (argv[1][0] != '-') || ((argv[1][1] != 'c') and (argv[1][1] != 'd'))) {
        std::cerr << "Compression:\tpa3q -c <input file> <output file>\n"
                  << "Decompression:\tpa3q -d <input file> <output file>\n";
        return 1;
    }

    const std::string input_path = argv[2];
    const std::string output_path = argv[3];

    if (argv[1][1] == 'c') {
        std::cout << "Compressing: ..." << '\n';
        try {
            Compressor compressor(input_path, output_path);

            const auto start = std::chrono::high_resolution_clock::now();
            compressor.compress();
            const auto end = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double> time = end - start;

            std::cout << "Compression successful!\n";
            std::cout << "Compression time (seconds):\t" << time.count() << '\n';
            std::cout << "File compressed from " << std::filesystem::file_size(input_path) << " to "
                      << std::filesystem::file_size(output_path) << " bytes.\n";

        } catch (const std::exception &e) {
            std::cerr << "Compression error: " << e.what() << '\n';
            return 1;
        }
    } else {
        std::cout << "Decompressing: ..." << '\n';
        try {
            Decompressor decompressor(input_path, output_path);

            const auto start = std::chrono::high_resolution_clock::now();
            decompressor.decompress();
            const auto end = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double> time = end - start;

            std::cout << "Decompression successful!\n";
            std::cout << "Decompression time (seconds):\t" << time.count() << '\n';
            std::cout << "File decompressed from " << std::filesystem::file_size(input_path)
                      << " to " << std::filesystem::file_size(output_path) << " bytes.\n";

        } catch (const std::exception &e) {
            std::cerr << "Decompression error: " << e.what() << '\n';
            return 1;
        }
    }

    return 0;
}
