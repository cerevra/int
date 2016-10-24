#pragma once

///////////////////////////////////////////////////////////////
//  Distributed under the Boost Software License, Version 1.0.
//  (See at http://www.boost.org/LICENSE_1_0.txt)
///////////////////////////////////////////////////////////////

#include <stdexcept>
#include <stdint.h>
#include <type_traits>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <limits>


#include <iostream>

constexpr inline bool is_power_of_two(uint16_t bits) {
    return bits == 0x0002 ||
           bits == 0x0004 ||
           bits == 0x0008 ||
           bits == 0x0010 ||
           bits == 0x0020 ||
           bits == 0x0040 ||
           bits == 0x0080 ||
           bits == 0x0100 ||
           bits == 0x0200 ||
           bits == 0x0400 ||
           bits == 0x0800 ||
           bits == 0x1000 ||
           bits == 0x2000 ||
           bits == 0x4000 ||
           bits == 0x8000;
}

template<int bits, bool sgn>
class wide_int {
public:
    static_assert(bits >= 128, "");
    static_assert(is_power_of_two(bits), "");
    using base_type = uint64_t;
    constexpr static int arr_size() noexcept {
        return bits / 64;
    }
    constexpr static int base_bits() noexcept {
        return sizeof(base_type)*8;
    }

private:
    template<int b, bool s>
    constexpr static bool is_negative(const wide_int<b,s>& n) noexcept {
        return s && static_cast<int64_t>(n.m_arr[0]) < 0;
    }

    template<typename T>
    constexpr void wide_int_from_T(T other) noexcept {
        int i = 0;
        for ( ; i < arr_size() - 1; ++i) {
            m_arr[i] = other < 0 ? std::numeric_limits<uint64_t>::max() : 0;
        }
        m_arr[i] = static_cast<uint64_t>(other);
    }
    template<int bits2, bool sgn2>
    constexpr void wide_int_from_wide_int(const wide_int<bits2,sgn2>& other) noexcept {
        int bytes_to_copy = std::min(arr_size(), other.arr_size());
        for (int i = 0; i < bytes_to_copy; ++i) {
            m_arr[arr_size() - 1 - i] = other.m_arr[other.arr_size() - 1 - i];
        }
        for (int i = 0; i < arr_size() - bytes_to_copy; ++i) {
            m_arr[i] = is_negative(other) ? std::numeric_limits<base_type>::max() : 0;
        }
    }

public:
    wide_int() = default;

    template<typename T>
    constexpr wide_int(T other) noexcept {
        wide_int_from_T(other);
    }
    template<int bits2, bool sgn2>
    constexpr wide_int(const wide_int<bits2,sgn2>& other) noexcept {
        wide_int_from_wide_int(other);
    }

    constexpr wide_int(const wide_int<bits,sgn>&) = default;

    template<int bits2, bool sgn2>
    constexpr wide_int<bits,sgn>& operator=(const wide_int<bits2,sgn2>& other) noexcept {
        wide_int_from_wide_int(other);
        return *this;
    }

    template<typename T>
    constexpr wide_int<bits,sgn>& operator=(T other) noexcept {
        wide_int_from_T(other);
        return *this;
    }

    constexpr static wide_int<bits,false> shift_left(const wide_int<bits,false>& other, int n) noexcept {
        if (n >= bits) return 0;
        if (n <= 0) return other;

        wide_int<bits,sgn> num = other;
        int cur_shift = n % base_bits();
        if (cur_shift) {
            num.m_arr[0] <<= cur_shift;
            for (int i = 1; i < arr_size(); ++i) {
                num.m_arr[i-1]  |= num.m_arr[i] >> (base_bits() - cur_shift);
                num.m_arr[i  ] <<= cur_shift;
            }
            n -= cur_shift;
        }
        if (n) {
            int i = 0;
            for (; i < arr_size() - n/base_bits(); ++i) {
                num.m_arr[i] = num.m_arr[i + n/base_bits()];
            }
            for (; i < arr_size(); ++i) {
                num.m_arr[i] = 0;
            }
        }
        return num;
    }

    constexpr static wide_int<bits,true> shift_left(const wide_int<bits,true>& , int ) {
        throw std::runtime_error("shift left for signed numbers is underfined!");
    }

