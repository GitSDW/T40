#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstring> // For strlen()
#include <cstdint> // For uint8_t and uint32_t

#include <opencv2/opencv.hpp>
#include "cxx_util.h"

using namespace cv;
using namespace std;

int scanf_cmd(void)
{
    int buf;
    std::cout << "Insert Commend : ";
    std::scanf("%d", &buf);

    return buf;
}

int scanf_index(void)
{
    int buf;
    std::scanf("%d", &buf);

    return buf;
}

int scanf_string(char *buf)
{
    // char buf[30];
    std::scanf("%s", buf);

    return 1;
}

// MD5 해시를 계산하기 위한 보조 함수들
constexpr uint32_t leftRotate(uint32_t x, uint32_t c) {
    return (x << c) | (x >> (32 - c));
}

void md5Transform(uint32_t state[4], const uint8_t block[64]) {
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
    uint32_t m[16];
    for (int i = 0; i < 16; ++i) {
        m[i] = (uint32_t(block[i * 4 + 0]) << 0) |
               (uint32_t(block[i * 4 + 1]) << 8) |
               (uint32_t(block[i * 4 + 2]) << 16) |
               (uint32_t(block[i * 4 + 3]) << 24);
    }

    // Round 1
    const uint32_t k[] = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821
    };
    for (int i = 0; i < 16; ++i) {
        uint32_t f = (b & c) | ((~b) & d);
        uint32_t g = i;
        uint32_t temp = d;
        d = c;
        c = b;
        b = b + leftRotate((a + f + k[i] + m[g]), 7);
        a = temp;
    }

    // Round 2
    const uint32_t k2[] = {
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a
    };
    for (int i = 0; i < 16; ++i) {
        uint32_t f = (d & b) | ((~d) & c);
        uint32_t g = (5 * i + 1) % 16;
        uint32_t temp = d;
        d = c;
        c = b;
        b = b + leftRotate((a + f + k2[i] + m[g]), 12);
        a = temp;
    }

    // Round 3
    const uint32_t k3[] = {
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665
    };
    for (int i = 0; i < 16; ++i) {
        uint32_t f = b ^ c ^ d;
        uint32_t g = (3 * i + 5) % 16;
        uint32_t temp = d;
        d = c;
        c = b;
        b = b + leftRotate((a + f + k3[i] + m[g]), 17);
        a = temp;
    }

    // Round 4
    const uint32_t k4[] = {
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
    };
    for (int i = 0; i < 16; ++i) {
        uint32_t f = c ^ (b | (~d));
        uint32_t g = (7 * i) % 16;
        uint32_t temp = d;
        d = c;
        c = b;
        b = b + leftRotate((a + f + k4[i] + m[g]), 22);
        a = temp;
    }

    // Update state
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    // Clear sensitive information
    memset(m, 0, sizeof(m));
}

// 파일의 MD5 해시를 계산하는 함수
std::string calculate_md5(const std::string& file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("파일을 열 수 없습니다.");
    }

    // MD5 해시를 계산하기 위한 변수 및 상수
    constexpr int buffer_size = 64;
    uint32_t state[4] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
    uint8_t buffer[buffer_size];
    uint64_t file_length = 0;
    while (!file.eof()) {
        file.read(reinterpret_cast<char*>(buffer), buffer_size);
        auto bytesRead = file.gcount();
        file_length += bytesRead;
        if (bytesRead > 0) {
            if (bytesRead < buffer_size) {
                // Padding
                buffer[bytesRead++] = 0x80;
                while (bytesRead < buffer_size) {
                    buffer[bytesRead++] = 0;
                }
            }
            md5Transform(state, buffer);
        }
    }

    // Append padding and length
    uint64_t total_bits = file_length * 8;
    buffer[0] = total_bits;
    buffer[1] = total_bits >> 8;
    buffer[2] = total_bits >> 16;
    buffer[3] = total_bits >> 24;
    buffer[4] = total_bits >> 32;
    buffer[5] = total_bits >> 40;
    buffer[6] = total_bits >> 48;
    buffer[7] = total_bits >> 56;
    memset(buffer + 8, 0, buffer_size - 8);
    md5Transform(state, buffer);

    // Convert state to string
    std::stringstream md5_stream;
    for (int i = 0; i < 4; ++i) {
        md5_stream << std::hex << std::setw(8) << std::setfill('0') << state[i];
    }

    return md5_stream.str();
}

// 파일의 MD5 해시를 검증하는 함수
bool verify_md5(const std::string& file_path, const std::string& expected_md5) {
    std::string actual_md5;
    try {
        actual_md5 = calculate_md5(file_path);
    } catch (const std::exception& e) {
        std::cerr << "에러: " << e.what() << std::endl;
        return false;
    }
    return actual_md5 == expected_md5;
}

int test_hash222(char *filepath) {
    try {
        std::string file_path = filepath;
        std::string make_md5 = calculate_md5(file_path);
        std::cout << file_path << " HASH : " << make_md5 << std::endl;
    } catch (Exception& e) {
        std::cout << "Fail to Get Hash!" << std::endl;
    }

    return 0;
}
