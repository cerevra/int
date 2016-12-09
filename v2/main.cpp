
#include "int.h"

#include <iostream>
#include <iomanip>

using namespace std;

#define assert(a, info) {if (!(a)) throw std::runtime_error(#a ", line:" + std::to_string(__LINE__) + ", info:" + info );}

template<size_t Bits, bool Sgn>
void printNum(const wide_int<Bits, Sgn>& n) {
    for (auto c : n.m_arr) {
        std::cerr << std::hex << std::setw(16) << std::setfill('0') << uint64_t(c) << std::endl;
    }
}

void printNum(uint64_t n) {
    std::cerr << std::hex << std::setw(16) << std::setfill('0') << n << std::endl;
}

void testCtors() {
    int512_t a1 = 0;
    for (auto c : a1.m_arr) {
        assert(c == 0, "");
    }
    uint512_t b1 = 0;
    for (auto c : b1.m_arr) {
        assert(c == 0, "");
    }

    int512_t a2 = 13;
    for (int idx = 0; idx < 63; ++idx) {
        assert(a2.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a2.m_arr[63] == 13, "");
    uint512_t b2 = 13;
    for (int idx = 0; idx < 63; ++idx) {
        assert(b2.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b2.m_arr[63] == 13, "");

    int512_t a3 = -13;
    for (int idx = 0; idx < 63; ++idx) {
        assert(a3.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(a3.m_arr[63] == 0xf3, "");
    uint512_t b3 = -13;
    for (int idx = 0; idx < 63; ++idx) {
        assert(b3.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b3.m_arr[63] == 0xf3, "");

    int512_t a4 = int128_t(13);
    for (int idx = 0; idx < 63; ++idx) {
        assert(a4.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a4.m_arr[63] == 13, "");
    uint512_t b4 = uint128_t(13);
    for (int idx = 0; idx < 63; ++idx) {
        assert(b4.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b4.m_arr[63] == 13, "");

    int512_t a5 = uint128_t(13);
    for (int idx = 0; idx < 7; ++idx) {
        assert(a5.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a5.m_arr[63] == 13, "");
    uint512_t b5 = int128_t(13);
    for (int idx = 0; idx < 7; ++idx) {
        assert(b5.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b5.m_arr[63] == 13, "");

    int512_t a6 = uint128_t(-13);
    for (int idx = 0; idx < 48; ++idx) {
        assert(a6.m_arr[idx] == 0, std::to_string(idx));
    }
    for (int idx = 48; idx < 63; ++idx) {
        assert(a6.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(a6.m_arr[63] == 0xf3, "");
    uint512_t b6 = int128_t(-13);
    for (int idx = 0; idx < 63; ++idx) {
        assert(b6.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b6.m_arr[63] == 0xf3, "");

    int512_t a7 = int128_t(-13);
    for (int idx = 0; idx < 63; ++idx) {
        assert(a7.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(a7.m_arr[63] == 0xf3, "");
    uint512_t b7 = uint128_t(-13);
    for (int idx = 0; idx < 48; ++idx) {
        assert(b7.m_arr[idx] == 0, std::to_string(idx));
    }
    for (int idx = 56; idx < 63; ++idx) {
        assert(b7.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b7.m_arr[63] == 0xf3, "");
}

void testShifts() {
    //left
    uint512_t a1 = 0;
    for (auto c : uint512_t::shift_left(a1, 0).m_arr) {
        assert(c == 0, "");
    }
    uint512_t a2 = 0;
    for (auto c : uint512_t::shift_left(a2, 15).m_arr) {
        assert(c == 0, "");
    }

    uint512_t a3 = 0;
    a3.m_arr[56] = 0xf1;
    a3.m_arr[57] = 0xf2;
    a3.m_arr[58] = 0xf3;
    a3.m_arr[59] = 0xf4;
    a3.m_arr[60] = 0xf5;
    a3.m_arr[61] = 0xf6;
    a3.m_arr[62] = 0xf7;
    a3.m_arr[63] = 0xf8;
    uint512_t a31 = uint512_t::shift_left(a3, 8);
    for (int idx = 0; idx < 55; ++idx) {
        assert(a31.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a31.m_arr[55] == 0xf1, "");
    assert(a31.m_arr[56] == 0xf2, "");
    assert(a31.m_arr[57] == 0xf3, "");
    assert(a31.m_arr[58] == 0xf4, "");
    assert(a31.m_arr[59] == 0xf5, "");
    assert(a31.m_arr[60] == 0xf6, "");
    assert(a31.m_arr[61] == 0xf7, "");
    assert(a31.m_arr[62] == 0xf8, "");
    assert(a31.m_arr[63] == 0x00, "");

    uint512_t a32 = uint512_t::shift_left(a3, 16);
    for (int idx = 0; idx < 54; ++idx) {
        assert(a32.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a32.m_arr[54] == 0xf1, "");
    assert(a32.m_arr[55] == 0xf2, "");
    assert(a32.m_arr[56] == 0xf3, "");
    assert(a32.m_arr[57] == 0xf4, "");
    assert(a32.m_arr[58] == 0xf5, "");
    assert(a32.m_arr[59] == 0xf6, "");
    assert(a32.m_arr[60] == 0xf7, "");
    assert(a32.m_arr[61] == 0xf8, "");
    assert(a32.m_arr[62] == 0x00, "");
    assert(a32.m_arr[63] == 0x00, "");

    uint512_t a33 = uint512_t::shift_left(a3, 4);
    for (int idx = 0; idx < 55; ++idx) {
        assert(a33.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a33.m_arr[55] == 0x0f, "");
    assert(a33.m_arr[56] == 0x1f, "");
    assert(a33.m_arr[57] == 0x2f, "");
    assert(a33.m_arr[58] == 0x3f, "");
    assert(a33.m_arr[59] == 0x4f, "");
    assert(a33.m_arr[60] == 0x5f, "");
    assert(a33.m_arr[61] == 0x6f, "");
    assert(a33.m_arr[62] == 0x7f, "");
    assert(a33.m_arr[63] == 0x80, "");

    uint512_t a34 = uint512_t::shift_left(a3, 64);
    for (int idx = 0; idx < 48; ++idx) {
        assert(a34.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a34.m_arr[48] == 0xf1, "");
    assert(a34.m_arr[49] == 0xf2, "");
    assert(a34.m_arr[50] == 0xf3, "");
    assert(a34.m_arr[51] == 0xf4, "");
    assert(a34.m_arr[52] == 0xf5, "");
    assert(a34.m_arr[53] == 0xf6, "");
    assert(a34.m_arr[54] == 0xf7, "");
    assert(a34.m_arr[55] == 0xf8, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(a34.m_arr[idx] == 0, std::to_string(idx));
    }

    uint512_t a35 = uint512_t::shift_left(a3, 256);
    for (int idx = 0; idx < 24; ++idx) {
        assert(a35.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a35.m_arr[24] == 0xf1, "");
    assert(a35.m_arr[25] == 0xf2, "");
    assert(a35.m_arr[26] == 0xf3, "");
    assert(a35.m_arr[27] == 0xf4, "");
    assert(a35.m_arr[28] == 0xf5, "");
    assert(a35.m_arr[29] == 0xf6, "");
    assert(a35.m_arr[30] == 0xf7, "");
    assert(a35.m_arr[31] == 0xf8, "");
    for (int idx = 32; idx < 63; ++idx) {
        assert(a35.m_arr[idx] == 0, std::to_string(idx));
    }

    uint512_t a36 = uint512_t::shift_left(a3, 260);
    for (int idx = 0; idx < 23; ++idx) {
        assert(a36.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a36.m_arr[23] == 0x0f, "");
    assert(a36.m_arr[24] == 0x1f, "");
    assert(a36.m_arr[25] == 0x2f, "");
    assert(a36.m_arr[26] == 0x3f, "");
    assert(a36.m_arr[27] == 0x4f, "");
    assert(a36.m_arr[28] == 0x5f, "");
    assert(a36.m_arr[29] == 0x6f, "");
    assert(a36.m_arr[30] == 0x7f, "");
    assert(a36.m_arr[31] == 0x80, "");
    for (int idx = 32; idx < 63; ++idx) {
        assert(a36.m_arr[idx] == 0, std::to_string(idx));
    }

    //right uint
    uint512_t a4 = 0;
    for (auto c : uint512_t::shift_right(a4, 0).m_arr) {
        assert(c == 0, "");
    }
    uint512_t a5 = 0;
    for (auto c : uint512_t::shift_right(a5, 15).m_arr) {
        assert(c == 0, "");
    }

    uint512_t a6 = 0;
    a6.m_arr[56] = 0xf1;
    a6.m_arr[57] = 0xf2;
    a6.m_arr[58] = 0xf3;
    a6.m_arr[59] = 0xf4;
    a6.m_arr[60] = 0xf5;
    a6.m_arr[61] = 0xf6;
    a6.m_arr[62] = 0xf7;
    a6.m_arr[63] = 0xf8;
    a6.m_arr[0] = 0xf1;
    a6.m_arr[1] = 0xf2;
    a6.m_arr[2] = 0xf3;
    a6.m_arr[3] = 0xf4;
    a6.m_arr[4] = 0xf5;
    a6.m_arr[5] = 0xf6;
    a6.m_arr[6] = 0xf7;
    a6.m_arr[7] = 0xf8;
    uint512_t a61 = uint512_t::shift_right(a6, 8);
    assert(a61.m_arr[0] == 0, "");
    assert(a61.m_arr[1] == 0xf1, "");
    assert(a61.m_arr[2] == 0xf2, "");
    assert(a61.m_arr[3] == 0xf3, "");
    assert(a61.m_arr[4] == 0xf4, "");
    assert(a61.m_arr[5] == 0xf5, "");
    assert(a61.m_arr[6] == 0xf6, "");
    assert(a61.m_arr[7] == 0xf7, "");
    assert(a61.m_arr[8] == 0xf8, "");
    for (int idx = 9; idx < 57; ++idx) {
        assert(a61.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a61.m_arr[57] == 0xf1, "");
    assert(a61.m_arr[58] == 0xf2, "");
    assert(a61.m_arr[59] == 0xf3, "");
    assert(a61.m_arr[60] == 0xf4, "");
    assert(a61.m_arr[61] == 0xf5, "");
    assert(a61.m_arr[62] == 0xf6, "");
    assert(a61.m_arr[63] == 0xf7, "");

    uint512_t a62 = uint512_t::shift_right(a6, 16);
    assert(a62.m_arr[0] == 0, "");
    assert(a62.m_arr[1] == 0, "");
    assert(a62.m_arr[2] == 0xf1, "");
    assert(a62.m_arr[3] == 0xf2, "");
    assert(a62.m_arr[4] == 0xf3, "");
    assert(a62.m_arr[5] == 0xf4, "");
    assert(a62.m_arr[6] == 0xf5, "");
    assert(a62.m_arr[7] == 0xf6, "");
    assert(a62.m_arr[8] == 0xf7, "");
    assert(a62.m_arr[9] == 0xf8, "");
    for (int idx = 10; idx < 58; ++idx) {
        assert(a62.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a62.m_arr[58] == 0xf1, "");
    assert(a62.m_arr[59] == 0xf2, "");
    assert(a62.m_arr[60] == 0xf3, "");
    assert(a62.m_arr[61] == 0xf4, "");
    assert(a62.m_arr[62] == 0xf5, "");
    assert(a62.m_arr[63] == 0xf6, "");

    uint512_t a63 = uint512_t::shift_right(a6, 4);
    assert(a63.m_arr[0] == 0x0f, "");
    assert(a63.m_arr[1] == 0x1f, "");
    assert(a63.m_arr[2] == 0x2f, "");
    assert(a63.m_arr[3] == 0x3f, "");
    assert(a63.m_arr[4] == 0x4f, "");
    assert(a63.m_arr[5] == 0x5f, "");
    assert(a63.m_arr[6] == 0x6f, "");
    assert(a63.m_arr[7] == 0x7f, "");
    assert(a63.m_arr[8] == 0x80, "");
    for (int idx = 9; idx < 56; ++idx) {
        assert(a63.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a63.m_arr[56] == 0x0f, "");
    assert(a63.m_arr[57] == 0x1f, "");
    assert(a63.m_arr[58] == 0x2f, "");
    assert(a63.m_arr[59] == 0x3f, "");
    assert(a63.m_arr[60] == 0x4f, "");
    assert(a63.m_arr[61] == 0x5f, "");
    assert(a63.m_arr[62] == 0x6f, "");
    assert(a63.m_arr[63] == 0x7f, "");

    uint512_t a64 = uint512_t::shift_right(a6, 64);
    for (int idx = 0; idx < 8; ++idx) {
        assert(a64.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a64.m_arr[8] == 0xf1, "");
    assert(a64.m_arr[9] == 0xf2, "");
    assert(a64.m_arr[10] == 0xf3, "");
    assert(a64.m_arr[11] == 0xf4, "");
    assert(a64.m_arr[12] == 0xf5, "");
    assert(a64.m_arr[13] == 0xf6, "");
    assert(a64.m_arr[14] == 0xf7, "");
    assert(a64.m_arr[15] == 0xf8, "");
    for (int idx = 16; idx < 63; ++idx) {
        assert(a64.m_arr[idx] == 0, std::to_string(idx));
    }

    uint512_t a65 = uint512_t::shift_right(a6, 256);
    for (int idx = 0; idx < 32; ++idx) {
        assert(a65.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a65.m_arr[32] == 0xf1, "");
    assert(a65.m_arr[33] == 0xf2, "");
    assert(a65.m_arr[34] == 0xf3, "");
    assert(a65.m_arr[35] == 0xf4, "");
    assert(a65.m_arr[36] == 0xf5, "");
    assert(a65.m_arr[37] == 0xf6, "");
    assert(a65.m_arr[38] == 0xf7, "");
    assert(a65.m_arr[39] == 0xf8, "");
    for (int idx = 40; idx < 63; ++idx) {
        assert(a65.m_arr[idx] == 0, std::to_string(idx));
    }

    uint512_t a66 = uint512_t::shift_right(a6, 260);
    for (int idx = 0; idx < 32; ++idx) {
        assert(a66.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a66.m_arr[32] == 0x0f, "");
    assert(a66.m_arr[33] == 0x1f, "");
    assert(a66.m_arr[34] == 0x2f, "");
    assert(a66.m_arr[35] == 0x3f, "");
    assert(a66.m_arr[36] == 0x4f, "");
    assert(a66.m_arr[37] == 0x5f, "");
    assert(a66.m_arr[38] == 0x6f, "");
    assert(a66.m_arr[39] == 0x7f, "");
    assert(a66.m_arr[40] == 0x80, "");
    for (int idx = 41; idx < 63; ++idx) {
        assert(a66.m_arr[idx] == 0, std::to_string(idx));
    }

    //right int > 0
    int512_t b4 = 0;
    for (auto c : int512_t::shift_right(b4, 0).m_arr) {
        assert(c == 0, "");
    }
    int512_t b5 = 0;
    for (auto c : int512_t::shift_right(b5, 15).m_arr) {
        assert(c == 0, "");
    }

    int512_t b6 = 0;
    b6.m_arr[0] = 0x71;
    b6.m_arr[1] = 0xf2;
    b6.m_arr[2] = 0xf3;
    b6.m_arr[3] = 0xf4;
    b6.m_arr[4] = 0xf5;
    b6.m_arr[5] = 0xf6;
    b6.m_arr[6] = 0xf7;
    b6.m_arr[7] = 0xf8;
    b6.m_arr[56] = 0xf1;
    b6.m_arr[57] = 0xf2;
    b6.m_arr[58] = 0xf3;
    b6.m_arr[59] = 0xf4;
    b6.m_arr[60] = 0xf5;
    b6.m_arr[61] = 0xf6;
    b6.m_arr[62] = 0xf7;
    b6.m_arr[63] = 0xf8;

    int512_t b61 = int512_t::shift_right(b6, 8);
    assert(b61.m_arr[0] == 0   , "");
    assert(b61.m_arr[1] == 0x71, "");
    assert(b61.m_arr[2] == 0xf2, "");
    assert(b61.m_arr[3] == 0xf3, "");
    assert(b61.m_arr[4] == 0xf4, "");
    assert(b61.m_arr[5] == 0xf5, "");
    assert(b61.m_arr[6] == 0xf6, "");
    assert(b61.m_arr[7] == 0xf7, "");
    assert(b61.m_arr[8] == 0xf8, "");
    for (int idx = 9; idx < 57; ++idx) {
        assert(b61.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b61.m_arr[57] == 0xf1, "");
    assert(b61.m_arr[58] == 0xf2, "");
    assert(b61.m_arr[59] == 0xf3, "");
    assert(b61.m_arr[60] == 0xf4, "");
    assert(b61.m_arr[61] == 0xf5, "");
    assert(b61.m_arr[62] == 0xf6, "");
    assert(b61.m_arr[63] == 0xf7, "");

    int512_t b62 = int512_t::shift_right(b6, 16);
    assert(b62.m_arr[0] == 0   , "");
    assert(b62.m_arr[1] == 0   , "");
    assert(b62.m_arr[2] == 0x71, "");
    assert(b62.m_arr[3] == 0xf2, "");
    assert(b62.m_arr[4] == 0xf3, "");
    assert(b62.m_arr[5] == 0xf4, "");
    assert(b62.m_arr[6] == 0xf5, "");
    assert(b62.m_arr[7] == 0xf6, "");
    assert(b62.m_arr[8] == 0xf7, "");
    assert(b62.m_arr[9] == 0xf8, "");
    for (int idx = 10; idx < 58; ++idx) {
        assert(b62.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b62.m_arr[58] == 0xf1, "");
    assert(b62.m_arr[59] == 0xf2, "");
    assert(b62.m_arr[60] == 0xf3, "");
    assert(b62.m_arr[61] == 0xf4, "");
    assert(b62.m_arr[62] == 0xf5, "");
    assert(b62.m_arr[63] == 0xf6, "");

    int512_t b63 = int512_t::shift_right(b6, 4);
    assert(b63.m_arr[0] == 0x07, "");
    assert(b63.m_arr[1] == 0x1f, "");
    assert(b63.m_arr[2] == 0x2f, "");
    assert(b63.m_arr[3] == 0x3f, "");
    assert(b63.m_arr[4] == 0x4f, "");
    assert(b63.m_arr[5] == 0x5f, "");
    assert(b63.m_arr[6] == 0x6f, "");
    assert(b63.m_arr[7] == 0x7f, "");
    assert(b63.m_arr[8] == 0x80, "");
    for (int idx = 9; idx < 56; ++idx) {
        assert(b63.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b63.m_arr[56] == 0x0f, "");
    assert(b63.m_arr[57] == 0x1f, "");
    assert(b63.m_arr[58] == 0x2f, "");
    assert(b63.m_arr[59] == 0x3f, "");
    assert(b63.m_arr[60] == 0x4f, "");
    assert(b63.m_arr[61] == 0x5f, "");
    assert(b63.m_arr[62] == 0x6f, "");
    assert(b63.m_arr[63] == 0x7f, "");

    int512_t b64 = int512_t::shift_right(b6, 64);
    assert(b64.m_arr[8] == 0x71, "");
    assert(b64.m_arr[9] == 0xf2, "");
    assert(b64.m_arr[10] == 0xf3, "");
    assert(b64.m_arr[11] == 0xf4, "");
    assert(b64.m_arr[12] == 0xf5, "");
    assert(b64.m_arr[13] == 0xf6, "");
    assert(b64.m_arr[14] == 0xf7, "");
    assert(b64.m_arr[15] == 0xf8, "");
    for (int idx = 16; idx < 63; ++idx) {
        assert(b64.m_arr[idx] == 0, std::to_string(idx));
    }

    int512_t b65 = int512_t::shift_right(b6, 256);
    for (int idx = 0; idx < 32; ++idx) {
        assert(b65.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b65.m_arr[32] == 0x71, "");
    assert(b65.m_arr[33] == 0xf2, "");
    assert(b65.m_arr[34] == 0xf3, "");
    assert(b65.m_arr[35] == 0xf4, "");
    assert(b65.m_arr[36] == 0xf5, "");
    assert(b65.m_arr[37] == 0xf6, "");
    assert(b65.m_arr[38] == 0xf7, "");
    assert(b65.m_arr[39] == 0xf8, "");
    for (int idx = 40; idx < 63; ++idx) {
        assert(b65.m_arr[idx] == 0, std::to_string(idx));
    }

    int512_t b66 = int512_t::shift_right(b6, 260);
    for (int idx = 0; idx < 32; ++idx) {
        assert(b66.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b66.m_arr[32] == 0x07, "");
    assert(b66.m_arr[33] == 0x1f, "");
    assert(b66.m_arr[34] == 0x2f, "");
    assert(b66.m_arr[35] == 0x3f, "");
    assert(b66.m_arr[36] == 0x4f, "");
    assert(b66.m_arr[37] == 0x5f, "");
    assert(b66.m_arr[38] == 0x6f, "");
    assert(b66.m_arr[39] == 0x7f, "");
    assert(b66.m_arr[40] == 0x80, "");
    for (int idx = 41; idx < 63; ++idx) {
        assert(b66.m_arr[idx] == 0, std::to_string(idx));
    }

    //right int < 0
    int512_t b7 = 0;
    b7.m_arr[0] = 0xf1;
    b7.m_arr[1] = 0xf2;
    b7.m_arr[2] = 0xf3;
    b7.m_arr[3] = 0xf4;
    b7.m_arr[4] = 0xf5;
    b7.m_arr[5] = 0xf6;
    b7.m_arr[6] = 0xf7;
    b7.m_arr[7] = 0xf8;
    b7.m_arr[56] = 0xf1;
    b7.m_arr[57] = 0xf2;
    b7.m_arr[58] = 0xf3;
    b7.m_arr[59] = 0xf4;
    b7.m_arr[60] = 0xf5;
    b7.m_arr[61] = 0xf6;
    b7.m_arr[62] = 0xf7;
    b7.m_arr[63] = 0xf8;
    int512_t b71 = int512_t::shift_right(b7, 8);
    assert(b71.m_arr[0] == 0xff, "");
    assert(b71.m_arr[1] == 0xf1, "");
    assert(b71.m_arr[2] == 0xf2, "");
    assert(b71.m_arr[3] == 0xf3, "");
    assert(b71.m_arr[4] == 0xf4, "");
    assert(b71.m_arr[5] == 0xf5, "");
    assert(b71.m_arr[6] == 0xf6, "");
    assert(b71.m_arr[7] == 0xf7, "");
    assert(b71.m_arr[8] == 0xf8, "");
    for (int idx = 9; idx < 57; ++idx) {
        assert(b71.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b71.m_arr[57] == 0xf1, "");
    assert(b71.m_arr[58] == 0xf2, "");
    assert(b71.m_arr[59] == 0xf3, "");
    assert(b71.m_arr[60] == 0xf4, "");
    assert(b71.m_arr[61] == 0xf5, "");
    assert(b71.m_arr[62] == 0xf6, "");
    assert(b71.m_arr[63] == 0xf7, "");

    int512_t b72 = int512_t::shift_right(b7, 16);
    assert(b72.m_arr[0] == 0xff, "");
    assert(b72.m_arr[1] == 0xff, "");
    assert(b72.m_arr[2] == 0xf1, "");
    assert(b72.m_arr[3] == 0xf2, "");
    assert(b72.m_arr[4] == 0xf3, "");
    assert(b72.m_arr[5] == 0xf4, "");
    assert(b72.m_arr[6] == 0xf5, "");
    assert(b72.m_arr[7] == 0xf6, "");
    assert(b72.m_arr[8] == 0xf7, "");
    assert(b72.m_arr[9] == 0xf8, "");
    for (int idx = 10; idx < 58; ++idx) {
        assert(b72.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b72.m_arr[58] == 0xf1, "");
    assert(b72.m_arr[59] == 0xf2, "");
    assert(b72.m_arr[60] == 0xf3, "");
    assert(b72.m_arr[61] == 0xf4, "");
    assert(b72.m_arr[62] == 0xf5, "");
    assert(b72.m_arr[63] == 0xf6, "");

    int512_t b73 = int512_t::shift_right(b7, 4);
    assert(b73.m_arr[0] == 0xff, "");
    assert(b73.m_arr[1] == 0x1f, "");
    assert(b73.m_arr[2] == 0x2f, "");
    assert(b73.m_arr[3] == 0x3f, "");
    assert(b73.m_arr[4] == 0x4f, "");
    assert(b73.m_arr[5] == 0x5f, "");
    assert(b73.m_arr[6] == 0x6f, "");
    assert(b73.m_arr[7] == 0x7f, "");
    assert(b73.m_arr[8] == 0x80, "");
    for (int idx = 9; idx < 56; ++idx) {
        assert(b73.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b73.m_arr[56] == 0x0f, "");
    assert(b73.m_arr[57] == 0x1f, "");
    assert(b73.m_arr[58] == 0x2f, "");
    assert(b73.m_arr[59] == 0x3f, "");
    assert(b73.m_arr[60] == 0x4f, "");
    assert(b73.m_arr[61] == 0x5f, "");
    assert(b73.m_arr[62] == 0x6f, "");
    assert(b73.m_arr[63] == 0x7f, "");

    int512_t b74 = int512_t::shift_right(b7, 64);
    for (int idx = 0; idx < 7; ++idx) {
        assert(b74.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b74.m_arr[8] == 0xf1, "");
    assert(b74.m_arr[9] == 0xf2, "");
    assert(b74.m_arr[10] == 0xf3, "");
    assert(b74.m_arr[11] == 0xf4, "");
    assert(b74.m_arr[12] == 0xf5, "");
    assert(b74.m_arr[13] == 0xf6, "");
    assert(b74.m_arr[14] == 0xf7, "");
    assert(b74.m_arr[15] == 0xf8, "");
    for (int idx = 16; idx < 64; ++idx) {
        assert(b74.m_arr[idx] == 0, std::to_string(idx));
    }

    int512_t b75 = int512_t::shift_right(b7, 256);
    for (int idx = 0; idx < 32; ++idx) {
        assert(b75.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b75.m_arr[32] == 0xf1, "");
    assert(b75.m_arr[33] == 0xf2, "");
    assert(b75.m_arr[34] == 0xf3, "");
    assert(b75.m_arr[35] == 0xf4, "");
    assert(b75.m_arr[36] == 0xf5, "");
    assert(b75.m_arr[37] == 0xf6, "");
    assert(b75.m_arr[38] == 0xf7, "");
    assert(b75.m_arr[39] == 0xf8, "");
    for (int idx = 40; idx < 63; ++idx) {
        assert(b75.m_arr[idx] == 0, std::to_string(idx));
    }

    int512_t b76 = int512_t::shift_right(b7, 260);
    for (int idx = 0; idx < 33; ++idx) {
        assert(b76.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b76.m_arr[33] == 0x1f, "");
    assert(b76.m_arr[34] == 0x2f, "");
    assert(b76.m_arr[35] == 0x3f, "");
    assert(b76.m_arr[36] == 0x4f, "");
    assert(b76.m_arr[37] == 0x5f, "");
    assert(b76.m_arr[38] == 0x6f, "");
    assert(b76.m_arr[39] == 0x7f, "");
    assert(b76.m_arr[40] == 0x80, "");
    for (int idx = 41; idx < 63; ++idx) {
        assert(b76.m_arr[idx] == 0, std::to_string(idx));
    }

    int512_t b8 = -13;
    int512_t b81 = int512_t::shift_right(b8, 1);
    for (int idx = 0; idx < 63; ++idx) {
        assert(b81.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b81.m_arr[63] == 0xf9, "");
}

void testAssign() {
    uint512_t a1 = 0;
    int512_t b1 = 0;

    a1 = 4;
    for (int idx = 0; idx < 63; ++idx) {
        assert(a1.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a1.m_arr[63] == 4, "");
    b1 = 4;
    for (int idx = 0; idx < 63; ++idx) {
        assert(b1.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b1.m_arr[63] == 4, "");

    a1 = -4;
    for (int idx = 0; idx < 63; ++idx) {
        assert(a1.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(a1.m_arr[63] == 0xfc, "");
    b1 = -4;
    for (int idx = 0; idx < 63; ++idx) {
        assert(b1.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b1.m_arr[63] == 0xfc, "");

    a1 = uint128_t(4);
    for (int idx = 0; idx < 63; ++idx) {
        assert(a1.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a1.m_arr[63] == 4, "");
    b1 = int128_t(4);
    for (int idx = 0; idx < 63; ++idx) {
        assert(b1.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b1.m_arr[63] == 4, "");

    a1 = uint128_t(-4);
    for (int idx = 0; idx < 48; ++idx) {
        assert(a1.m_arr[idx] == 0, std::to_string(idx));
    }
    for (int idx = 48; idx < 63; ++idx) {
        assert(a1.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(a1.m_arr[63] == 0xfc, "");
    b1 = int128_t(-4);
    for (int idx = 0; idx < 63; ++idx) {
        assert(b1.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b1.m_arr[63] == 0xfc, "");

    a1 = int128_t(4);
    for (int idx = 0; idx < 63; ++idx) {
        assert(a1.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a1.m_arr[63] == 4, "");
    b1 = uint128_t(4);
    for (int idx = 0; idx < 63; ++idx) {
        assert(b1.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b1.m_arr[63] == 4, "");

    a1 = int128_t(-4);
    for (int idx = 0; idx < 63; ++idx) {
        assert(a1.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(a1.m_arr[63] == 0xfc, "");
    b1 = uint128_t(-4);
    for (int idx = 0; idx < 48; ++idx) {
        assert(b1.m_arr[idx] == 0, std::to_string(idx));
    }
    for (int idx = 48; idx < 63; ++idx) {
        assert(a1.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b1.m_arr[63] == 0xfc, "");
}

void testOperatorPlus() {
    uint512_t a1 = 3;
    a1.m_arr[55] = 1;

    uint512_t a11 = uint512_t::operator_plus_T(a1, 5);
    for (int idx = 0; idx < 55; ++idx) {
        assert(a11.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a11.m_arr[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(a11.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a11.m_arr[63] == 8, "");

    uint512_t a12 = uint512_t::operator_plus_T(a1, 0);
    for (int idx = 0; idx < 55; ++idx) {
        assert(a12.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a12.m_arr[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(a12.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a12.m_arr[63] == 3, "");

    uint512_t a14 = uint512_t::operator_plus_T(a1, std::numeric_limits<uint64_t>::max());
    for (int idx = 0; idx < 55; ++idx) {
        assert(a14.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a14.m_arr[55] == 2, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(a14.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a14.m_arr[63] == 2, "");

    int512_t b1 = 3;
    b1.m_arr[55] = 1;

    int512_t b11 = int512_t::operator_plus_T(b1, 5);
    for (int idx = 0; idx < 55; ++idx) {
        assert(b11.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b11.m_arr[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(b11.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b11.m_arr[63] == 8, "");

    int512_t b12 = int512_t::operator_plus_T(b1, 0);
    for (int idx = 0; idx < 55; ++idx) {
        assert(b12.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b12.m_arr[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(b12.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b12.m_arr[63] == 3, "");

    int512_t b2 = -3;
    b2.m_arr[55] = -2;

    int512_t b22 = int512_t::operator_plus_T(b2, 0);
    for (int idx = 0; idx < 55; ++idx) {
        assert(b22.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b22.m_arr[55] == 0xfe, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(b22.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b22.m_arr[63] == 0xfd, "");

    int512_t b23 = int512_t::operator_plus_T(b2, uint16_t(5));
    for (int idx = 0; idx < 56; ++idx) {
        assert(b23.m_arr[idx] == 0xff, std::to_string(idx));
    }
    for (int idx = 56; idx < 63; ++idx) {
        assert(b23.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b23.m_arr[63] == 2, "");

    int512_t b3 = -3;

    int512_t b31 = int512_t::operator_plus_T(b3, 5);
    for (int idx = 0; idx < 63; ++idx) {
        assert(b31.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(static_cast<int64_t>(b31.m_arr[63]) == 2, "");

    int512_t b4 = -1;
    b4.m_arr[55] = -2;
    b4.m_arr[63] = 3;

    int512_t b41 = int512_t::operator_plus_T(b4, 5);
    for (int idx = 0; idx < 55; ++idx) {
        assert(b41.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b41.m_arr[55] == 0xfe, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(b41.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b41.m_arr[63] == 8, "");
}

void testOperatorTilda() {
    uint512_t a1 = 250;
    a1.m_arr[3] = 17;

    uint512_t a2 = uint512_t::operator_unary_tilda(a1);
    for (int idx = 0; idx < 3; ++idx) {
        assert(a2.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(a2.m_arr[3] == 0xee, "");
    for (int idx = 4; idx < 63; ++idx) {
        assert(a2.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(a2.m_arr[63] == 0x05, "");

    int512_t b1 = 250;
    b1.m_arr[3] = 17;

    int512_t b2 = int512_t::operator_unary_tilda(b1);
    for (int idx = 0; idx < 3; ++idx) {
        assert(b2.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b2.m_arr[3] == 0xee, "");
    for (int idx = 4; idx < 63; ++idx) {
        assert(b2.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b2.m_arr[63] == 0x05, "");

    int512_t b3 = -250;
    b3.m_arr[3] = 17;

    int512_t b4 = int512_t::operator_unary_tilda(b3);
    for (int idx = 0; idx < 3; ++idx) {
        assert(b4.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b4.m_arr[3] == 0xee, "");
    for (int idx = 4; idx < 63; ++idx) {
        assert(b4.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b4.m_arr[63] == 0xf9, "");
}

void testOperatorUnaryMinus() {
    uint512_t a1 = 250;
    a1.m_arr[3] = 17;

    uint512_t a2 = uint512_t::operator_unary_minus(a1);
    for (int idx = 0; idx < 3; ++idx) {
        assert(a2.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(a2.m_arr[3] == 0xee, "");
    for (int idx = 4; idx < 63; ++idx) {
        assert(a2.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(a2.m_arr[63] == 0x06, "");

    int512_t b1 = 250;
    b1.m_arr[3] = 17;

    int512_t b2 = int512_t::operator_unary_minus(b1);
    for (int idx = 0; idx < 3; ++idx) {
        assert(b2.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b2.m_arr[3] == 0xee, "");
    for (int idx = 4; idx < 63; ++idx) {
        assert(b2.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b2.m_arr[63] == 0x06, "");

    int512_t b3 = -250;
    b3.m_arr[3] = 17;

    int512_t b4 = int512_t::operator_unary_minus(b3);
    for (int idx = 0; idx < 3; ++idx) {
        assert(b4.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b4.m_arr[3] == 0xee, "");
    for (int idx = 4; idx < 63; ++idx) {
        assert(b4.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b4.m_arr[63] == 0xfa, "");
}

void testOperatorPlusWide() {
    uint512_t a1 = 3;
    a1.m_arr[55] = 1;

    uint512_t a11 = uint512_t::operator_plus(a1, a1);
    for (int idx = 0; idx < 55; ++idx) {
        assert(a11.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a11.m_arr[55] == 2, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(a11.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a11.m_arr[63] == 6, "");

    uint512_t a12 = uint512_t::operator_plus(a1, uint256_t(0));
    for (int idx = 0; idx < 55; ++idx) {
        assert(a12.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a12.m_arr[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(a12.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a12.m_arr[63] == 3, "");

    uint512_t a13 = uint512_t::operator_minus(a1, uint128_t(5));
    for (int idx = 0; idx < 56; ++idx) {
        assert(a13.m_arr[idx] == 0, std::to_string(idx));
    }
    for (int idx = 57; idx < 63; ++idx) {
        assert(a13.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(a13.m_arr[63] == 0xfe, "");

    uint512_t a14 = uint512_t::operator_plus(a1, uint512_t::operator_plus_T(a1, std::numeric_limits<uint64_t>::max()));
    for (int idx = 0; idx < 55; ++idx) {
        assert(a14.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a14.m_arr[55] == 3, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(a14.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a14.m_arr[63] == 5, "");

    uint512_t a15 = uint512_t::operator_minus(a1, uint256_t(0));
    for (int idx = 0; idx < 55; ++idx) {
        assert(a15.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a15.m_arr[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(a15.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a15.m_arr[63] == 3, "");

    int512_t b1 = 3;
    b1.m_arr[55] = 1;

    int512_t b11 = int512_t::operator_plus(b1, uint128_t(5));
    for (int idx = 0; idx < 55; ++idx) {
        assert(b11.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b11.m_arr[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(b11.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b11.m_arr[63] == 8, "");

    int512_t b12 = int512_t::operator_plus(b1, uint512_t(0));
    for (int idx = 0; idx < 55; ++idx) {
        assert(b12.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b12.m_arr[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(b12.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b12.m_arr[63] == 3, "");

    int512_t b13 = int512_t::operator_minus(b1, uint128_t(5));
    for (int idx = 0; idx < 56; ++idx) {
        assert(b13.m_arr[idx] == 0, std::to_string(idx));
    }
    for (int idx = 56; idx < 63; ++idx) {
        assert(b13.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b13.m_arr[63] == 0xfe, "");

    int512_t b14 = int512_t::operator_minus(b1, int128_t(0));
    for (int idx = 0; idx < 55; ++idx) {
        assert(b14.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b14.m_arr[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(b14.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b14.m_arr[63] == 3, "");

    int512_t b2 = -3;
    b2.m_arr[55] = -2;

    int512_t b21 = int512_t::operator_minus(b2, uint128_t(5));
    for (int idx = 0; idx < 55; ++idx) {
        assert(b21.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b21.m_arr[55] == 0xfe, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(b21.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(static_cast<int8_t>(b21.m_arr[63]) == -8, "");

    int512_t b22 = int512_t::operator_plus(b2, int256_t(0));
    for (int idx = 0; idx < 55; ++idx) {
        assert(b22.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b22.m_arr[55] == 0xfe, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(b22.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(static_cast<int8_t>(b22.m_arr[63]) == -3, "");

    int512_t b23 = int512_t::operator_plus(b2, uint128_t(5));
    for (int idx = 0; idx < 55; ++idx) {
        assert(b23.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b23.m_arr[55] == 0xff, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(b23.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b23.m_arr[63] == 2, "");

    int512_t b3 = -3;

    int512_t b31 = int512_t::operator_plus(b3, int128_t(5));
    for (int idx = 0; idx < 63; ++idx) {
        assert(b31.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(static_cast<int8_t>(b31.m_arr[63]) == 2, "");

    int512_t b4 = -1;
    b4.m_arr[55] = -2;
    b4.m_arr[63] = 3;

    int512_t b41 = int512_t::operator_plus(b4, uint256_t(5));
    for (int idx = 0; idx < 55; ++idx) {
        assert(b41.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(b41.m_arr[55] == 0xfe, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(b41.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(static_cast<int8_t>(b41.m_arr[63]) == 8, "");

    // test cast
    assert(typeid(uint512_t) == typeid(uint512_t::operator_plus(uint512_t(0), uint128_t(3))), "");
    assert(typeid(uint512_t) == typeid(uint512_t::operator_plus(uint512_t(0), uint256_t(3))), "");
    assert(typeid(uint512_t) == typeid(uint512_t::operator_plus(uint512_t(0), uint512_t(3))), "");
    assert(typeid(int512_t) == typeid(int512_t::operator_plus(int512_t(0), int128_t(3))), "");
    assert(typeid(int512_t) == typeid(int512_t::operator_plus(int512_t(0), int256_t(3))), "");
    assert(typeid(int512_t) == typeid(int512_t::operator_plus(int512_t(0), int512_t(3))), "");
    assert(typeid(uint512_t) == typeid(uint512_t::operator_plus(uint512_t(0), int128_t(3))), "");
    assert(typeid(uint512_t) == typeid(uint512_t::operator_plus(uint512_t(0), int256_t(3))), "");
    assert(typeid(uint512_t) == typeid(uint512_t::operator_plus(uint512_t(0), int512_t(3))), "");
    assert(typeid(int512_t) == typeid(int512_t::operator_plus(int512_t(0), uint128_t(3))), "");
    assert(typeid(int512_t) == typeid(int512_t::operator_plus(int512_t(0), uint256_t(3))), "");
    assert(typeid(int512_t) == typeid(int512_t::operator_plus(int512_t(0), uint512_t(3))), "");

    assert(typeid(uint512_t) == typeid(uint128_t::operator_plus(uint128_t(0), uint512_t(3))), "");
    assert(typeid(uint512_t) == typeid(uint256_t::operator_plus(uint256_t(0), uint512_t(3))), "");
    assert(typeid(int512_t) == typeid(int128_t::operator_plus(int128_t(0), int512_t(3))), "");
    assert(typeid(int512_t) == typeid(int256_t::operator_plus(int256_t(0), int512_t(3))), "");
    assert(typeid(uint512_t) == typeid(uint128_t::operator_plus(uint128_t(0), int512_t(3))), "");
    assert(typeid(uint512_t) == typeid(uint256_t::operator_plus(uint256_t(0), int512_t(3))), "");
    assert(typeid(int512_t) == typeid(int128_t::operator_plus(int128_t(0), uint512_t(3))), "");
    assert(typeid(int512_t) == typeid(int256_t::operator_plus(int256_t(0), uint512_t(3))), "");


    assert(typeid(uint512_t) == typeid(uint512_t::operator_minus(uint512_t(0), uint128_t(3))), "");
    assert(typeid(uint512_t) == typeid(uint512_t::operator_minus(uint512_t(0), uint256_t(3))), "");
    assert(typeid(uint512_t) == typeid(uint512_t::operator_minus(uint512_t(0), uint512_t(3))), "");
    assert(typeid(int512_t) == typeid(int512_t::operator_minus(int512_t(0), int128_t(3))), "");
    assert(typeid(int512_t) == typeid(int512_t::operator_minus(int512_t(0), int256_t(3))), "");
    assert(typeid(int512_t) == typeid(int512_t::operator_minus(int512_t(0), int512_t(3))), "");
    assert(typeid(uint512_t) == typeid(uint512_t::operator_minus(uint512_t(0), int128_t(3))), "");
    assert(typeid(uint512_t) == typeid(uint512_t::operator_minus(uint512_t(0), int256_t(3))), "");
    assert(typeid(uint512_t) == typeid(uint512_t::operator_minus(uint512_t(0), int512_t(3))), "");
    assert(typeid(int512_t) == typeid(int512_t::operator_minus(int512_t(0), uint128_t(3))), "");
    assert(typeid(int512_t) == typeid(int512_t::operator_minus(int512_t(0), uint256_t(3))), "");
    assert(typeid(int512_t) == typeid(int512_t::operator_minus(int512_t(0), uint512_t(3))), "");

    assert(typeid(uint512_t) == typeid(uint128_t::operator_minus(uint128_t(0), uint512_t(3))), "");
    assert(typeid(uint512_t) == typeid(uint256_t::operator_minus(uint256_t(0), uint512_t(3))), "");
    assert(typeid(int512_t) == typeid(int128_t::operator_minus(int128_t(0), int512_t(3))), "");
    assert(typeid(int512_t) == typeid(int256_t::operator_minus(int256_t(0), int512_t(3))), "");
    assert(typeid(uint512_t) == typeid(uint128_t::operator_minus(uint128_t(0), int512_t(3))), "");
    assert(typeid(uint512_t) == typeid(uint256_t::operator_minus(uint256_t(0), int512_t(3))), "");
    assert(typeid(int512_t) == typeid(int128_t::operator_minus(int128_t(0), uint512_t(3))), "");
    assert(typeid(int512_t) == typeid(int256_t::operator_minus(int256_t(0), uint512_t(3))), "");
}

void testOperatorStar() {
    uint512_t a1 = 256_uint512;

    uint512_t a11 = uint512_t::operator_star(a1, uint512_t(1));
    for (int idx = 0; idx < 62; ++idx) {
        assert(a11.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a11.m_arr[62] == 1, "");
    assert(a11.m_arr[63] == 0, "");

    uint512_t a12 = uint512_t::operator_star(a1, uint512_t(0));
    for (int idx = 0; idx < 63; ++idx) {
        assert(a12.m_arr[idx] == 0, std::to_string(idx));
    }

    uint512_t a13 = uint512_t::operator_star(a1, uint512_t(2));
    for (int idx = 0; idx < 62; ++idx) {
        assert(a13.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a13.m_arr[62] == 2, "");
    assert(a13.m_arr[63] == 0, "");

    uint512_t a2 = std::numeric_limits<uint64_t>::max();
    uint512_t a24 = uint512_t::operator_star(a2, uint512_t(2));
    for (int idx = 0; idx < 55; ++idx) {
        assert(a24.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a24.m_arr[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(a24.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(a24.m_arr[63] == 0xfe, "");

    uint512_t a3 = 256;

    uint512_t a31 = uint512_t::operator_star(a3, 1);
    for (int idx = 0; idx < 62; ++idx) {
        assert(a31.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a31.m_arr[62] == 1, "");
    assert(a31.m_arr[63] == 0, "");

    uint512_t a32 = uint512_t::operator_star(a3, 0);
    for (int idx = 0; idx < 63; ++idx) {
        assert(a32.m_arr[idx] == 0, std::to_string(idx));
    }

    uint512_t a33 = uint512_t::operator_star(a3, 2);
    for (int idx = 0; idx < 62; ++idx) {
        assert(a33.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a33.m_arr[62] == 2, "");
    assert(a33.m_arr[63] == 0, "");

    uint512_t a4 = std::numeric_limits<uint64_t>::max();
    uint512_t a44 = uint512_t::operator_star(a4, 2);
    for (int idx = 0; idx < 55; ++idx) {
        assert(a44.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a44.m_arr[55] == 1, "");
    for (int idx = 56; idx < 63; ++idx) {
        assert(a44.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(a44.m_arr[63] == 0xfe, "");
}

void testOperatorMore() {
    assert(int512_t::operator_more(15, -18), "");
    assert(!uint512_t::operator_more(15, 18U), "");
    assert(uint512_t::operator_more(18, 15U), "");
    assert(!uint512_t::operator_more(15, 18U), "");
    assert(int512_t::operator_more(-15, -18), "");
    assert(!int512_t::operator_more(-18, -15), "");
}

void testOperatorEq() {
    assert(uint512_t::operator_eq(uint512_t(12), int64_t(12)), "");
    assert(uint512_t::operator_eq(12, 12), "");
    assert(uint512_t::operator_eq(0, 0), "");
}

void testOperatorPipe() {
    uint512_t a1 = 0x0102030405060708;
    for (int i = 0; i < 7; ++i) {
        a1.m_arr[i] = 0x01;
    }
    uint512_t a2 = uint512_t::operator_pipe(a1, 0xf0f0f0f0f0f0f0f0);

    for (int i = 0; i < 7; ++i) {
        assert(a2.m_arr[i] == 0x01, "");
    }
    for (int idx = 8; idx < 56; ++idx) {
        assert(a2.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a2.m_arr[56] == 0xf1, "");
    assert(a2.m_arr[57] == 0xf2, "");
    assert(a2.m_arr[58] == 0xf3, "");
    assert(a2.m_arr[59] == 0xf4, "");
    assert(a2.m_arr[60] == 0xf5, "");
    assert(a2.m_arr[61] == 0xf6, "");
    assert(a2.m_arr[62] == 0xf7, "");
    assert(a2.m_arr[63] == 0xf8, "");

    uint512_t a3 = 0xf0f0f0f0f0f0f0f0;
    for (int i = 0; i < 7; ++i) {
        a3.m_arr[i] = 0xf0;
    }

    uint512_t a4 = uint512_t::operator_pipe(a1, a3);

    for (int i = 0; i < 7; ++i) {
        assert(a4.m_arr[i] == 0xf1, "");
    }
    for (int idx = 8; idx < 56; ++idx) {
        assert(a4.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a4.m_arr[56] == 0xf1, "");
    assert(a4.m_arr[57] == 0xf2, "");
    assert(a4.m_arr[58] == 0xf3, "");
    assert(a4.m_arr[59] == 0xf4, "");
    assert(a4.m_arr[60] == 0xf5, "");
    assert(a4.m_arr[61] == 0xf6, "");
    assert(a4.m_arr[62] == 0xf7, "");
    assert(a4.m_arr[63] == 0xf8, "");
}

void testOperatorAmp() {
    uint512_t a1 = 0x0102030405060708;
    for (int i = 0; i < 7; ++i) {
        a1.m_arr[i] = 0x01;
    }
    uint512_t a2 = uint512_t::operator_amp(a1, 0xf0f0f0f0f0f0f0f0);

    for (int idx = 0; idx < 63; ++idx) {
        assert(a2.m_arr[idx] == 0, std::to_string(idx));
    }

    uint512_t a3 = 0xffffffffffffffff;
    for (int i = 0; i < 7; ++i) {
        a2.m_arr[i] = 0xf0;
    }

    uint512_t a4 = uint512_t::operator_amp(a1, a3);

    for (int idx = 0; idx < 56; ++idx) {
        assert(a4.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a4.m_arr[56] == 0x01, "");
    assert(a4.m_arr[57] == 0x02, "");
    assert(a4.m_arr[58] == 0x03, "");
    assert(a4.m_arr[59] == 0x04, "");
    assert(a4.m_arr[60] == 0x05, "");
    assert(a4.m_arr[61] == 0x06, "");
    assert(a4.m_arr[62] == 0x07, "");
    assert(a4.m_arr[63] == 0x08, "");
}

void testOperatorSlash() {
    uint512_t a1 = uint512_t::operator_slash(15, 3);
    for (int idx = 0; idx < 63; ++idx) {
        assert(a1.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a1.m_arr[63] == 5, "");

    uint512_t a2 = uint512_t::operator_slash(17, 3);
    for (int idx = 0; idx < 63; ++idx) {
        assert(a2.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a2.m_arr[63] == 5, "");

    uint512_t a3 = uint512_t::operator_slash(0, 3);
    for (int idx = 0; idx < 64; ++idx) {
        assert(a3.m_arr[idx] == 0, std::to_string(idx));
    }

    uint512_t a4 = 2048;
    a4.m_arr[55] = 2;
    uint512_t a41 = uint512_t::operator_slash(a4, 2);
    for (int idx = 0; idx < 55; ++idx) {
        assert(a41.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a41.m_arr[55] == 1, "");
    for (int idx = 56; idx < 62; ++idx) {
        assert(a41.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a41.m_arr[62] == 4, "");
    assert(a41.m_arr[63] == 0, "");
}

void testOperatorPercent() {
    uint512_t a1 = uint512_t::operator_percent(15, 3);
    for (int idx = 0; idx < 64; ++idx) {
        assert(a1.m_arr[idx] == 0, std::to_string(idx));
    }

    uint512_t a2 = uint512_t::operator_percent(17, 3);
    for (int idx = 0; idx < 63; ++idx) {
        assert(a2.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a2.m_arr[63] == 2, "");

    uint512_t a3 = uint512_t::operator_percent(0, 3);
    for (int idx = 0; idx < 64; ++idx) {
        assert(a3.m_arr[idx] == 0, std::to_string(idx));
    }

    uint512_t a4 = 2049;
    a4.m_arr[55] = 2;
    uint512_t a41 = uint512_t::operator_percent(a4, 2);
    for (int idx = 0; idx < 63; ++idx) {
        assert(a41.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a41.m_arr[63] == 1, "");
}

void testCircumflex() {
    uint512_t a1 = 0xff;

    uint512_t a2 = uint512_t::operator_circumflex(a1, 0xf);
    for (int idx = 0; idx < 63; ++idx) {
        assert(a2.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a2.m_arr[63] == 0xf0, "");
}

void testToString() {
    assert(to_string(uint512_t(123455)) == std::string("123455"), "");
    assert(to_string(int512_t(-123455)) == std::string("-123455"), "");
    std::cout << int512_t(-123455) << std::endl;
}

void testCast() {
    assert(int(uint512_t(18)) == 18, "");

    uint128_t a1 = uint512_t(18);
    for (int idx = 0; idx < 15; ++idx) {
        assert(a1.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a1.m_arr[15] == 18, "");
}

void testFromString() {
    auto a1 = uint512_t::from_str("1234567");
    assert(typeid(uint512_t) == typeid(a1), "");
    assert(std::to_string(a1) == "1234567", "");
    for (int idx = 0; idx < 61; ++idx) {
        assert(a1.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a1.m_arr[61] == 0x12, "");
    assert(a1.m_arr[62] == 0xd6, "");
    assert(a1.m_arr[63] == 0x87, "");
}

void testNativeOperators() {
    auto a1 = ~uint512_t(0xff);
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a1.m_arr[idx] == 0xff, std::to_string(idx));
    }
    assert(a1.m_arr[63] == 0x00, "");

    auto a2 = -uint512_t(0x1);
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a2.m_arr[idx] == 0xff, std::to_string(idx));
    }

    auto a3 = 2*uint512_t(0x1);
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a3.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a3.m_arr[63] == 2, "");

    auto a4 = uint512_t(0x1)*2;
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a4.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a4.m_arr[63] == 2, "");

    auto a5 = 4/uint512_t(0x2);
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a5.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a5.m_arr[63] == 2, "");

    auto a6 = uint512_t(0x4)/2;
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a6.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a6.m_arr[63] == 2, "");

    auto a7 = 8%uint512_t(0x5);
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a7.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a7.m_arr[63] == 3, "");

    auto a8 = uint512_t(0x8)%5;
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a8.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a8.m_arr[63] == 3, "");

    auto a9 = 8+uint512_t(0x5);
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a9.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a9.m_arr[63] == 13, "");

    auto a10 = uint512_t(0x8)+5;
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a10.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a10.m_arr[63] == 13, "");

    auto a11 = 8-uint512_t(0x5);
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a11.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a11.m_arr[63] == 3, "");

    auto a12 = uint512_t(0x8)-5;
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a12.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a12.m_arr[63] == 3, "");

    assert(uint512_t(15) < 20U, "");
    assert(uint512_t(20) > 15U, "");
    assert(uint512_t(15) <= 20U, "");
    assert(uint512_t(20) >= 15U, "");
    assert(uint512_t(20) <= 20U, "");
    assert(uint512_t(20) >= 20U, "");
    assert(uint512_t(20) == 20U, "");

    assert(15U <  uint512_t(20), "");
    assert(20U >  uint512_t(15), "");
    assert(15U <= uint512_t(20), "");
    assert(20U >= uint512_t(15), "");
    assert(20U <= uint512_t(20), "");
    assert(20U >= uint512_t(20), "");
    assert(20U == uint512_t(20), "");

    assert(uint512_t(15) <  uint128_t(20), "");
    assert(uint512_t(20) >  uint128_t(15), "");
    assert(uint512_t(15) <= uint128_t(20), "");
    assert(uint512_t(20) >= uint128_t(15), "");
    assert(uint512_t(20) <= uint128_t(20), "");
    assert(uint512_t(20) >= uint128_t(20), "");
    assert(uint512_t(20) == uint128_t(20), "");

    assert(uint128_t(15) <  uint512_t(20), "");
    assert(uint128_t(20) >  uint512_t(15), "");
    assert(uint128_t(15) <= uint512_t(20), "");
    assert(uint128_t(20) >= uint512_t(15), "");
    assert(uint128_t(20) <= uint512_t(20), "");
    assert(uint128_t(20) >= uint512_t(20), "");
    assert(uint128_t(20) == uint512_t(20), "");

    auto a13 = 9&uint512_t(5);
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a13.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a13.m_arr[63] == 1, "");

    auto a14 = uint512_t(9)&5;
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a14.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a14.m_arr[63] == 1, "");

    auto a15 = 9|uint512_t(5);
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a15.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a15.m_arr[63] == 13, "");

    auto a16 = uint512_t(9)|5;
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a16.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a16.m_arr[63] == 13, "");

    auto a17 = 4^uint512_t(5);
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a17.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a17.m_arr[63] == 1, "");

    auto a18 = uint512_t(4)^5;
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a18.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a18.m_arr[63] == 1, "");

    auto a19 = uint512_t(8) << 1;
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a19.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a19.m_arr[63] == 16, "");

    auto a20 = uint512_t(8) >> 1;
    assert(typeid(uint512_t) == typeid(a1), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a20.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a20.m_arr[63] == 4, "");

    assert(!uint256_t(0), "");
    assert(uint256_t(1), "");

    auto a21 = +uint512_t(0x1);
    assert(typeid(uint512_t) == typeid(a21), "");
    for (int idx = 0; idx < 63; ++idx) {
        assert(a21.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a21.m_arr[63] == 1, "");
}

void testNativeOperatorsAssign() {
    uint512_t a1 = 18;

    a1 *= 2U;
    assert(a1 == 36U, "");

    a1 /= 2U;
    assert(a1 == 18U, "");

    a1 %= 5U;
    assert(a1 == 3U, "");

    a1 += 2U;
    assert(a1 == 5U, "");

    a1 -= 1U;
    assert(a1 == 4U, "");

    a1 &= 3U;
    assert(a1 == 0U, "");

    a1 |= 255U;
    assert(a1 == 255U, "");

    a1 ^= 0xffffffffffffffff;
    assert(a1 == 0xffffffffffffff00, "");

    a1 <<= 4;
    assert(a1 == 0xffffffffffffff000_uint128, "");

    a1 >>= 8;
    assert(a1 == 0x0ffffffffffffff0, "");
}

void testConstexpr() {
    constexpr uint256_t a0 {};
    static_assert(a0 == 0, "");
    static_assert(a0 >= 0U, "");
    static_assert(a0 <  1U, "");
    static_assert(a0 <= 1U, "");

    constexpr uint512_t a1 = 7;
    static_assert(a1 >  0U, "");
    static_assert(a1 == 7, "");
    static_assert(a1 + 12 == 19, "");
    static_assert(12 + a1 == 19, "");

    constexpr int128_t a2 = a1;
    static_assert(a2 == 7, "");
    static_assert(a2 - 3 == 4, "");
    static_assert(11 - a2 == 4, "");

    static_assert(a1/2 == 3, "");
    static_assert(a2*2 == 14, "");
    static_assert(a1%2 == 1, "");

    static_assert(~a2 == -8, "");
    static_assert(-a2 == -7, "");

    static_assert(a1 | 0xff == 7, "");
    static_assert(a1 & 0xff == 0xff, "");
    static_assert((a2 ^ 0xff) == 0xf8, "");

    static_assert(0xff_int128 == 255, "");
    static_assert(255_int128 == 255, "");

    static_assert(a1 >> 1 == 3, "");
    static_assert(a1 << 1 == 14, "");

    constexpr uint256_t b = std::numeric_limits<uint256_t>::min();
    static_assert(b == 0, "");

    constexpr int128_t c = std::numeric_limits<int128_t>::min();
    static_assert(c == 0x80000000000000000000000000000000_uint128, "");

    static_assert(0x80000000000000000000000000000000_uint128, "");
}

void tests() {
    static_assert(std::is_pod<int512_t>::value, "");
    static_assert(std::is_pod<uint512_t>::value, "");
    static_assert(std::is_standard_layout<int512_t>::value, "");
    static_assert(std::is_standard_layout<uint512_t>::value, "");

    testCtors();
    testShifts();
    testAssign();
    testOperatorPlus();
    testOperatorTilda();
    testOperatorUnaryMinus();
    testOperatorPlusWide();
    testOperatorStar();
    testOperatorMore();
    testOperatorEq();
    testOperatorPipe();
    testOperatorAmp();
    testOperatorSlash();
    testOperatorPercent();
    testCircumflex();
    testToString();
    testCast();
    testFromString();
    testNativeOperators();
    testNativeOperatorsAssign();
    testConstexpr();
    std::cout << wide_int<80, false>(18) << std::endl;

    std::cout << "OK" << std::endl;
}

int main() {
    tests();

    return 0;
}
