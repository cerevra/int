
#include "int.h"

#include <iostream>
#include <iomanip>

using namespace std;

#define assert(a, info) {if (!(a)) throw std::runtime_error(#a ", line:" + std::to_string(__LINE__) + ", info:" + info );}

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
    for (int idx = 0; idx < 7; ++idx) {
        assert(a2.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a2.m_arr[7] == 13, "");
    uint512_t b2 = 13;
    for (int idx = 0; idx < 7; ++idx) {
        assert(b2.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b2.m_arr[7] == 13, "");

    int512_t a3 = -13;
    for (int idx = 0; idx < 7; ++idx) {
        assert(a3.m_arr[idx] == 0xffffffffffffffff, std::to_string(idx));
    }
    assert(a3.m_arr[7] == 0xfffffffffffffff3, "");
    uint512_t b3 = -13;
    for (int idx = 0; idx < 7; ++idx) {
        assert(b3.m_arr[idx] == 0xffffffffffffffff, std::to_string(idx));
    }
    assert(b3.m_arr[7] == 0xfffffffffffffff3, "");

    int512_t a4 = int128_t(13);
    for (int idx = 0; idx < 7; ++idx) {
        assert(a4.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a4.m_arr[7] == 13, "");
    uint512_t b4 = uint128_t(13);
    for (int idx = 0; idx < 7; ++idx) {
        assert(b4.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b4.m_arr[7] == 13, "");

    int512_t a5 = uint128_t(13);
    for (int idx = 0; idx < 7; ++idx) {
        assert(a5.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a5.m_arr[7] == 13, "");
    uint512_t b5 = int128_t(13);
    for (int idx = 0; idx < 7; ++idx) {
        assert(b5.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b5.m_arr[7] == 13, "");

    int512_t a6 = uint128_t(-13);
    for (int idx = 0; idx < 6; ++idx) {
        assert(a6.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a6.m_arr[6] == 0xffffffffffffffff, "");
    assert(a6.m_arr[7] == 0xfffffffffffffff3, "");
    uint512_t b6 = int128_t(-13);
    for (int idx = 0; idx < 7; ++idx) {
        assert(b6.m_arr[idx] == 0xffffffffffffffff, std::to_string(idx));
    }
    assert(b6.m_arr[7] == 0xfffffffffffffff3, "");

    int512_t a7 = int128_t(-13);
    for (int idx = 0; idx < 7; ++idx) {
        assert(a7.m_arr[idx] == 0xffffffffffffffff, std::to_string(idx));
    }
    assert(a7.m_arr[7] == 0xfffffffffffffff3, "");
    uint512_t b7 = uint128_t(-13);
    for (int idx = 0; idx < 6; ++idx) {
        assert(b7.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b7.m_arr[6] == 0xffffffffffffffff, "");
    assert(b7.m_arr[7] == 0xfffffffffffffff3, "");
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

    uint512_t a3 = 0xf1f2f3f4f5f6f7f8;
    uint512_t a31 = uint512_t::shift_left(a3, 8);
    for (int idx = 0; idx < 6; ++idx) {
        assert(a31.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a31.m_arr[6] == 0x00000000000000f1, "");
    assert(a31.m_arr[7] == 0xf2f3f4f5f6f7f800, "");

    uint512_t a32 = uint512_t::shift_left(a3, 16);
    for (int idx = 0; idx < 6; ++idx) {
        assert(a32.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a32.m_arr[6] == 0x000000000000f1f2, "");
    assert(a32.m_arr[7] == 0xf3f4f5f6f7f80000, "");

    uint512_t a33 = uint512_t::shift_left(a3, 4);
    for (int idx = 0; idx < 6; ++idx) {
        assert(a33.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a33.m_arr[6] == 0x000000000000000f, "");
    assert(a33.m_arr[7] == 0x1f2f3f4f5f6f7f80, "");

    uint512_t a34 = uint512_t::shift_left(a3, 64);
    for (int idx = 0; idx < 6; ++idx) {
        assert(a34.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a34.m_arr[6] == 0xf1f2f3f4f5f6f7f8, "");
    assert(a34.m_arr[7] == 0, "");

    uint512_t a35 = uint512_t::shift_left(a3, 256);
    for (int idx = 0; idx < 3; ++idx) {
        assert(a35.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a35.m_arr[3] == 0xf1f2f3f4f5f6f7f8, "");
    for (int idx = 4; idx < 8; ++idx) {
        assert(a35.m_arr[idx] == 0, std::to_string(idx));
    }

    uint512_t a36 = uint512_t::shift_left(a3, 260);
    assert(a36.m_arr[0] == 0, "");
    assert(a36.m_arr[1] == 0, "");
    assert(a36.m_arr[2] == 0x000000000000000f, "");
    assert(a36.m_arr[3] == 0x1f2f3f4f5f6f7f80, "");
    for (int idx = 4; idx < 8; ++idx) {
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

    uint512_t a6 = 0xf1f2f3f4f5f6f7f8;
    a6.m_arr[0] = 0xf1f2f3f4f5f6f7f8;
    uint512_t a61 = uint512_t::shift_right(a6, 8);
    assert(a61.m_arr[0] == 0x00f1f2f3f4f5f6f7, "");
    assert(a61.m_arr[1] == 0xf800000000000000, "");
    for (int idx = 2; idx < 7; ++idx) {
        assert(a61.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a61.m_arr[7] == 0x00f1f2f3f4f5f6f7, "");

    uint512_t a62 = uint512_t::shift_right(a6, 16);
    assert(a62.m_arr[0] == 0x0000f1f2f3f4f5f6, "");
    assert(a62.m_arr[1] == 0xf7f8000000000000, "");
    for (int idx = 2; idx < 6; ++idx) {
        assert(a62.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a62.m_arr[7] == 0x0000f1f2f3f4f5f6, "");

    uint512_t a63 = uint512_t::shift_right(a6, 4);
    assert(a63.m_arr[0] == 0x0f1f2f3f4f5f6f7f, "");
    assert(a63.m_arr[1] == 0x8000000000000000, "");
    for (int idx = 2; idx < 6; ++idx) {
        assert(a63.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a63.m_arr[7] == 0x0f1f2f3f4f5f6f7f, "");

    uint512_t a64 = uint512_t::shift_right(a6, 64);
    assert(a64.m_arr[0] == 0, "");
    assert(a64.m_arr[1] == 0xf1f2f3f4f5f6f7f8, "");
    for (int idx = 2; idx < 7; ++idx) {
        assert(a64.m_arr[idx] == 0, std::to_string(idx));
    }

    uint512_t a65 = uint512_t::shift_right(a6, 256);
    for (int idx = 0; idx < 4; ++idx) {
        assert(a65.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a65.m_arr[4] == 0xf1f2f3f4f5f6f7f8, "");
    for (int idx = 5; idx < 8; ++idx) {
        assert(a65.m_arr[idx] == 0, std::to_string(idx));
    }

    uint512_t a66 = uint512_t::shift_right(a6, 260);
    for (int idx = 0; idx < 4; ++idx) {
        assert(a66.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a66.m_arr[4] == 0x0f1f2f3f4f5f6f7f, "");
    assert(a66.m_arr[5] == 0x8000000000000000, "");
    for (int idx = 6; idx < 8; ++idx) {
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

    int512_t b6 = 0xf1f2f3f4f5f6f7f8;
    b6.m_arr[0] = 0x71f2f3f4f5f6f7f8;
    int512_t b61 = int512_t::shift_right(b6, 8);
    assert(b61.m_arr[0] == 0x0071f2f3f4f5f6f7, "");
    assert(b61.m_arr[1] == 0xf800000000000000, "");
    for (int idx = 2; idx < 7; ++idx) {
        assert(b61.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b61.m_arr[7] == 0x00f1f2f3f4f5f6f7, "");

    int512_t b62 = int512_t::shift_right(b6, 16);
    assert(b62.m_arr[0] == 0x000071f2f3f4f5f6, "");
    assert(b62.m_arr[1] == 0xf7f8000000000000, "");
    for (int idx = 2; idx < 6; ++idx) {
        assert(b62.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b62.m_arr[7] == 0x0000f1f2f3f4f5f6, "");

    int512_t b63 = int512_t::shift_right(b6, 4);
    assert(b63.m_arr[0] == 0x071f2f3f4f5f6f7f, "");
    assert(b63.m_arr[1] == 0x8000000000000000, "");
    for (int idx = 2; idx < 6; ++idx) {
        assert(b63.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b63.m_arr[7] == 0x0f1f2f3f4f5f6f7f, "");

    int512_t b64 = int512_t::shift_right(b6, 64);
    assert(b64.m_arr[0] == 0, "");
    assert(b64.m_arr[1] == 0x71f2f3f4f5f6f7f8, "");
    for (int idx = 2; idx < 7; ++idx) {
        assert(b64.m_arr[idx] == 0, std::to_string(idx));
    }

    int512_t b65 = int512_t::shift_right(b6, 256);
    for (int idx = 0; idx < 4; ++idx) {
        assert(b65.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b65.m_arr[4] == 0x71f2f3f4f5f6f7f8, "");
    for (int idx = 5; idx < 8; ++idx) {
        assert(b65.m_arr[idx] == 0, std::to_string(idx));
    }

    int512_t b66 = int512_t::shift_right(b6, 260);
    for (int idx = 0; idx < 4; ++idx) {
        assert(b66.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b66.m_arr[4] == 0x071f2f3f4f5f6f7f, "");
    assert(b66.m_arr[5] == 0x8000000000000000, "");
    for (int idx = 6; idx < 8; ++idx) {
        assert(b66.m_arr[idx] == 0, std::to_string(idx));
    }

    //right int < 0
    int512_t b7 = 0xf1f2f3f4f5f6f7f8;
    b7.m_arr[0] = 0xf1f2f3f4f5f6f7f8;
    int512_t b71 = int512_t::shift_right(b7, 8);
    assert(b71.m_arr[0] == 0xfff1f2f3f4f5f6f7, "");
    assert(b71.m_arr[1] == 0xf800000000000000, "");
    for (int idx = 2; idx < 7; ++idx) {
        assert(b71.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b71.m_arr[7] == 0x00f1f2f3f4f5f6f7, "");

    int512_t b72 = int512_t::shift_right(b7, 16);
    assert(b72.m_arr[0] == 0xfffff1f2f3f4f5f6, "");
    assert(b72.m_arr[1] == 0xf7f8000000000000, "");
    for (int idx = 2; idx < 6; ++idx) {
        assert(b72.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b72.m_arr[7] == 0x0000f1f2f3f4f5f6, "");

    int512_t b73 = int512_t::shift_right(b7, 4);
    assert(b73.m_arr[0] == 0xff1f2f3f4f5f6f7f, "");
    assert(b73.m_arr[1] == 0x8000000000000000, "");
    for (int idx = 2; idx < 6; ++idx) {
        assert(b73.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b73.m_arr[7] == 0x0f1f2f3f4f5f6f7f, "");

    int512_t b74 = int512_t::shift_right(b7, 64);
    assert(b74.m_arr[0] == 0xffffffffffffffff, "");
    assert(b74.m_arr[1] == 0xf1f2f3f4f5f6f7f8, "");
    for (int idx = 2; idx < 7; ++idx) {
        assert(b74.m_arr[idx] == 0, std::to_string(idx));
    }

    int512_t b75 = int512_t::shift_right(b7, 256);
    for (int idx = 0; idx < 4; ++idx) {
        assert(b75.m_arr[idx] == 0xffffffffffffffff, std::to_string(idx));
    }
    assert(b75.m_arr[4] == 0xf1f2f3f4f5f6f7f8, "");
    for (int idx = 5; idx < 8; ++idx) {
        assert(b75.m_arr[idx] == 0, std::to_string(idx));
    }

    int512_t b76 = int512_t::shift_right(b7, 260);
    for (int idx = 0; idx < 4; ++idx) {
        assert(b76.m_arr[idx] == 0xffffffffffffffff, std::to_string(idx));
    }
    assert(b76.m_arr[4] == 0xff1f2f3f4f5f6f7f, "");
    assert(b76.m_arr[5] == 0x8000000000000000, "");
    for (int idx = 6; idx < 8; ++idx) {
        assert(b76.m_arr[idx] == 0, std::to_string(idx));
    }

    int512_t b8 = -13;
    int512_t b81 = int512_t::shift_right(b8, 1);
    for (int idx = 0; idx < 7; ++idx) {
        assert(b81.m_arr[idx] == 0xffffffffffffffff, std::to_string(idx));
    }
    assert(b81.m_arr[7] == 0xfffffffffffffff9, "");
}

void testAssign() {
    uint512_t a1 = 0;
    int512_t b1 = 0;

    a1 = 4;
    for (int idx = 0; idx < 7; ++idx) {
        assert(a1.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a1.m_arr[7] == 4, "");
    b1 = 4;
    for (int idx = 0; idx < 7; ++idx) {
        assert(b1.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b1.m_arr[7] == 4, "");

    a1 = -4;
    for (int idx = 0; idx < 7; ++idx) {
        assert(a1.m_arr[idx] == 0xffffffffffffffff, std::to_string(idx));
    }
    assert(a1.m_arr[7] == 0xfffffffffffffffc, "");
    b1 = -4;
    for (int idx = 0; idx < 7; ++idx) {
        assert(b1.m_arr[idx] == 0xffffffffffffffff, std::to_string(idx));
    }
    assert(b1.m_arr[7] == 0xfffffffffffffffc, "");

    a1 = uint128_t(4);
    for (int idx = 0; idx < 7; ++idx) {
        assert(a1.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a1.m_arr[7] == 4, "");
    b1 = int128_t(4);
    for (int idx = 0; idx < 7; ++idx) {
        assert(b1.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b1.m_arr[7] == 4, "");

    a1 = uint128_t(-4);
    for (int idx = 0; idx < 6; ++idx) {
        assert(a1.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a1.m_arr[6] == 0xffffffffffffffff, "");
    assert(a1.m_arr[7] == 0xfffffffffffffffc, "");
    b1 = int128_t(-4);
    for (int idx = 0; idx < 7; ++idx) {
        assert(b1.m_arr[idx] == 0xffffffffffffffff, std::to_string(idx));
    }
    assert(b1.m_arr[7] == 0xfffffffffffffffc, "");

    a1 = int128_t(4);
    for (int idx = 0; idx < 7; ++idx) {
        assert(a1.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(a1.m_arr[7] == 4, "");
    b1 = uint128_t(4);
    for (int idx = 0; idx < 7; ++idx) {
        assert(b1.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b1.m_arr[7] == 4, "");

    a1 = int128_t(-4);
    for (int idx = 0; idx < 7; ++idx) {
        assert(a1.m_arr[idx] == 0xffffffffffffffff, std::to_string(idx));
    }
    assert(a1.m_arr[7] == 0xfffffffffffffffc, "");
    b1 = uint128_t(-4);
    for (int idx = 0; idx < 6; ++idx) {
        assert(b1.m_arr[idx] == 0, std::to_string(idx));
    }
    assert(b1.m_arr[6] == 0xffffffffffffffff, "");
    assert(b1.m_arr[7] == 0xfffffffffffffffc, "");
}

void tests() {
    static_assert(std::is_pod<int512_t>::value, "");
    static_assert(std::is_pod<uint512_t>::value, "");
    static_assert(std::is_standard_layout<int512_t>::value, "");
    static_assert(std::is_standard_layout<uint512_t>::value, "");

    testCtors();
    testShifts();
    testAssign();
}

int main() {
    tests();

    wide_int<512, true> a(13);
    a.m_arr[0] = std::numeric_limits<uint64_t>::max();
//    wide_int<128> b = 18;
    std::cout << "--- " << (wide_int<512, true>::shift_right(a, 0)).m_arr[7] << std::endl;

    for (uint64_t u : wide_int<512, true>::shift_right(a, 0).m_arr) {
        std::cout <<std::hex << std::setw(16) << std::setfill('0') << u << std::endl;
    }

    //    std::cout << b << std::endl;

//    wide_int<256> c = 200;
//    std::cout << c << std::endl;

//    wide_int<512> d = 400;
//    std::cout << d << std::endl;
//    d = 15;
//    std::cout << d << std::endl;
//    d = c + 22;
//    std::cout << d << std::endl;
//    d = 20 + c;
//    std::cout << d << std::endl;

//    // TODO
//    std::cout << std::numeric_limits<wide_int<512>>::max() << std::endl;

//    std::cout << 111123123123123123123_uint128 << std::endl;

    std::cout << std::hex << std::setw(16) << std::setfill('0');
    std::cout << uint64_t(uint32_t(-13)) << std::endl;
//    std::cout << std::hex << std::setw(16) << std::setfill('0');
//    std::cout << (-int64_t(0xc0c0) >> 1) << std::endl;

//    std::cout << std::hex << std::setw(16) << std::setfill('0');
//    std::cout << int64_t(-4) << std::endl;
//    std::cout << std::hex << std::setw(16) << std::setfill('0');
//    std::cout << (int64_t(-1) << 1) << std::endl;
//    std::cout << std::hex << std::setw(16) << std::setfill('0');
//    std::cout << (int64_t(-1) << 16) << std::endl;
//    std::cout << std::hex << std::setw(16) << std::setfill('0');
//    std::cout << (int64_t(-1) >> 8) << std::endl;
//    std::cout << std::hex << std::setw(16) << std::setfill('0');
//    std::cout << (int64_t(-1) >> 16) << std::endl;

    return 0;
}