    constexpr static wide_int<bits,false> shift_right(const wide_int<bits,false>& other, int n) noexcept {
        if (n >= bits) return 0;
        if (n <= 0) return other;

        wide_int<bits,sgn> num = other;
        int cur_shift = n % base_bits();
        if (cur_shift) {
            num.m_arr[arr_size() - 1] >>= cur_shift;
            for (int i = arr_size() - 2; i >= 0; --i) {
                num.m_arr[i+1]  |= num.m_arr[i] << (base_bits() - cur_shift);
                num.m_arr[i  ] >>= cur_shift;
            }
            n -= cur_shift;
        }
        if (n) {
            int i = arr_size() - 1;
            for (; i >= static_cast<int>(n/base_bits()); --i) {
                num.m_arr[i] = num.m_arr[i - n/base_bits()];
            }
            for (; i >= 0; --i) {
                num.m_arr[i] = 0;
            }
        }
        return num;
    }

private:
    constexpr static void shift_right_64(uint64_t& num, int n, bool is_neg) noexcept {
        if (is_neg) {
            num = int64_t(num) >> n;
        } else {
            num >>= n;
        }
    }
public:

    constexpr static wide_int<bits,true> shift_right(const wide_int<bits,true>& other, int n) noexcept {
        if (n >= bits) return 0;
        if (n <= 0) return other;

        bool is_neg = is_negative(other);
        if (!is_neg) {
            return shift_right(wide_int<bits,false>(other), n);
        }

        wide_int<bits,sgn> num = other;
        int cur_shift = n % base_bits();
        if (cur_shift) {
            int idx = 0;
            while(num.m_arr[idx] == std::numeric_limits<base_type>::max()) {
                ++idx;
            }
            bool preamb = is_neg;
            base_type prev = is_neg ? std::numeric_limits<base_type>::max() : 0;
            for (; idx < arr_size(); ++idx) {
                base_type curr = num.m_arr[idx];
                shift_right_64(num.m_arr[idx], cur_shift, preamb);
                num.m_arr[idx] |= prev << (sizeof(base_type)*8 - cur_shift);
                prev = curr;
                if (num.m_arr[idx] != std::numeric_limits<base_type>::max()) {
                    preamb = false;
                }
            }
            n -= cur_shift;
        }
        if (n) {
            int i = arr_size() - 1;
            for (; i >= static_cast<int>(n/base_bits()); --i) {
                num.m_arr[i] = num.m_arr[i - n/base_bits()];
            }
            for (; i >= 0; --i) {
                num.m_arr[i] = is_neg ? std::numeric_limits<uint64_t>::max() : 0;
            }
        }
        return num;
    }

    template<typename T>
    constexpr static wide_int<bits,sgn> operator_plus_T(const wide_int<bits,sgn>& num, T other) noexcept {
        if (other < 0) {
            return _operator_minus_T(num, -other);
        } else {
            return _operator_plus_T(num, other);
        }
    }

    template<typename T>
    constexpr static wide_int<bits,sgn> operator_minus_T(const wide_int<bits,sgn>& num, T other) noexcept {
        if (other < 0) {
            return _operator_plus_T(num, -other);
        } else {
            return _operator_minus_T(num, other);
        }
    }

private:
    template<typename T>
    constexpr static wide_int<bits,sgn> _operator_minus_T(const wide_int<bits,sgn>& num, T other) noexcept {
        wide_int<bits,sgn> res = num;

        bool is_underflow = res.m_arr[arr_size() - 1] < base_type(other);
        res.m_arr[arr_size() - 1] -= other;
        if (is_underflow) {
            --res.m_arr[arr_size() - 2];
            for (int i = arr_size() - 3; i >= 0; --i) {
                if (res.m_arr[i + 1] == std::numeric_limits<base_type>::max()) {
                    --res.m_arr[i];
                } else {
                    break;
                }
            }
        }

        return res;
    }

    template<typename T>
    constexpr static wide_int<bits,sgn> _operator_plus_T(const wide_int<bits,sgn>& num, T other) noexcept {
        wide_int<bits,sgn> res = num;

        res.m_arr[arr_size() - 1] += other;
        if (res.m_arr[arr_size() - 1] < base_type(other)) {
            ++res.m_arr[arr_size() - 2];
            for (int i = arr_size() - 3; i >= 0; --i) {
                if (res.m_arr[i + 1] == 0) {
                    ++res.m_arr[i];
                } else {
                    break;
                }
            }
        }

        return res;
    }
public:

//private:
    uint64_t m_arr[arr_size()];
};

using int128_t = wide_int<128,true>;
using uint128_t = wide_int<128,false>;

using int256_t = wide_int<256,true>;
using uint256_t = wide_int<256,false>;

using int512_t = wide_int<512,true>;
using uint512_t = wide_int<512,false>;
