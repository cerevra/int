
#include "int.h"

#include <iomanip>
#include <iostream>
#include <typeinfo>
#include <unordered_map>

using namespace std;

#define assert(a, info)                                                                           \
    {                                                                                             \
        if (!(a))                                                                                 \
            throw std::runtime_error(#a ", line:" + std::to_string(__LINE__) + ", info:" + info); \
    }

struct _test {
    template <size_t Bits, typename Sgn>
    static void printNum(const wide_integer<Bits, Sgn>& n) {
        for (auto c : n.m_arr) {
            std::cerr << std::hex << std::setw(2) << std::setfill('0') << uint64_t(c); // << std::endl;
        }
        std::cerr << std::endl;
    }

    static void printNum(uint64_t n) {
        std::cerr << std::hex << std::setw(16) << std::setfill('0') << n << std::endl;
    }

    static void testCtors() {
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

        int512_t a8 = -13.0;
        for (int idx = 0; idx < 63; ++idx) {
            assert(a8.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(a8.m_arr[63] == 0xf3, "");
        uint512_t b8 = -13.0;
        for (int idx = 0; idx < 63; ++idx) {
            assert(b8.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(b8.m_arr[63] == 0xf3, "");

        int512_t a9 = 13.0;
        for (int idx = 0; idx < 63; ++idx) {
            assert(a9.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a9.m_arr[63] == 13, "");
        uint512_t b9 = 13.0;
        for (int idx = 0; idx < 63; ++idx) {
            assert(b9.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(b9.m_arr[63] == 13, "");
    }

    static void testShifts() {
        //left
        uint512_t a1 = 0;
        for (auto c : uint512_t::_impl::shift_left(a1, 0).m_arr) {
            assert(c == 0, "");
        }
        uint512_t a2 = 0;
        for (auto c : uint512_t::_impl::shift_left(a2, 15).m_arr) {
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
        uint512_t a31 = uint512_t::_impl::shift_left(a3, 8);
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

        uint512_t a32 = uint512_t::_impl::shift_left(a3, 16);
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

        uint512_t a33 = uint512_t::_impl::shift_left(a3, 4);
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

        uint512_t a34 = uint512_t::_impl::shift_left(a3, 64);
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

        uint512_t a35 = uint512_t::_impl::shift_left(a3, 256);
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

        uint512_t a36 = uint512_t::_impl::shift_left(a3, 260);
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
        for (auto c : uint512_t::_impl::shift_right(a4, 0).m_arr) {
            assert(c == 0, "");
        }
        uint512_t a5 = 0;
        for (auto c : uint512_t::_impl::shift_right(a5, 15).m_arr) {
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
        uint512_t a61 = uint512_t::_impl::shift_right(a6, 8);
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

        uint512_t a62 = uint512_t::_impl::shift_right(a6, 16);
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

        uint512_t a63 = uint512_t::_impl::shift_right(a6, 4);
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

        uint512_t a64 = uint512_t::_impl::shift_right(a6, 64);
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

        uint512_t a65 = uint512_t::_impl::shift_right(a6, 256);
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

        uint512_t a66 = uint512_t::_impl::shift_right(a6, 260);
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
        for (auto c : int512_t::_impl::shift_right(b4, 0).m_arr) {
            assert(c == 0, "");
        }
        int512_t b5 = 0;
        for (auto c : int512_t::_impl::shift_right(b5, 15).m_arr) {
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

        int512_t b61 = int512_t::_impl::shift_right(b6, 8);
        assert(b61.m_arr[0] == 0, "");
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

        int512_t b62 = int512_t::_impl::shift_right(b6, 16);
        assert(b62.m_arr[0] == 0, "");
        assert(b62.m_arr[1] == 0, "");
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

        int512_t b63 = int512_t::_impl::shift_right(b6, 4);
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

        int512_t b64 = int512_t::_impl::shift_right(b6, 64);
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

        int512_t b65 = int512_t::_impl::shift_right(b6, 256);
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

        int512_t b66 = int512_t::_impl::shift_right(b6, 260);
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
        int512_t b71 = int512_t::_impl::shift_right(b7, 8);
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

        int512_t b72 = int512_t::_impl::shift_right(b7, 16);
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

        int512_t b73 = int512_t::_impl::shift_right(b7, 4);
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

        int512_t b74 = int512_t::_impl::shift_right(b7, 64);
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

        int512_t b75 = int512_t::_impl::shift_right(b7, 256);
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

        int512_t b76 = int512_t::_impl::shift_right(b7, 260);
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
        int512_t b81 = int512_t::_impl::shift_right(b8, 1);
        for (int idx = 0; idx < 63; ++idx) {
            assert(b81.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(b81.m_arr[63] == 0xf9, "");
    }

    static void testAssign() {
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

    static void testOperatorPlus() {
        uint512_t a1 = 3;
        a1.m_arr[55] = 1;

        uint512_t a11 = uint512_t::_impl::operator_plus_T(a1, 5);
        for (int idx = 0; idx < 55; ++idx) {
            assert(a11.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a11.m_arr[55] == 1, "");
        for (int idx = 56; idx < 63; ++idx) {
            assert(a11.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a11.m_arr[63] == 8, "");

        uint512_t a12 = uint512_t::_impl::operator_plus_T(a1, 0);
        for (int idx = 0; idx < 55; ++idx) {
            assert(a12.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a12.m_arr[55] == 1, "");
        for (int idx = 56; idx < 63; ++idx) {
            assert(a12.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a12.m_arr[63] == 3, "");

        uint512_t a14 = uint512_t::_impl::operator_plus_T(a1, std::numeric_limits<uint64_t>::max());
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

        int512_t b11 = int512_t::_impl::operator_plus_T(b1, 5);
        for (int idx = 0; idx < 55; ++idx) {
            assert(b11.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(b11.m_arr[55] == 1, "");
        for (int idx = 56; idx < 63; ++idx) {
            assert(b11.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(b11.m_arr[63] == 8, "");

        int512_t b12 = int512_t::_impl::operator_plus_T(b1, 0);
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

        int512_t b22 = int512_t::_impl::operator_plus_T(b2, 0);
        for (int idx = 0; idx < 55; ++idx) {
            assert(b22.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(b22.m_arr[55] == 0xfe, "");
        for (int idx = 56; idx < 63; ++idx) {
            assert(b22.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(b22.m_arr[63] == 0xfd, "");

        int512_t b23 = int512_t::_impl::operator_plus_T(b2, uint16_t(5));
        for (int idx = 0; idx < 56; ++idx) {
            assert(b23.m_arr[idx] == 0xff, std::to_string(idx));
        }
        for (int idx = 56; idx < 63; ++idx) {
            assert(b23.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(b23.m_arr[63] == 2, "");

        int512_t b3 = -3;

        int512_t b31 = int512_t::_impl::operator_plus_T(b3, 5);
        for (int idx = 0; idx < 63; ++idx) {
            assert(b31.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(static_cast<int64_t>(b31.m_arr[63]) == 2, "");

        int512_t b4 = -1;
        b4.m_arr[55] = -2;
        b4.m_arr[63] = 3;

        int512_t b41 = int512_t::_impl::operator_plus_T(b4, 5);
        for (int idx = 0; idx < 55; ++idx) {
            assert(b41.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(b41.m_arr[55] == 0xfe, "");
        for (int idx = 56; idx < 63; ++idx) {
            assert(b41.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(b41.m_arr[63] == 8, "");
    }

    static void testOperatorTilda() {
        uint512_t a1 = 250;
        a1.m_arr[3] = 17;

        uint512_t a2 = uint512_t::_impl::operator_unary_tilda(a1);
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

        int512_t b2 = int512_t::_impl::operator_unary_tilda(b1);
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

        int512_t b4 = int512_t::_impl::operator_unary_tilda(b3);
        for (int idx = 0; idx < 3; ++idx) {
            assert(b4.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(b4.m_arr[3] == 0xee, "");
        for (int idx = 4; idx < 63; ++idx) {
            assert(b4.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(b4.m_arr[63] == 0xf9, "");
    }

    static void testOperatorUnaryMinus() {
        uint512_t a1 = 250;
        a1.m_arr[3] = 17;

        uint512_t a2 = uint512_t::_impl::operator_unary_minus(a1);
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

        int512_t b2 = int512_t::_impl::operator_unary_minus(b1);
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

        int512_t b4 = int512_t::_impl::operator_unary_minus(b3);
        for (int idx = 0; idx < 3; ++idx) {
            assert(b4.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(b4.m_arr[3] == 0xee, "");
        for (int idx = 4; idx < 63; ++idx) {
            assert(b4.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(b4.m_arr[63] == 0xfa, "");
    }

    static void testOperatorPlusWide() {
        uint512_t a1 = 3;
        a1.m_arr[55] = 1;

        uint512_t a11 = uint512_t::_impl::operator_plus(a1, a1);
        for (int idx = 0; idx < 55; ++idx) {
            assert(a11.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a11.m_arr[55] == 2, "");
        for (int idx = 56; idx < 63; ++idx) {
            assert(a11.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a11.m_arr[63] == 6, "");

        uint512_t a12 = uint512_t::_impl::operator_plus(a1, uint256_t(0));
        for (int idx = 0; idx < 55; ++idx) {
            assert(a12.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a12.m_arr[55] == 1, "");
        for (int idx = 56; idx < 63; ++idx) {
            assert(a12.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a12.m_arr[63] == 3, "");

        uint512_t a13 = uint512_t::_impl::operator_minus(a1, uint128_t(5));
        for (int idx = 0; idx < 56; ++idx) {
            assert(a13.m_arr[idx] == 0, std::to_string(idx));
        }
        for (int idx = 57; idx < 63; ++idx) {
            assert(a13.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(a13.m_arr[63] == 0xfe, "");

        uint512_t a14 = uint512_t::_impl::operator_plus(a1, uint512_t::_impl::operator_plus_T(a1, std::numeric_limits<uint64_t>::max()));
        for (int idx = 0; idx < 55; ++idx) {
            assert(a14.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a14.m_arr[55] == 3, "");
        for (int idx = 56; idx < 63; ++idx) {
            assert(a14.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a14.m_arr[63] == 5, "");

        uint512_t a15 = uint512_t::_impl::operator_minus(a1, uint256_t(0));
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

        int512_t b11 = int512_t::_impl::operator_plus(b1, uint128_t(5));
        for (int idx = 0; idx < 55; ++idx) {
            assert(b11.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(b11.m_arr[55] == 1, "");
        for (int idx = 56; idx < 63; ++idx) {
            assert(b11.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(b11.m_arr[63] == 8, "");

        int512_t b12 = int512_t::_impl::operator_plus(b1, uint512_t(0));
        for (int idx = 0; idx < 55; ++idx) {
            assert(b12.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(b12.m_arr[55] == 1, "");
        for (int idx = 56; idx < 63; ++idx) {
            assert(b12.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(b12.m_arr[63] == 3, "");

        int512_t b13 = int512_t::_impl::operator_minus(b1, uint128_t(5));
        for (int idx = 0; idx < 56; ++idx) {
            assert(b13.m_arr[idx] == 0, std::to_string(idx));
        }
        for (int idx = 56; idx < 63; ++idx) {
            assert(b13.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(b13.m_arr[63] == 0xfe, "");

        int512_t b14 = int512_t::_impl::operator_minus(b1, int128_t(0));
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

        int512_t b21 = int512_t::_impl::operator_minus(b2, uint128_t(5));
        for (int idx = 0; idx < 55; ++idx) {
            assert(b21.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(b21.m_arr[55] == 0xfe, "");
        for (int idx = 56; idx < 63; ++idx) {
            assert(b21.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(static_cast<int8_t>(b21.m_arr[63]) == -8, "");

        int512_t b22 = int512_t::_impl::operator_plus(b2, int256_t(0));
        for (int idx = 0; idx < 55; ++idx) {
            assert(b22.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(b22.m_arr[55] == 0xfe, "");
        for (int idx = 56; idx < 63; ++idx) {
            assert(b22.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(static_cast<int8_t>(b22.m_arr[63]) == -3, "");

        int512_t b23 = int512_t::_impl::operator_plus(b2, uint128_t(5));
        for (int idx = 0; idx < 55; ++idx) {
            assert(b23.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(b23.m_arr[55] == 0xff, "");
        for (int idx = 56; idx < 63; ++idx) {
            assert(b23.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(b23.m_arr[63] == 2, "");

        int512_t b3 = -3;

        int512_t b31 = int512_t::_impl::operator_plus(b3, int128_t(5));
        for (int idx = 0; idx < 63; ++idx) {
            assert(b31.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(static_cast<int8_t>(b31.m_arr[63]) == 2, "");

        int512_t b4 = -1;
        b4.m_arr[55] = -2;
        b4.m_arr[63] = 3;

        int512_t b41 = int512_t::_impl::operator_plus(b4, uint256_t(5));
        for (int idx = 0; idx < 55; ++idx) {
            assert(b41.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(b41.m_arr[55] == 0xfe, "");
        for (int idx = 56; idx < 63; ++idx) {
            assert(b41.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(static_cast<int8_t>(b41.m_arr[63]) == 8, "");

        // test cast
        assert(typeid(uint512_t) == typeid(uint512_t::_impl::operator_plus(uint512_t(0), uint128_t(3))), "");
        assert(typeid(uint512_t) == typeid(uint512_t::_impl::operator_plus(uint512_t(0), uint256_t(3))), "");
        assert(typeid(uint512_t) == typeid(uint512_t::_impl::operator_plus(uint512_t(0), uint512_t(3))), "");
        assert(typeid(int512_t) == typeid(int512_t::_impl::operator_plus(int512_t(0), int128_t(3))), "");
        assert(typeid(int512_t) == typeid(int512_t::_impl::operator_plus(int512_t(0), int256_t(3))), "");
        assert(typeid(int512_t) == typeid(int512_t::_impl::operator_plus(int512_t(0), int512_t(3))), "");
        assert(typeid(uint512_t) == typeid(uint512_t::_impl::operator_plus(uint512_t(0), int128_t(3))), "");
        assert(typeid(uint512_t) == typeid(uint512_t::_impl::operator_plus(uint512_t(0), int256_t(3))), "");
        assert(typeid(uint512_t) == typeid(uint512_t::_impl::operator_plus(uint512_t(0), int512_t(3))), "");
        assert(typeid(int512_t) == typeid(int512_t::_impl::operator_plus(int512_t(0), uint128_t(3))), "");
        assert(typeid(int512_t) == typeid(int512_t::_impl::operator_plus(int512_t(0), uint256_t(3))), "");
        assert(typeid(int512_t) == typeid(int512_t::_impl::operator_plus(int512_t(0), uint512_t(3))), "");

        assert(typeid(uint512_t) == typeid(uint128_t::_impl::operator_plus(uint128_t(0), uint512_t(3))), "");
        assert(typeid(uint512_t) == typeid(uint256_t::_impl::operator_plus(uint256_t(0), uint512_t(3))), "");
        assert(typeid(int512_t) == typeid(int128_t::_impl::operator_plus(int128_t(0), int512_t(3))), "");
        assert(typeid(int512_t) == typeid(int256_t::_impl::operator_plus(int256_t(0), int512_t(3))), "");
        assert(typeid(uint512_t) == typeid(uint128_t::_impl::operator_plus(uint128_t(0), int512_t(3))), "");
        assert(typeid(uint512_t) == typeid(uint256_t::_impl::operator_plus(uint256_t(0), int512_t(3))), "");
        assert(typeid(int512_t) == typeid(int128_t::_impl::operator_plus(int128_t(0), uint512_t(3))), "");
        assert(typeid(int512_t) == typeid(int256_t::_impl::operator_plus(int256_t(0), uint512_t(3))), "");

        assert(typeid(uint512_t) == typeid(uint512_t::_impl::operator_minus(uint512_t(0), uint128_t(3))), "");
        assert(typeid(uint512_t) == typeid(uint512_t::_impl::operator_minus(uint512_t(0), uint256_t(3))), "");
        assert(typeid(uint512_t) == typeid(uint512_t::_impl::operator_minus(uint512_t(0), uint512_t(3))), "");
        assert(typeid(int512_t) == typeid(int512_t::_impl::operator_minus(int512_t(0), int128_t(3))), "");
        assert(typeid(int512_t) == typeid(int512_t::_impl::operator_minus(int512_t(0), int256_t(3))), "");
        assert(typeid(int512_t) == typeid(int512_t::_impl::operator_minus(int512_t(0), int512_t(3))), "");
        assert(typeid(uint512_t) == typeid(uint512_t::_impl::operator_minus(uint512_t(0), int128_t(3))), "");
        assert(typeid(uint512_t) == typeid(uint512_t::_impl::operator_minus(uint512_t(0), int256_t(3))), "");
        assert(typeid(uint512_t) == typeid(uint512_t::_impl::operator_minus(uint512_t(0), int512_t(3))), "");
        assert(typeid(int512_t) == typeid(int512_t::_impl::operator_minus(int512_t(0), uint128_t(3))), "");
        assert(typeid(int512_t) == typeid(int512_t::_impl::operator_minus(int512_t(0), uint256_t(3))), "");
        assert(typeid(int512_t) == typeid(int512_t::_impl::operator_minus(int512_t(0), uint512_t(3))), "");

        assert(typeid(uint512_t) == typeid(uint128_t::_impl::operator_minus(uint128_t(0), uint512_t(3))), "");
        assert(typeid(uint512_t) == typeid(uint256_t::_impl::operator_minus(uint256_t(0), uint512_t(3))), "");
        assert(typeid(int512_t) == typeid(int128_t::_impl::operator_minus(int128_t(0), int512_t(3))), "");
        assert(typeid(int512_t) == typeid(int256_t::_impl::operator_minus(int256_t(0), int512_t(3))), "");
        assert(typeid(uint512_t) == typeid(uint128_t::_impl::operator_minus(uint128_t(0), int512_t(3))), "");
        assert(typeid(uint512_t) == typeid(uint256_t::_impl::operator_minus(uint256_t(0), int512_t(3))), "");
        assert(typeid(int512_t) == typeid(int128_t::_impl::operator_minus(int128_t(0), uint512_t(3))), "");
        assert(typeid(int512_t) == typeid(int256_t::_impl::operator_minus(int256_t(0), uint512_t(3))), "");
    }

    static void testOperatorStar() {
        uint512_t a1 = 256_uint512;

        uint512_t a11 = uint512_t::_impl::operator_star(a1, uint512_t(1));
        for (int idx = 0; idx < 62; ++idx) {
            assert(a11.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a11.m_arr[62] == 1, "");
        assert(a11.m_arr[63] == 0, "");

        uint512_t a12 = uint512_t::_impl::operator_star(a1, uint512_t(0));
        for (int idx = 0; idx < 63; ++idx) {
            assert(a12.m_arr[idx] == 0, std::to_string(idx));
        }

        uint512_t a13 = uint512_t::_impl::operator_star(a1, uint512_t(2));
        for (int idx = 0; idx < 62; ++idx) {
            assert(a13.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a13.m_arr[62] == 2, "");
        assert(a13.m_arr[63] == 0, "");

        uint512_t a2 = std::numeric_limits<uint64_t>::max();
        uint512_t a24 = uint512_t::_impl::operator_star(a2, uint512_t(2));
        for (int idx = 0; idx < 55; ++idx) {
            assert(a24.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a24.m_arr[55] == 1, "");
        for (int idx = 56; idx < 63; ++idx) {
            assert(a24.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(a24.m_arr[63] == 0xfe, "");

        uint512_t a3 = 256;

        uint512_t a31 = uint512_t::_impl::operator_star(a3, 1);
        for (int idx = 0; idx < 62; ++idx) {
            assert(a31.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a31.m_arr[62] == 1, "");
        assert(a31.m_arr[63] == 0, "");

        uint512_t a32 = uint512_t::_impl::operator_star(a3, 0);
        for (int idx = 0; idx < 63; ++idx) {
            assert(a32.m_arr[idx] == 0, std::to_string(idx));
        }

        uint512_t a33 = uint512_t::_impl::operator_star(a3, 2);
        for (int idx = 0; idx < 62; ++idx) {
            assert(a33.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a33.m_arr[62] == 2, "");
        assert(a33.m_arr[63] == 0, "");

        uint512_t a4 = std::numeric_limits<uint64_t>::max();
        uint512_t a44 = uint512_t::_impl::operator_star(a4, 2);
        for (int idx = 0; idx < 55; ++idx) {
            assert(a44.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a44.m_arr[55] == 1, "");
        for (int idx = 56; idx < 63; ++idx) {
            assert(a44.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(a44.m_arr[63] == 0xfe, "");
    }

    static void testOperatorMore() {
        assert(int512_t::_impl::operator_more(15, -18), "");
        assert(!uint512_t::_impl::operator_more(15, 18U), "");
        assert(uint512_t::_impl::operator_more(18, 15U), "");
        assert(!uint512_t::_impl::operator_more(15, 18U), "");
        assert(int512_t::_impl::operator_more(-15, -18), "");
        assert(!int512_t::_impl::operator_more(-18, -15), "");
    }

    static void testOperatorEq() {
        assert(uint512_t::_impl::operator_eq(uint512_t(12), int64_t(12)), "");
        assert(uint512_t::_impl::operator_eq(12, 12), "");
        assert(uint512_t::_impl::operator_eq(0, 0), "");
    }

    static void testOperatorPipe() {
        uint512_t a1 = 0x0102030405060708;
        for (int i = 0; i < 7; ++i) {
            a1.m_arr[i] = 0x01;
        }
        uint512_t a2 = uint512_t::_impl::operator_pipe(a1, 0xf0f0f0f0f0f0f0f0);

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

        uint512_t a4 = uint512_t::_impl::operator_pipe(a1, a3);

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

    static void testOperatorAmp() {
        uint512_t a1 = 0x0102030405060708;
        for (int i = 0; i < 7; ++i) {
            a1.m_arr[i] = 0x01;
        }
        uint512_t a2 = uint512_t::_impl::operator_amp(a1, 0xf0f0f0f0f0f0f0f0);

        for (int idx = 0; idx < 63; ++idx) {
            assert(a2.m_arr[idx] == 0, std::to_string(idx));
        }

        uint512_t a3 = 0xffffffffffffffff;
        for (int i = 0; i < 7; ++i) {
            a2.m_arr[i] = 0xf0;
        }

        uint512_t a4 = uint512_t::_impl::operator_amp(a1, a3);

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

    static void testOperatorSlash() {
        uint512_t a1 = uint512_t::_impl::operator_slash(15, 3);
        for (int idx = 0; idx < 63; ++idx) {
            assert(a1.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a1.m_arr[63] == 5, "");

        uint512_t a2 = uint512_t::_impl::operator_slash(17, 3);
        for (int idx = 0; idx < 63; ++idx) {
            assert(a2.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a2.m_arr[63] == 5, "");

        uint512_t a3 = uint512_t::_impl::operator_slash(0, 3);
        for (int idx = 0; idx < 64; ++idx) {
            assert(a3.m_arr[idx] == 0, std::to_string(idx));
        }

        uint512_t a4 = 2048;
        a4.m_arr[55] = 2;
        uint512_t a41 = uint512_t::_impl::operator_slash(a4, 2);
        for (int idx = 0; idx < 55; ++idx) {
            assert(a41.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a41.m_arr[55] == 1, "");
        for (int idx = 56; idx < 62; ++idx) {
            assert(a41.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a41.m_arr[62] == 4, "");
        assert(a41.m_arr[63] == 0, "");

        assert((int256_t(-6) / int256_t(-2)) == int256_t(3), "");
    }

    static void testOperatorPercent() {
        uint512_t a1 = uint512_t::_impl::operator_percent(15, 3);
        for (int idx = 0; idx < 64; ++idx) {
            assert(a1.m_arr[idx] == 0, std::to_string(idx));
        }

        uint512_t a2 = uint512_t::_impl::operator_percent(17, 3);
        for (int idx = 0; idx < 63; ++idx) {
            assert(a2.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a2.m_arr[63] == 2, "");

        uint512_t a3 = uint512_t::_impl::operator_percent(0, 3);
        for (int idx = 0; idx < 64; ++idx) {
            assert(a3.m_arr[idx] == 0, std::to_string(idx));
        }

        uint512_t a4 = 2049;
        a4.m_arr[55] = 2;
        uint512_t a41 = uint512_t::_impl::operator_percent(a4, 2);
        for (int idx = 0; idx < 63; ++idx) {
            assert(a41.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a41.m_arr[63] == 1, "");

        assert(int256_t(-14) % int256_t(10) == int256_t(-4), "");
    }

    static void testCircumflex() {
        uint512_t a1 = 0xff;

        uint512_t a2 = uint512_t::_impl::operator_circumflex(a1, 0xf);
        for (int idx = 0; idx < 63; ++idx) {
            assert(a2.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a2.m_arr[63] == 0xf0, "");
    }

    static void testToString() {
        assert(to_string(uint512_t(123455)) == std::string("123455"), "");
        assert(to_string(int512_t(-123455)) == std::string("-123455"), "");
        std::cout << int512_t(-123455) << std::endl;

        assert(to_wstring(uint512_t(123455)) == std::wstring(L"123455"), "");
        assert(to_wstring(int512_t(-123455)) == std::wstring(L"-123455"), "");
        std::wcout << 123000999_int128 << std::endl;
    }

    static void testCast() {
        assert(int(uint512_t(18)) == 18, "");
        assert(double(uint512_t(1024)) == 1024, "");

        uint128_t a1 = uint512_t(18);
        for (int idx = 0; idx < 15; ++idx) {
            assert(a1.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a1.m_arr[15] == 18, "");

        uint512_t a2 = 1024;
        assert(a2 == 1024, "");
        uint32_t a3 = a2;
        assert(a3 == 1024, "");

        assert(int(int512_t(-1024)) == -1024, "");
        assert(double(int512_t(-1024)) == -1024, "");
        assert(double(-int512_t(1024)) == -1024, "");
        assert(-double(int512_t(1024)) == -1024, "");
    }

    static void testFromString() {
        auto a1 = uint512_t::_impl::from_str("1234567");
        assert(typeid(uint512_t) == typeid(a1), "");
        assert(std::to_string(a1) == "1234567", "");
        for (int idx = 0; idx < 61; ++idx) {
            assert(a1.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a1.m_arr[61] == 0x12, "");
        assert(a1.m_arr[62] == 0xd6, "");
        assert(a1.m_arr[63] == 0x87, "");

        std::string s = "1000000000000000000";
        std::istringstream in(s);
        in >> a1;
        assert(a1 == 1000000000000000000_int256, "");

        std::wstring ws = L"7000000000000000000";
        std::wistringstream win(ws);
        win >> a1;
        assert(a1 == 7000000000000000000_int256, "");
    }

    static void testNativeOperators() {
        auto a1 = ~uint512_t(0xff);
        assert(typeid(uint512_t) == typeid(a1), "");
        for (int idx = 0; idx < 63; ++idx) {
            assert(a1.m_arr[idx] == 0xff, std::to_string(idx));
        }
        assert(a1.m_arr[63] == 0x00, "");

        auto a2 = -uint512_t(0x1);
        assert(typeid(uint512_t) == typeid(a2), "");
        for (int idx = 0; idx < 63; ++idx) {
            assert(a2.m_arr[idx] == 0xff, std::to_string(idx));
        }

        auto a3 = 2 * uint512_t(0x1);
        assert(typeid(uint512_t) == typeid(a3), "");
        for (int idx = 0; idx < 63; ++idx) {
            assert(a3.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a3.m_arr[63] == 2, "");

        auto a4 = uint512_t(0x1) * 2;
        assert(typeid(uint512_t) == typeid(a4), "");
        for (int idx = 0; idx < 63; ++idx) {
            assert(a4.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a4.m_arr[63] == 2, "");

        auto a5 = 4 / uint512_t(0x2);
        assert(typeid(uint512_t) == typeid(a5), "");
        for (int idx = 0; idx < 63; ++idx) {
            assert(a5.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a5.m_arr[63] == 2, "");

        auto a6 = uint512_t(0x4) / 2;
        assert(typeid(uint512_t) == typeid(a6), "");
        for (int idx = 0; idx < 63; ++idx) {
            assert(a6.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a6.m_arr[63] == 2, "");

        auto a7 = 8 % uint512_t(0x5);
        assert(typeid(uint512_t) == typeid(a7), "");
        for (int idx = 0; idx < 63; ++idx) {
            assert(a7.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a7.m_arr[63] == 3, "");

        auto a8 = uint512_t(0x8) % 5;
        assert(typeid(uint512_t) == typeid(a8), "");
        for (int idx = 0; idx < 63; ++idx) {
            assert(a8.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a8.m_arr[63] == 3, "");

        auto a9 = 8 + uint512_t(0x5);
        assert(typeid(uint512_t) == typeid(a9), "");
        for (int idx = 0; idx < 63; ++idx) {
            assert(a9.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a9.m_arr[63] == 13, "");

        auto a10 = uint512_t(0x8) + 5;
        assert(typeid(uint512_t) == typeid(a10), "");
        for (int idx = 0; idx < 63; ++idx) {
            assert(a10.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a10.m_arr[63] == 13, "");

        auto a11 = 8 - uint512_t(0x5);
        assert(typeid(uint512_t) == typeid(a11), "");
        for (int idx = 0; idx < 63; ++idx) {
            assert(a11.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a11.m_arr[63] == 3, "");

        auto a12 = uint512_t(0x8) - 5;
        assert(typeid(uint512_t) == typeid(a12), "");
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

        assert(15U < uint512_t(20), "");
        assert(20U > uint512_t(15), "");
        assert(15U <= uint512_t(20), "");
        assert(20U >= uint512_t(15), "");
        assert(20U <= uint512_t(20), "");
        assert(20U >= uint512_t(20), "");
        assert(20U == uint512_t(20), "");

        assert(uint512_t(15) < uint128_t(20), "");
        assert(uint512_t(20) > uint128_t(15), "");
        assert(uint512_t(15) <= uint128_t(20), "");
        assert(uint512_t(20) >= uint128_t(15), "");
        assert(uint512_t(20) <= uint128_t(20), "");
        assert(uint512_t(20) >= uint128_t(20), "");
        assert(uint512_t(20) == uint128_t(20), "");

        assert(uint128_t(15) < uint512_t(20), "");
        assert(uint128_t(20) > uint512_t(15), "");
        assert(uint128_t(15) <= uint512_t(20), "");
        assert(uint128_t(20) >= uint512_t(15), "");
        assert(uint128_t(20) <= uint512_t(20), "");
        assert(uint128_t(20) >= uint512_t(20), "");
        assert(uint128_t(20) == uint512_t(20), "");

        auto a13 = 9 & uint512_t(5);
        assert(typeid(uint512_t) == typeid(a13), "");
        for (int idx = 0; idx < 63; ++idx) {
            assert(a13.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a13.m_arr[63] == 1, "");

        auto a14 = uint512_t(9) & 5;
        assert(typeid(uint512_t) == typeid(a14), "");
        for (int idx = 0; idx < 63; ++idx) {
            assert(a14.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a14.m_arr[63] == 1, "");

        auto a15 = 9 | uint512_t(5);
        assert(typeid(uint512_t) == typeid(a15), "");
        for (int idx = 0; idx < 63; ++idx) {
            assert(a15.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a15.m_arr[63] == 13, "");

        auto a16 = uint512_t(9) | 5;
        assert(typeid(uint512_t) == typeid(a16), "");
        for (int idx = 0; idx < 63; ++idx) {
            assert(a16.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a16.m_arr[63] == 13, "");

        auto a17 = 4 ^ uint512_t(5);
        assert(typeid(uint512_t) == typeid(a17), "");
        for (int idx = 0; idx < 63; ++idx) {
            assert(a17.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a17.m_arr[63] == 1, "");

        auto a18 = uint512_t(4) ^ 5;
        assert(typeid(uint512_t) == typeid(a18), "");
        for (int idx = 0; idx < 63; ++idx) {
            assert(a18.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a18.m_arr[63] == 1, "");

        auto a19 = uint512_t(8) << 1;
        assert(typeid(uint512_t) == typeid(a19), "");
        for (int idx = 0; idx < 63; ++idx) {
            assert(a19.m_arr[idx] == 0, std::to_string(idx));
        }
        assert(a19.m_arr[63] == 16, "");

        auto a20 = uint512_t(8) >> 1;
        assert(typeid(uint512_t) == typeid(a20), "");
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

        assert(1024_int128 * 2.0 == 2048, "");
        assert(2.0 * 1024_int128 == 2048, "");

        int128_t b = -123;
        assert(b < 0, "");

        uint256_t c1 = 100000000000000000000_uint256;
        uint256_t c2 = 100000000000000000000_uint256;
        uint256_t c3 = c1 * c2;
        assert(c3 == 10000000000000000000000000000000000000000_uint256, "");
    }

    static void testNativeOperatorsAssign() {
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

    static void testNativeOperatorsAssignFloat() {
        uint512_t a1 = 18;

        a1 *= 2.5;
        assert(a1 == 45, "");

        a1 /= 2.5;
        assert(a1 == 18U, "");

        a1 += 2.9;
        assert(a1 == 20, "");

        a1 -= 1.9;
        assert(a1 == 18, "");
    }

    static void testConstexpr() {
        constexpr uint256_t a0{};
        static_assert(a0 == 0, "");
        static_assert(a0 != 1, "");
        static_assert(1 != a0, "");
        static_assert(a0 >= 0U, "");
        static_assert(a0 < 1U, "");
        static_assert(a0 <= 1U, "");

        constexpr uint512_t a1 = 7;
        static_assert(a1 > 0U, "");
        static_assert(a1 == 7, "");
        static_assert(a1 + 12 == 19, "");
        static_assert(12 + a1 == 19, "");

        constexpr int128_t a2 = a1;
        static_assert(a2 == 7, "");
        static_assert(a2 - 3 == 4, "");
        static_assert(11 - a2 == 4, "");

        static_assert(a1 / 2 == 3, "");
        static_assert(a2 * 2 == 14, "");
        static_assert(a1 % 2 == 1, "");

        static_assert(~a2 == -8, "");
        static_assert(-a2 == -7, "");

        static_assert((a1 | 0xff) == 0xff, "");
        static_assert((a1 & 0xff) == 7, "");
        static_assert((a2 ^ 0xff) == 0xf8, "");

        static_assert(0xff_int128 == 255, "");
        static_assert(255_int128 == 255, "");

        static_assert((a1 >> 1) == 3, "");
        static_assert((a1 << 1) == 14, "");

        constexpr uint256_t b = std::numeric_limits<uint256_t>::min();
        static_assert(b == 0, "");

        constexpr int128_t c = std::numeric_limits<int128_t>::min();
        static_assert(c == 0x80000000000000000000000000000000_uint128, "");

        static_assert(0x80000000000000000000000000000000_uint128, "");
    }

    static void testEtc() {
        std::unordered_map<uint256_t, bool> m;
        assert(m.empty(), "");

        static_assert((34562625464547567_uint512 * 234623412124100_uint256) == 8109201118459457025988957064700_uint256, "");
        static_assert((34562625464547567_uint512 / 234623412124100_uint256) == 147, "");
        static_assert((34562625464547567_uint512 + 234623412124100_uint256) == 34797248876671667_uint256, "");
        static_assert((34562625464547567_uint512 - 234623412124100_uint256) == 34328002052423467_uint256, "");
        static_assert((34562625464547567_uint512 % 234623412124100_uint256) == 72983882304867_uint256, "");
        static_assert((34562625464547567_uint512 & 234623412124100_uint512) == 211114906400964_uint256, "");
        static_assert((34562625464547567_uint512 | 234623412124100_uint256) == 34586133970270703_uint256, "");

        static_assert((34562625464547567_uint512 << 3) == 276501003716380536_uint512, "");
        static_assert((34562625464547567_uint512 >> 3) == 4320328183068445_uint512, "");

        static_assert(234623412124100_uint256 < 34562625464547567_uint512, "");
        static_assert(34562625464547567_uint512 > 234623412124100_uint256, "");
        static_assert(234623412124100_uint256 <= 34562625464547567_uint512, "");
        static_assert(34562625464547567_uint512 >= 234623412124100_uint256, "");
        static_assert(34562625464547567_uint512 == 34562625464547567_uint256, "");
        static_assert(34562625464547567_uint512 != 234623412124100_uint256, "");

        //    static_assert(wide_integer<1024,false>::_impl::from_str("1"), "");
    }

    static void testToChars() {
        uint128_t a = 65535;
        int128_t b = -65535;
        uint128_t c = 0;

        char arr1[1];
        std::to_chars_result res = to_chars(arr1, arr1 + sizeof(arr1), a, 1);
        assert(res.ec.value() == (int)std::errc::invalid_argument, "");
        res = to_chars(arr1, arr1 + sizeof(arr1), a, 50);
        assert(res.ec.value() == (int)std::errc::invalid_argument, "");
        res = to_chars(arr1 + sizeof(arr1), arr1, a);
        assert(res.ec.value() == (int)std::errc::invalid_argument, "");

        res = to_chars(arr1, arr1 + sizeof(arr1), a);
        assert(res.ec.value() == (int)std::errc::value_too_large, "");

        char arr2[5];
        res = to_chars(arr2, arr2 + sizeof(arr2), a);
        assert(!res.ec, "");
        res = to_chars(arr2, arr2 + sizeof(arr2), b);
        assert(res.ec.value() == (int)std::errc::value_too_large, "");

        auto check = [](auto num, int base, const std::string& str) {
            char arr3[24];
            std::to_chars_result res = to_chars(arr3, arr3 + sizeof(arr3), num, base);
            assert(!res.ec, "");
            assert(str == std::string(arr3),
                   std::to_string(num) + " " + std::to_string(base) + " " + arr3);
        };

        check(a, 2, "1111111111111111");
        check(a, 3, "10022220020");
        check(a, 4, "33333333");
        check(a, 5, "4044120");
        check(a, 6, "1223223");
        check(a, 7, "362031");
        check(a, 8, "177777");
        check(a, 9, "108806");
        check(a, 10, "65535");
        check(a, 11, "45268");
        check(a, 12, "31b13");
        check(a, 13, "23aa2");
        check(a, 14, "19c51");
        check(a, 15, "14640");
        check(a, 16, "ffff");
        check(a, 17, "d5d0");
        check(a, 18, "b44f");
        check(a, 19, "9aa4");
        check(a, 20, "83gf");
        check(a, 21, "71cf");
        check(a, 22, "638j");
        check(a, 23, "58k8");
        check(a, 24, "4hif");
        check(a, 25, "44la");
        check(a, 26, "3iof");
        check(a, 27, "38o6");
        check(a, 28, "2rgf");
        check(a, 29, "2jqo");
        check(a, 30, "2cof");
        check(a, 31, "2661");
        check(a, 32, "1vvv");
        check(a, 33, "1r5u");
        check(a, 34, "1mnh");
        check(a, 35, "1ihf");
        check(a, 36, "1ekf");

        check(b, 2, "-1111111111111111");
        check(b, 3, "-10022220020");
        check(b, 4, "-33333333");
        check(b, 5, "-4044120");
        check(b, 6, "-1223223");
        check(b, 7, "-362031");
        check(b, 8, "-177777");
        check(b, 9, "-108806");
        check(b, 10, "-65535");
        check(b, 11, "-45268");
        check(b, 12, "-31b13");
        check(b, 13, "-23aa2");
        check(b, 14, "-19c51");
        check(b, 15, "-14640");
        check(b, 16, "-ffff");
        check(b, 17, "-d5d0");
        check(b, 18, "-b44f");
        check(b, 19, "-9aa4");
        check(b, 20, "-83gf");
        check(b, 21, "-71cf");
        check(b, 22, "-638j");
        check(b, 23, "-58k8");
        check(b, 24, "-4hif");
        check(b, 25, "-44la");
        check(b, 26, "-3iof");
        check(b, 27, "-38o6");
        check(b, 28, "-2rgf");
        check(b, 29, "-2jqo");
        check(b, 30, "-2cof");
        check(b, 31, "-2661");
        check(b, 32, "-1vvv");
        check(b, 33, "-1r5u");
        check(b, 34, "-1mnh");
        check(b, 35, "-1ihf");
        check(b, 36, "-1ekf");

        for (int i = 2; i <= 36; ++i) {
            check(c, i, "0");
        }
    }

    template <class T>
    static void checkFromChars(int64_t expect, int base, const std::string& str) {
        T num = 0;
        std::from_chars_result res = from_chars(str.data(), str.data() + str.size(), num, base);
        assert(!res.ec, res.ec.message());
        assert(expect == num,
               std::to_string(expect) + " " + std::to_string(base) + " " + std::to_string(num));
    }

    static void testFromChars() {
        uint128_t a = 0;
        int128_t b = 0;

        char arr1[] = "65535";
        std::from_chars_result res = from_chars(arr1, arr1 + sizeof(arr1), a, 1);
        assert(res.ec.value() == (int)std::errc::invalid_argument, res.ec.message());
        res = from_chars(arr1, arr1 + sizeof(arr1), a, 50);
        assert(res.ec.value() == (int)std::errc::invalid_argument, res.ec.message());
        res = from_chars(arr1 + sizeof(arr1), arr1, a);
        assert(res.ec.value() == (int)std::errc::invalid_argument, res.ec.message());

        char arr2[] = "-65535";
        res = from_chars(arr2, arr2 + sizeof(arr2), a);
        assert(res.ec.value() == (int)std::errc::result_out_of_range, res.ec.message());

        char arr3[] = "-";
        res = from_chars(arr3, arr3 + 1, b);
        assert(res.ec.value() == (int)std::errc::invalid_argument, res.ec.message());

        checkFromChars<uint128_t>(65535, 2, "1111111111111111");
        checkFromChars<uint128_t>(65535, 3, "10022220020");
        checkFromChars<uint128_t>(65535, 4, "33333333");
        checkFromChars<uint128_t>(65535, 5, "4044120");
        checkFromChars<uint128_t>(65535, 6, "1223223");
        checkFromChars<uint128_t>(65535, 7, "362031");
        checkFromChars<uint128_t>(65535, 8, "177777");
        checkFromChars<uint128_t>(65535, 9, "108806");
        checkFromChars<uint128_t>(65535, 10, "65535");
        checkFromChars<uint128_t>(65535, 11, "45268");
        checkFromChars<uint128_t>(65535, 12, "31b13");
        checkFromChars<uint128_t>(65535, 13, "23aa2");
        checkFromChars<uint128_t>(65535, 14, "19c51");
        checkFromChars<uint128_t>(65535, 15, "14640");
        checkFromChars<uint128_t>(65535, 16, "ffff");
        checkFromChars<uint128_t>(65535, 17, "d5d0");
        checkFromChars<uint128_t>(65535, 18, "b44f");
        checkFromChars<uint128_t>(65535, 19, "9aa4");
        checkFromChars<uint128_t>(65535, 20, "83gf");
        checkFromChars<uint128_t>(65535, 21, "71cf");
        checkFromChars<uint128_t>(65535, 22, "638j");
        checkFromChars<uint128_t>(65535, 23, "58k8");
        checkFromChars<uint128_t>(65535, 24, "4hif");
        checkFromChars<uint128_t>(65535, 25, "44la");
        checkFromChars<uint128_t>(65535, 26, "3iof");
        checkFromChars<uint128_t>(65535, 27, "38o6");
        checkFromChars<uint128_t>(65535, 28, "2rgf");
        checkFromChars<uint128_t>(65535, 29, "2jqo");
        checkFromChars<uint128_t>(65535, 30, "2cof");
        checkFromChars<uint128_t>(65535, 31, "2661");
        checkFromChars<uint128_t>(65535, 32, "1vvv");
        checkFromChars<uint128_t>(65535, 33, "1r5u");
        checkFromChars<uint128_t>(65535, 34, "1mnh");
        checkFromChars<uint128_t>(65535, 35, "1ihf");
        checkFromChars<uint128_t>(65535, 36, "1ekf");

        checkFromChars<int128_t>(-65535, 2, "-1111111111111111");
        checkFromChars<int128_t>(-65535, 3, "-10022220020");
        checkFromChars<int128_t>(-65535, 4, "-33333333");
        checkFromChars<int128_t>(-65535, 5, "-4044120");
        checkFromChars<int128_t>(-65535, 6, "-1223223");
        checkFromChars<int128_t>(-65535, 7, "-362031");
        checkFromChars<int128_t>(-65535, 8, "-177777");
        checkFromChars<int128_t>(-65535, 9, "-108806");
        checkFromChars<int128_t>(-65535, 10, "-65535");
        checkFromChars<int128_t>(-65535, 11, "-45268");
        checkFromChars<int128_t>(-65535, 12, "-31b13");
        checkFromChars<int128_t>(-65535, 13, "-23aa2");
        checkFromChars<int128_t>(-65535, 14, "-19c51");
        checkFromChars<int128_t>(-65535, 15, "-14640");
        checkFromChars<int128_t>(-65535, 16, "-ffff");
        checkFromChars<int128_t>(-65535, 17, "-d5d0");
        checkFromChars<int128_t>(-65535, 18, "-b44f");
        checkFromChars<int128_t>(-65535, 19, "-9aa4");
        checkFromChars<int128_t>(-65535, 20, "-83gf");
        checkFromChars<int128_t>(-65535, 21, "-71cf");
        checkFromChars<int128_t>(-65535, 22, "-638j");
        checkFromChars<int128_t>(-65535, 23, "-58k8");
        checkFromChars<int128_t>(-65535, 24, "-4hif");
        checkFromChars<int128_t>(-65535, 25, "-44la");
        checkFromChars<int128_t>(-65535, 26, "-3iof");
        checkFromChars<int128_t>(-65535, 27, "-38o6");
        checkFromChars<int128_t>(-65535, 28, "-2rgf");
        checkFromChars<int128_t>(-65535, 29, "-2jqo");
        checkFromChars<int128_t>(-65535, 30, "-2cof");
        checkFromChars<int128_t>(-65535, 31, "-2661");
        checkFromChars<int128_t>(-65535, 32, "-1vvv");
        checkFromChars<int128_t>(-65535, 33, "-1r5u");
        checkFromChars<int128_t>(-65535, 34, "-1mnh");
        checkFromChars<int128_t>(-65535, 35, "-1ihf");
        checkFromChars<int128_t>(-65535, 36, "-1ekf");

        for (int i = 2; i <= 36; ++i) {
            checkFromChars<uint128_t>(0, i, "0");
        }
    }

    static void testNegative() {
        assert(int256_t(-3) * int256_t(-2) == (-3) * (-2), "");
        assert(int256_t(3) * int256_t(-2) == (3) * (-2), "");
        assert(int256_t(-3) * int256_t(2) == (-3) * (2), "");
        assert(int256_t(3) * int256_t(2) == (3) * (2), "");

        assert(int256_t(-3) / int256_t(-2) == (-3) / (-2), "");
        assert(int256_t(3) / int256_t(-2) == (3) / (-2), "");
        assert(int256_t(-3) / int256_t(2) == (-3) / (2), "");
        assert(int256_t(3) / int256_t(2) == (3) / (2), "");

        assert(int256_t(-3) % int256_t(-2) == (-3) % (-2), "");
        assert(int256_t(3) % int256_t(-2) == (3) % (-2), "");
        assert(int256_t(-3) % int256_t(2) == (-3) % (2), "");
        assert(int256_t(3) % int256_t(2) == (3) % (2), "");

        assert(int256_t(-3) + int256_t(-2) == (-3) + (-2), "");
        assert(int256_t(3) + int256_t(-2) == (3) + (-2), "");
        assert(int256_t(-3) + int256_t(2) == (-3) + (2), "");
        assert(int256_t(3) + int256_t(2) == (3) + (2), "");

        assert(int256_t(-3) - int256_t(-2) == (-3) - (-2), "");
        assert(int256_t(3) - int256_t(-2) == (3) - (-2), "");
        assert(int256_t(-3) - int256_t(2) == (-3) - (2), "");
        assert(int256_t(3) - int256_t(2) == (3) - (2), "");

        assert((int256_t(-3) == int256_t(-2)) == ((-3) == (-2)), "");
        assert((int256_t(3) == int256_t(-2)) == ((3) == (-2)), "");
        assert((int256_t(-3) == int256_t(2)) == ((-3) == (2)), "");
        assert((int256_t(3) == int256_t(2)) == ((3) == (2)), "");

        assert((int256_t(-2) == int256_t(-2)) == ((-2) == (-2)), "");
        assert((int256_t(2) == int256_t(-2)) == ((2) == (-2)), "");
        assert((int256_t(-2) == int256_t(2)) == ((-2) == (2)), "");
        assert((int256_t(2) == int256_t(2)) == ((2) == (2)), "");

        assert((-int256_t(-2) == -int256_t(-2)) == (-(-2) == -(-2)), "");
        assert((-int256_t(2) == -int256_t(-2)) == (-(2) == -(-2)), "");
        assert((-int256_t(-2) == -int256_t(2)) == (-(-2) == -(2)), "");
        assert((-int256_t(2) == -int256_t(2)) == (-(2) == -(2)), "");

        assert((-int256_t(0) == int256_t(0)) == (-(0) == (0)), "");
        assert((int256_t(0) == -int256_t(0)) == ((0) == -(0)), "");

        assert((int256_t(-3) <= int256_t(-2)) == ((-3) <= (-2)), "");
        assert((int256_t(3) <= int256_t(-2)) == ((3) <= (-2)), "");
        assert((int256_t(-3) <= int256_t(2)) == ((-3) <= (2)), "");
        assert((int256_t(3) <= int256_t(2)) == ((3) <= (2)), "");

        assert((int256_t(-2) <= int256_t(-2)) == ((-2) <= (-2)), "");
        assert((int256_t(2) <= int256_t(-2)) == ((2) <= (-2)), "");
        assert((int256_t(-2) <= int256_t(2)) == ((-2) <= (2)), "");
        assert((int256_t(2) <= int256_t(2)) == ((2) <= (2)), "");

        assert((int256_t(-3) < int256_t(-2)) == ((-3) < (-2)), "");
        assert((int256_t(3) < int256_t(-2)) == ((3) < (-2)), "");
        assert((int256_t(-3) < int256_t(2)) == ((-3) < (2)), "");
        assert((int256_t(3) < int256_t(2)) == ((3) < (2)), "");

        assert((int256_t(-2) < int256_t(-2)) == ((-2) < (-2)), "");
        assert((int256_t(2) < int256_t(-2)) == ((2) < (-2)), "");
        assert((int256_t(-2) < int256_t(2)) == ((-2) < (2)), "");
        assert((int256_t(2) < int256_t(2)) == ((2) < (2)), "");

        assert((int256_t(-3) >= int256_t(-2)) == ((-3) >= (-2)), "");
        assert((int256_t(3) >= int256_t(-2)) == ((3) >= (-2)), "");
        assert((int256_t(-3) >= int256_t(2)) == ((-3) >= (2)), "");
        assert((int256_t(3) >= int256_t(2)) == ((3) >= (2)), "");

        assert((int256_t(-2) >= int256_t(-2)) == ((-2) >= (-2)), "");
        assert((int256_t(2) >= int256_t(-2)) == ((2) >= (-2)), "");
        assert((int256_t(-2) >= int256_t(2)) == ((-2) >= (2)), "");
        assert((int256_t(2) >= int256_t(2)) == ((2) >= (2)), "");

        assert((int256_t(-3) > int256_t(-2)) == ((-3) > (-2)), "");
        assert((int256_t(3) > int256_t(-2)) == ((3) > (-2)), "");
        assert((int256_t(-3) > int256_t(2)) == ((-3) > (2)), "");
        assert((int256_t(3) > int256_t(2)) == ((3) > (2)), "");

        assert((int256_t(-2) > int256_t(-2)) == ((-2) > (-2)), "");
        assert((int256_t(2) > int256_t(-2)) == ((2) > (-2)), "");
        assert((int256_t(-2) > int256_t(2)) == ((-2) > (2)), "");
        assert((int256_t(2) > int256_t(2)) == ((2) > (2)), "");
    }

    static void testUnaryIncOrDecr() {
        int512_t a1 = 0;
        assert(++a1 == 1, "");
        assert(a1 == 1, "");

        assert(a1++ == 1, "");
        assert(a1 == 2, "");

        assert(--a1 == 1, "");
        assert(a1 == 1, "");

        assert(a1-- == 1, "");
        assert(a1 == 0, "");

        uint512_t b1 = 0;
        assert(++b1 == 1, "");
        assert(b1 == 1, "");

        assert(b1++ == 1, "");
        assert(b1 == 2, "");

        assert(--b1 == 1, "");
        assert(b1 == 1, "");

        assert(b1-- == 1, "");
        assert(b1 == 0, "");
    }

    static void testSwap() {
        int256_t a = 0;
        int256_t b = 256;

        std::swap(a, b);

        assert(a == 256, "");
        assert(b == 0, "");
    }

    static void testNumericLimits() {
        assert(
            std::numeric_limits<uint128_t>::max() ==
                340282366920938463463374607431768211455_uint128,
            "");
    }

    static void tests() {
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
        testNativeOperatorsAssignFloat();
        testConstexpr();
        testEtc();
        testToChars();
        testFromChars();
        testNegative();
        testUnaryIncOrDecr();
        testSwap();
        testNumericLimits();
        std::cout << wide_integer<64, unsigned>(18) << std::endl;

        std::cout << "OK" << std::endl;
    }
};

int main() {
    _test::tests();

    return 0;
}
