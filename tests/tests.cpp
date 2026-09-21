#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <vector>

#include "predictor.hpp"
#include "compressor.hpp"
#include "decompressor.hpp"


TEST(PredictorTest, initial_boundaries) {
    Predictor p;
    uint64_t prob = p.next_bit_probability();
    EXPECT_GT(prob, 0);
    EXPECT_LT(prob, 0xFFFFFFFFFFFFFFFF);
}

TEST(PredictorTest, model_updating) {
    Predictor p;
    uint64_t initial_prob = p.next_bit_probability();
    for (int i = 0; i < 100; ++i) {
        p.update_model(false);
    }
    uint64_t new_prob = p.next_bit_probability();
    EXPECT_NE(initial_prob, new_prob);
}


class ArchiverTest : public ::testing::Test {
protected:
    const std::string input_file = "test_input.bin";
    const std::string archive_file = "test_archive.pa3q";
    const std::string output_file = "test_output.bin";
    void TearDown() override {
        std::filesystem::remove(input_file);
        std::filesystem::remove(archive_file);
        std::filesystem::remove(output_file);
    }
    void create_file(const std::string& path, const std::vector<char>& content) {
        std::ofstream ofs(path, std::ios::binary);
        ofs.write(content.data(), content.size());
    }
    bool files_are_equal(const std::string& path1, const std::string& path2) {
        std::ifstream f1(path1, std::ios::binary);
        std::ifstream f2(path2, std::ios::binary);
        if (!f1.is_open() or !f2.is_open()) { return false; }
        if (std::filesystem::file_size(path1) != std::filesystem::file_size(path2)) { return false; }
        std::istreambuf_iterator<char> it1(f1), it2(f2), end;
        while (it1 != end && it2 != end) {
            if (*it1 != *it2) { return false; }
            it1++; 
            it2++;
        }
        return true;
    }

    void run_round_trip_test(const std::vector<char>& data) {
        create_file(input_file, data);
        Compressor comp(input_file, archive_file);
        comp.compress();
        Decompressor decomp(archive_file, output_file);
        decomp.decompress();
        EXPECT_TRUE(files_are_equal(input_file, output_file));
    }
};


TEST_F(ArchiverTest, empty_file) {
    run_round_trip_test(std::vector<char>{});
}

TEST_F(ArchiverTest, small_file) {
    std::string text = "Tralala lala, Tralala tralaladdy, Tralala lala, Tralala lala.";
    std::vector<char> data(text.begin(), text.end());
    run_round_trip_test(data);
}

TEST_F(ArchiverTest, redundant_data) {
    std::vector<char> data(10000, 'A');
    run_round_trip_test(data);
}

TEST_F(ArchiverTest, random_data) {
    std::vector<char> data(5000);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(0, 255);
    for (char& byte : data) {
        byte = static_cast<char>(dist(rng));
    }
    run_round_trip_test(data);
}

TEST_F(ArchiverTest, throwing_missing_files_compressor) {
    EXPECT_THROW({
        Compressor comp("this_file_does_not_exist.bin", archive_file);
    }, std::runtime_error);
}

TEST_F(ArchiverTest, throwing_missing_files_decompressor) {
    EXPECT_THROW({
        Decompressor decomp("this_archive_does_not_exist.pa3q", output_file);
    }, std::runtime_error);
}

TEST_F(ArchiverTest, single_byte) {
    run_round_trip_test({'Z'});
}

TEST_F(ArchiverTest, full_byte_spectrum) {
    std::vector<char> data(256);
    for (int i = 0; i < 256; ++i) {
        data[i] = static_cast<char>(i);
    }
    run_round_trip_test(data);
}

TEST_F(ArchiverTest, compression_ratio_efficiency) {
    std::vector<char> data(20000, 'X');
    create_file(input_file, data);

    Compressor comp(input_file, archive_file);
    comp.compress();

    ASSERT_TRUE(std::filesystem::exists(archive_file));
    EXPECT_LT(std::filesystem::file_size(archive_file), std::filesystem::file_size(input_file) / 2);

    Decompressor decomp(archive_file, output_file);
    decomp.decompress();
    EXPECT_TRUE(files_are_equal(input_file, output_file));
}

TEST_F(ArchiverTest, throwing_corrupted_archive) {
    std::vector<char> corrupted_header = {'B', 'A', 'D', '!', 0x01, 0x02, 0x03};
    create_file(archive_file, corrupted_header);
    EXPECT_THROW({
        Decompressor decomp(archive_file, output_file);
        decomp.decompress();
    }, std::exception);
}