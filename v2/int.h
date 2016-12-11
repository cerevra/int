#pragma once

///////////////////////////////////////////////////////////////
//  Distributed under the Boost Software License, Version 1.0.
//  (See at http://www.boost.org/LICENSE_1_0.txt)
///////////////////////////////////////////////////////////////

/*  Divide and multiply
 *
 *
 * Copyright (c) 2008
 * Evan Teran
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appears in all copies and that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the same name not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission. We make no representations about the
 * suitability this software for any purpose. It is provided "as is"
 * without express or implied warranty.
 */

#include <cstdint>
#include <cmath>
#include <stdexcept>
#include <stdint.h>
#include <type_traits>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <limits>


#include <iostream>

constexpr inline bool is_multiple_of_8(size_t Bytes) {
    return Bytes % 8 == 0;
}

template<size_t Bytes, bool Signed>
class wide_int;

namespace std {
// type traits
//template<size_t Bytes,bool Signed>
//struct is_integral<wide_int<Bytes,Signed>> : std::true_type {};

//template<size_t Bytes,bool Signed>
//struct is_arithmetic<wide_int<Bytes,Signed>> : std::true_type {};

template<size_t Bytes,bool Signed>
struct is_scalar<wide_int<Bytes,Signed>> : std::true_type {};

template<size_t Bytes,bool Signed>
struct is_compound<wide_int<Bytes,Signed>> : std::true_type {};

template<size_t Bytes>
struct is_unsigned<wide_int<Bytes,true>> : std::false_type {};
template<size_t Bytes>
struct is_unsigned<wide_int<Bytes,false>> : std::true_type {};

template<size_t Bytes>
struct is_signed<wide_int<Bytes,true>> : std::true_type {};
template<size_t Bytes>
struct is_signed<wide_int<Bytes,false>> : std::false_type {};
}

template<size_t Bytes, bool Signed>
class wide_int {
public:
//    static_assert(Bytes > 64, "");
//    static_assert(is_multiple_of_8(Bytes), "");
    using base_type = uint8_t;
    using signed_base_type = int8_t;
    constexpr static int arr_size() noexcept {
//        return Bytes/base_bits(); // for template<size_t Bits, bool Signed>
        return Bytes; // for template<size_t Bytes, bool Signed>
    }
    constexpr static int base_bits() noexcept {
        return sizeof(base_type)*CHAR_BIT;
    }

    template<size_t B, bool S>
    constexpr static bool is_negative(const wide_int<B,S>& n) noexcept {
        return S && static_cast<signed_base_type>(n.m_arr[0]) < 0;
    }

private:
    template<typename T>
    constexpr void wide_int_from_T(T other) noexcept {
        int r_idx = 0;

        for (; static_cast<size_t>(r_idx) < sizeof(T) && r_idx < arr_size(); ++r_idx) {
            base_type& curr = m_arr[arr_size() - 1 - r_idx];
            base_type curr_other = (other >> (r_idx*CHAR_BIT)) & std::numeric_limits<base_type>::max();
            curr = curr_other;
        }

        for ( ; r_idx < arr_size(); ++r_idx) {
            base_type& curr = m_arr[arr_size() - 1 - r_idx];
            curr = other < 0 ? std::numeric_limits<base_type>::max() : 0;
        }
    }
    template<size_t Bytes2, bool Signed2>
    constexpr void wide_int_from_wide_int(const wide_int<Bytes2,Signed2>& other) noexcept {
//        int bytes_to_copy = std::min(arr_size(), other.arr_size());
        int bytes_to_copy = arr_size() < other.arr_size() ? arr_size()
                                                          : other.arr_size();
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
    constexpr wide_int(T other) noexcept
        : m_arr {}
    {
        wide_int_from_T(other);
    }
    template<size_t Bytes2, bool Signed2>
    constexpr wide_int(const wide_int<Bytes2,Signed2>& other) noexcept
        : m_arr {}
    {
        wide_int_from_wide_int(other);
    }

    template<size_t Bytes2, bool Signed2>
    constexpr wide_int<Bytes,Signed>& operator=(const wide_int<Bytes2,Signed2>& other) noexcept {
        wide_int_from_wide_int(other);
        return *this;
    }

    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator=(T other) noexcept {
        wide_int_from_T(other);
        return *this;
    }

    constexpr static wide_int<Bytes,false> shift_left(const wide_int<Bytes,false>& other, int n) noexcept {
        if (static_cast<size_t>(n) >= base_bits()*arr_size()) return 0;
        if (n <= 0) return other;

        wide_int<Bytes,Signed> num = other;
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

    constexpr static wide_int<Bytes,true> shift_left(const wide_int<Bytes,true>& other, int n) {
//        static_assert(is_negative(other), "shift left for negative numbers is underfined!");
        if (is_negative(other)) {
            throw std::runtime_error("shift left for negative numbers is underfined!");
        }
        return wide_int<Bytes,true>(shift_left(wide_int<Bytes,false>(other), n));
    }

    constexpr static wide_int<Bytes,false> shift_right(const wide_int<Bytes,false>& other, int n) noexcept {
        if (static_cast<size_t>(n) >= base_bits()*arr_size()) return 0;
        if (n <= 0) return other;

        wide_int<Bytes,Signed> num = other;
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

    constexpr static wide_int<Bytes,true> shift_right(const wide_int<Bytes,true>& other, int n) noexcept {
        if (static_cast<size_t>(n) >= base_bits()*arr_size()) return 0;
        if (n <= 0) return other;

        bool is_neg = is_negative(other);
        if (!is_neg) {
            return shift_right(wide_int<Bytes,false>(other), n);
        }

        wide_int<Bytes,Signed> num = other;
        int cur_shift = n % base_bits();
        if (cur_shift) {
            num = shift_right(wide_int<Bytes,false>(num), cur_shift);
            num.m_arr[0] |= std::numeric_limits<base_type>::max() << (base_bits() - cur_shift);
            n -= cur_shift;
        }
        if (n) {
            int i = arr_size() - 1;
            for (; i >= static_cast<int>(n/base_bits()); --i) {
                num.m_arr[i] = num.m_arr[i - n/base_bits()];
            }
            for (; i >= 0; --i) {
                num.m_arr[i] = std::numeric_limits<base_type>::max();
            }
        }
        return num;
    }

    template<typename T>
    constexpr static wide_int<Bytes,Signed> operator_plus_T(const wide_int<Bytes,Signed>& num, T other) noexcept {
        if (other < 0) {
            return _operator_minus_T(num, -other);
        } else {
            return _operator_plus_T(num, other);
        }
    }

private:
    template<typename T>
    constexpr static wide_int<Bytes,Signed> _operator_minus_T(const wide_int<Bytes,Signed>& num, T other) noexcept {
        wide_int<Bytes,Signed> res = num;

        bool is_underflow = false;
        int r_idx = 0;
        for (; static_cast<size_t>(r_idx) < sizeof(T) && r_idx < arr_size(); ++r_idx) {
            base_type& res_i = res.m_arr[arr_size() - 1 - r_idx];
            base_type curr_other = (other >> (r_idx*CHAR_BIT)) & std::numeric_limits<base_type>::max();

            if (is_underflow) {
                --res_i;
                is_underflow = res_i == std::numeric_limits<base_type>::max();
            }

            if (res_i < curr_other) {
                is_underflow = true;
            }
            res_i -= curr_other;
        }

        if (is_underflow && r_idx < arr_size()) {
            --res.m_arr[arr_size() - 1 - r_idx];
            for (int i = arr_size() - 1 - r_idx - 1; i >= 0; --i) {
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
    constexpr static wide_int<Bytes,Signed> _operator_plus_T(const wide_int<Bytes,Signed>& num, T other) noexcept {
        wide_int<Bytes,Signed> res = num;

        bool is_overflow = false;
        int r_idx = 0;
        for (; static_cast<size_t>(r_idx) < sizeof(T) && r_idx < arr_size(); ++r_idx) {
            base_type& res_i = res.m_arr[arr_size() - 1 - r_idx];
            base_type curr_other = (other >> (r_idx*CHAR_BIT)) & std::numeric_limits<base_type>::max();

            if (is_overflow) {
                ++res_i;
                is_overflow = res_i == 0;
            }

            res_i += curr_other;
            if (res_i < curr_other) {
                is_overflow = true;
            }
        }

        if (is_overflow && r_idx < arr_size()) {
            ++res.m_arr[arr_size() - 1 - r_idx];
            for (int i = arr_size() - 1 - r_idx - 1; i >= 0; --i) {
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

    constexpr static wide_int<Bytes,Signed> operator_unary_tilda(const wide_int<Bytes,Signed>& num) noexcept {
        wide_int<Bytes,Signed> res {};
        for (int i = 0; i < arr_size(); ++i) {
            res.m_arr[i] = ~num.m_arr[i];
        }
        return res;
    }

    constexpr static wide_int<Bytes,Signed> operator_unary_minus(const wide_int<Bytes,Signed>& num) noexcept {
        return operator_plus_T(operator_unary_tilda(num), 1);
    }

    template<typename T>
    using __keep_size = typename std::enable_if<sizeof(T) <= Bytes, wide_int<Bytes,Signed>>::type;
    template<size_t Bytes2, bool Signed2>
    using __need_increase_size = typename std::enable_if<Bytes < Bytes2, wide_int<Bytes2,Signed>>::type;

    template<typename T, class = __keep_size<T>>
    constexpr static wide_int<Bytes,Signed> operator_plus(const wide_int<Bytes,Signed>& num,
                                                      const T& other) noexcept {
        wide_int<Bytes,Signed> t = other;
        if (is_negative(t)) {
            return _operator_minus_wide_int(num, operator_unary_minus(t));
        } else {
            return _operator_plus_wide_int(num, t);
        }
    }

    template<size_t Bytes2, bool Signed2, class = __need_increase_size<Bytes2, Signed2>>
    constexpr static wide_int<Bytes2, Signed> operator_plus(const wide_int<Bytes,Signed>& num,
                                                        const wide_int<Bytes2,Signed2>& other) noexcept {
        return wide_int<Bytes2,Signed>::operator_plus(wide_int<Bytes2,Signed>(num), other);
    }

    template<typename T, class = __keep_size<T>>
    constexpr static wide_int<Bytes,Signed> operator_minus(const wide_int<Bytes,Signed>& num,
                                                       const T& other) noexcept {
        wide_int<Bytes,Signed> t = other;
        if (is_negative(t)) {
            return _operator_plus_wide_int(num, operator_unary_minus(t));
        } else {
            return _operator_minus_wide_int(num, t);
        }
    }

    template<size_t Bytes2, bool Signed2, class = __need_increase_size<Bytes2, Signed2>>
    constexpr static wide_int<Bytes2, Signed> operator_minus(const wide_int<Bytes,Signed>& num,
                                                         const wide_int<Bytes2,Signed2>& other) noexcept {
        return wide_int<Bytes2,Signed>::operator_minus(wide_int<Bytes2,Signed>(num), other);
    }
private:
    constexpr static wide_int<Bytes,Signed> _operator_minus_wide_int(const wide_int<Bytes,Signed>& num,
                                                                 const wide_int<Bytes,Signed>& other) noexcept {
        wide_int<Bytes,Signed> res = num;

        bool is_underflow = false;
        for (int idx = arr_size() - 1; idx >= 0; --idx) {
            base_type& res_i = res.m_arr[idx];
            const base_type other_i = other.m_arr[idx];

            if (is_underflow) {
                --res_i;
                is_underflow = res_i == std::numeric_limits<base_type>::max();
            }

            if (res_i < other_i) {
                is_underflow = true;
            }
            res_i -= other_i;
        }

        return res;
    }

    constexpr static wide_int<Bytes,Signed> _operator_plus_wide_int(const wide_int<Bytes,Signed>& num,
                                                                const wide_int<Bytes,Signed>& other) noexcept {
        wide_int<Bytes,Signed> res = num;

        bool is_overflow = false;
        for (int idx = arr_size() - 1; idx >= 0; --idx) {
            base_type& res_i = res.m_arr[idx];
            const base_type other_i = other.m_arr[idx];

            if (is_overflow) {
                ++res_i;
                is_overflow = res_i == 0;
            }

            res_i += other_i;
            if (res_i < other_i) {
                is_overflow = true;
            }
        }

        return res;
    }
public:


    template<typename T, class = __keep_size<T>>
    constexpr static wide_int<Bytes,Signed> operator_star(const wide_int<Bytes,Signed>& num,
                                                      const T& other) noexcept {

        wide_int<Bytes,Signed> a(num);
        wide_int<Bytes,Signed> t = other;

        wide_int<Bytes,Signed> res = 0;

        for (size_t i = 0; i < Bytes; ++i) {
            if((t.m_arr[arr_size() - 1] & 1) != 0) {
                res = operator_plus(res, (shift_left(a, i)));
            }

            t = shift_right(t, 1);
        }

        return res;
    }

    template<size_t Bytes2, bool Signed2, class = __need_increase_size<Bytes2, Signed2>>
    constexpr static wide_int<Bytes2, Signed2> operator_star(const wide_int<Bytes,Signed>& num,
                                                         const wide_int<Bytes2,Signed2>& other) noexcept {
        return wide_int<Bytes2,Signed2>::operator_star(wide_int<Bytes2,Signed2>(num), other);
    }

    template<typename T, class = __keep_size<T>>
    constexpr static bool operator_more(const wide_int<Bytes,Signed>& num,
                                        const T& other) noexcept {
//        static_assert(Signed == std::is_signed<T>::value,
//                      "warning: operator_more: comparison of integers of different signs");

        wide_int<Bytes,Signed> t = other;

        if (std::is_signed<T>::value && (is_negative(num) != is_negative(t))) {
            return is_negative(t);
        }

        for (int i = 0; i < arr_size(); ++i) {
            if (num.m_arr[i] != t.m_arr[i]) {
                return num.m_arr[i] > t.m_arr[i];
            }
        }

        return false;
    }

    template<size_t Bytes2, class = __need_increase_size<Bytes2, Signed>>
    constexpr static bool operator_more(const wide_int<Bytes,Signed>& num,
                                        const wide_int<Bytes2,Signed>& other) noexcept {
        return wide_int<Bytes2,Signed>::operator_more(wide_int<Bytes2,Signed>(num), other);
    }

    template<typename T, class = __keep_size<T>>
    constexpr static bool operator_less(const wide_int<Bytes,Signed>& num,
                                        const T& other) noexcept {
//        static_assert(Signed == std::is_signed<T>::value,
//                      "warning: operator_less: comparison of integers of different signs");

        wide_int<Bytes,Signed> t = other;

        if (std::is_signed<T>::value && (is_negative(num) != is_negative(t))) {
            return is_negative(t);
        }

        for (int i = 0; i < arr_size(); ++i) {
            if (num.m_arr[i] != t.m_arr[i]) {
                return num.m_arr[i] < t.m_arr[i];
            }
        }

        return false;
    }

    template<size_t Bytes2, class = __need_increase_size<Bytes2, Signed>>
    constexpr static bool operator_less(const wide_int<Bytes,Signed>& num,
                                        const wide_int<Bytes2,Signed>& other) noexcept {
        return wide_int<Bytes2,Signed>::operator_less(wide_int<Bytes2,Signed>(num), other);
    }

    template<typename T, class = __keep_size<T>>
    constexpr static bool operator_eq(const wide_int<Bytes,Signed>& num,
                                      const T& other) noexcept {
        wide_int<Bytes,Signed> t = other;

        for (int i = 0; i < arr_size(); ++i) {
            if (num.m_arr[i] != t.m_arr[i]) {
                return false;
            }
        }

        return true;
    }

    template<size_t Bytes2, class = __need_increase_size<Bytes2, Signed>>
    constexpr static bool operator_eq(const wide_int<Bytes,Signed>& num,
                                        const wide_int<Bytes2,Signed>& other) noexcept {
        return wide_int<Bytes2,Signed>::operator_eq(wide_int<Bytes2,Signed>(num), other);
    }

    template<typename T, class = __keep_size<T>>
    constexpr static wide_int<Bytes,Signed> operator_pipe(const wide_int<Bytes,Signed>& num,
                                                      const T& other) noexcept {
        wide_int<Bytes,Signed> t = other;
        wide_int<Bytes,Signed> res = num;

        for (int i = 0; i < arr_size(); ++i) {
            res.m_arr[i] |= t.m_arr[i];
        }

        return res;
    }

    template<size_t Bytes2, class = __need_increase_size<Bytes2, Signed>>
    constexpr static wide_int<Bytes2,Signed> operator_pipe(const wide_int<Bytes,Signed>& num,
                                                       const wide_int<Bytes2,Signed>& other) noexcept {
        return wide_int<Bytes2,Signed>::operator_pipe(wide_int<Bytes2,Signed>(num), other);
    }

    template<typename T, class = __keep_size<T>>
    constexpr static wide_int<Bytes,Signed> operator_amp(const wide_int<Bytes,Signed>& num,
                                                     const T& other) noexcept {
        wide_int<Bytes,Signed> t = other;
        wide_int<Bytes,Signed> res = num;

        for (int i = 0; i < arr_size(); ++i) {
            res.m_arr[i] &= t.m_arr[i];
        }

        return res;
    }

    template<size_t Bytes2, class = __need_increase_size<Bytes2, Signed>>
    constexpr static wide_int<Bytes2,Signed> operator_amp(const wide_int<Bytes,Signed>& num,
                                                      const wide_int<Bytes2,Signed>& other) noexcept {
        return wide_int<Bytes2,Signed>::operator_amp(wide_int<Bytes2,Signed>(num), other);
    }

private:
    template <typename T>
    constexpr static void divide(const T &numerator, const T &denominator, T &quotient, T &remainder) {

        bool is_zero = true;
        for (auto c : denominator.m_arr) {
            if (c != 0) {
                is_zero = false;
                break;
            }
        }

        if(is_zero) {
            throw std::domain_error("divide by zero");
        }

        T n      = numerator;
        T d      = denominator;
        T x      = 1;
        T answer = 0;

        while(!operator_more(d, n) && operator_eq( operator_amp( shift_right(d, base_bits()*arr_size() - 1), 1), 0)) {
            x = shift_left(x, 1);
            d = shift_left(d, 1);
        }

        while(!operator_eq(x, 0)) {
            if(!operator_more(d, n)) {
                n = operator_minus(n, d);
                answer = operator_pipe(answer, x);
            }

            x = shift_right(x, 1);
            d = shift_right(d, 1);
        }

        quotient = answer;
        remainder = n;
    }
public:

    template<typename T, class = __keep_size<T>>
    constexpr static wide_int<Bytes,Signed> operator_slash(const wide_int<Bytes,Signed>& num,
                                                       const T& other) noexcept {

        wide_int<Bytes,Signed> quotient{}, remainder{};
        divide(num, wide_int<Bytes,Signed>(other), quotient, remainder);
        return quotient;
    }

    template<size_t Bytes2, bool Signed2, class = __need_increase_size<Bytes2, Signed2>>
    constexpr static wide_int<Bytes2, Signed2> operator_slash(const wide_int<Bytes,Signed>& num,
                                                          const wide_int<Bytes2,Signed2>& other) noexcept {
        return wide_int<Bytes2,Signed2>::operator_slash(wide_int<Bytes2,Signed2>(num), other);
    }

    template<typename T, class = __keep_size<T>>
    constexpr static wide_int<Bytes,Signed> operator_percent(const wide_int<Bytes,Signed>& num,
                                                         const T& other) noexcept {

        wide_int<Bytes,Signed> quotient{}, remainder{};
        divide(num, wide_int<Bytes,Signed>(other), quotient, remainder);
        return remainder;
    }

    template<size_t Bytes2, bool Signed2, class = __need_increase_size<Bytes2, Signed2>>
    constexpr static wide_int<Bytes2, Signed2> operator_percent(const wide_int<Bytes,Signed>& num,
                                                            const wide_int<Bytes2,Signed2>& other) noexcept {
        return wide_int<Bytes2,Signed2>::operator_percent(wide_int<Bytes2,Signed2>(num), other);
    }


    template <class T>
    using __arithm_not_wide_int_class = typename std::enable_if< std::is_arithmetic<T>::value && sizeof(T) <= 64, T>::type;

    template <class T, class = __arithm_not_wide_int_class<T> >
    constexpr operator T () const noexcept {
        T res = 0;
        for (size_t r_idx = 0; r_idx < arr_size() && r_idx < sizeof(T); ++r_idx) {
            res |= m_arr[arr_size() - 1 - r_idx] << (base_bits() * r_idx);
        }
        return res;
    }

    constexpr explicit operator bool () const noexcept {
        return !operator_eq(*this, 0);
    }

    // ^
    template<typename T, class = __keep_size<T>>
    constexpr static wide_int<Bytes,Signed> operator_circumflex(const wide_int<Bytes,Signed>& num,
                                                            const T& other) noexcept {

        wide_int<Bytes,Signed> t(other);
        wide_int<Bytes,Signed> res = num;

        for (int i = 0; i < arr_size(); ++i) {
            res.m_arr[i] ^= t.m_arr[i];
        }

        return res;
    }

    template<size_t Bytes2, bool Signed2, class = __need_increase_size<Bytes2, Signed2>>
    constexpr static wide_int<Bytes2, Signed2> operator_circumflex(const wide_int<Bytes,Signed>& num,
                                                               const wide_int<Bytes2,Signed2>& other) noexcept {
        return wide_int<Bytes2,Signed2>::operator_circumflex(wide_int<Bytes2,Signed2>(num), other);
    }

    constexpr static wide_int<Bytes, Signed> from_str(const char* c) noexcept {
        wide_int<Bytes,Signed> res = 0;

        bool is_neg = Signed && *c == '-';
        if (is_neg) {
            ++c;
        }

        if (*c == '0' && *(c+1) == 'x') { // hex
            ++c;
            ++c;
            while (*c) {
                if (*c >= '0' && *c <= '9') {
                    res = operator_star(res, 16U);
                    res = operator_plus_T(res, *c - '0');
                    ++c;
                } else if (*c >= 'a' && *c <= 'f') {
                    res = operator_star(res, 16U);
                    res = operator_plus_T(res, *c - 'a' + 10U);
                    ++c;
                } else if (*c >= 'A' && *c <= 'F') { // tolower must be used, but it is not constexpr
                    res = operator_star(res, 16U);
                    res = operator_plus_T(res, *c - 'A' + 10U);
                    ++c;
                } else {
                    throw std::runtime_error("invalid char from");
                }
            }
        } else { // dec
            while (*c) {
                if (*c < '0' || *c > '9') {
                    throw std::runtime_error("invalid char from");
                }
                res = operator_star(res, 10U);
                res = operator_plus_T(res, *c - '0');
                ++c;
            }
        }

        if (is_neg) {
            res = operator_unary_minus(res);
        }

        return res;
    }

    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator*=(const T& other) noexcept {
        *this = operator_star(*this, wide_int<Bytes,Signed>(other));
        return *this;
    }

    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator/=(const T& other) noexcept {
        *this = operator_slash(*this, wide_int<Bytes,Signed>(other));
        return *this;
    }

    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator%=(const T& other) noexcept {
        *this = operator_percent(*this, wide_int<Bytes,Signed>(other));
        return *this;
    }

    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator+=(const T& other) noexcept {
        *this = operator_plus(*this, wide_int<Bytes,Signed>(other));
        return *this;
    }

    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator-=(const T& other) noexcept {
        *this = operator_minus(*this, wide_int<Bytes,Signed>(other));
        return *this;
    }

    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator&=(const T& other) noexcept {
        *this = operator_amp(*this, wide_int<Bytes,Signed>(other));
        return *this;
    }

    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator|=(const T& other) noexcept {
        *this = operator_pipe(*this, wide_int<Bytes,Signed>(other));
        return *this;
    }

    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator^=(const T& other) noexcept {
        *this = operator_circumflex(*this, wide_int<Bytes,Signed>(other));
        return *this;
    }

    constexpr wide_int<Bytes,Signed>& operator<<=(int n) noexcept {
        *this = shift_left(*this, n);
        return *this;
    }

    constexpr wide_int<Bytes,Signed>& operator>>=(int n) noexcept {
        *this = shift_right(*this, n);
        return *this;
    }

    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator!=(const T& other) noexcept {
        *this = !other;
        return *this;
    }

//private:
    base_type m_arr[arr_size()];
};

namespace std {
template<size_t Bytes, bool Signed>
std::string to_string(const wide_int<Bytes,Signed>& n) {
    std::string res;
    if (wide_int<Bytes, Signed>::operator_eq(n, 0U)) {
        return "0";
    }

    wide_int<Bytes, false> t;
    bool is_neg = wide_int<Bytes, Signed>::is_negative(n);
    if (is_neg) {
        t = wide_int<Bytes, Signed>::operator_unary_minus(n);
    } else {
        t = n;
    }

    while (!wide_int<Bytes, false>::operator_eq(t, 0U)) {
        res.insert(res.begin(), '0' + char(wide_int<Bytes, false>::operator_percent(t, 10U)));
        t = wide_int<Bytes, false>::operator_slash(t, 10U);
    }

    if (is_neg) {
        res.insert(res.begin(), '-');
    }

    return res;
}

template<size_t Bytes, bool Signed>
std::ostream& operator<<(std::ostream& out, const wide_int<Bytes,Signed>& n) {
    out << to_string(n);
    return out;
}
} // namespace std

template <class T> struct __is_wide_int: std::false_type{};
template <size_t Bytes, bool Signed> struct __is_wide_int<wide_int<Bytes, Signed> >: std::true_type{};
template <class T>
using __arithm_not_wide_int = typename std::enable_if< std::is_arithmetic<T>::value && !__is_wide_int<T>::value, T&>::type;


// Unary operators
template<size_t Bytes, bool Signed>
constexpr wide_int<Bytes,Signed> operator~(const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::operator_unary_tilda(num);
}

template<size_t Bytes, bool Signed>
constexpr wide_int<Bytes,Signed> operator-(const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::operator_unary_minus(num);
}

template<size_t Bytes, bool Signed>
constexpr wide_int<Bytes,Signed> operator+(const wide_int<Bytes,Signed>& num) noexcept {
    return num;
}


// Binary operators
template<size_t Bytes, bool Signed, typename T>
constexpr auto operator*(const wide_int<Bytes,Signed>& num,
                         const T& other) noexcept {
    return wide_int<Bytes,Signed>::operator_star(num, other);
}
template<size_t Bytes, bool Signed, typename T, class = __arithm_not_wide_int<T>>
constexpr wide_int<Bytes,Signed> operator*(const T& other,
                                       const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::operator_star(num, other);
}

template<size_t Bytes, bool Signed, typename T>
constexpr auto operator/(const wide_int<Bytes,Signed>& num, const T& other) noexcept {
    return wide_int<Bytes,Signed>::operator_slash(num, other);
}
template<size_t Bytes, bool Signed, typename T, class = __arithm_not_wide_int<T>>
constexpr wide_int<Bytes,Signed> operator/(const T& other,
                                       const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::operator_slash(wide_int<Bytes,Signed>(other), num);
}

template<size_t Bytes, bool Signed, typename T>
constexpr auto operator%(const wide_int<Bytes,Signed>& num, const T& other) noexcept {
    return wide_int<Bytes,Signed>::operator_percent(num, other);
}
template<size_t Bytes, bool Signed, typename T, class = __arithm_not_wide_int<T>>
constexpr wide_int<Bytes,Signed> operator%(const T& other,
                                       const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::operator_percent(wide_int<Bytes,Signed>(other), num);
}

template<size_t Bytes, bool Signed, typename T>
constexpr auto operator+(const wide_int<Bytes,Signed>& num, const T& other) noexcept {
    return wide_int<Bytes,Signed>::operator_plus(num, other);
}
template<size_t Bytes, bool Signed, typename T, class = __arithm_not_wide_int<T>>
constexpr wide_int<Bytes,Signed> operator+(const T& other,
                                       const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::operator_plus(wide_int<Bytes,Signed>(other), num);
}

template<size_t Bytes, bool Signed, typename T>
constexpr auto operator-(const wide_int<Bytes,Signed>& num, const T& other) noexcept {
    return wide_int<Bytes,Signed>::operator_minus(num, other);
}
template<size_t Bytes, bool Signed, typename T, class = __arithm_not_wide_int<T>>
constexpr wide_int<Bytes,Signed> operator-(const T& other,
                                       const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::operator_minus(wide_int<Bytes,Signed>(other), num);
}

template<size_t Bytes, bool Signed, typename T>
constexpr bool operator<(const wide_int<Bytes,Signed>& num,
                         const T& other) noexcept {
    return wide_int<Bytes,Signed>::operator_less(num, other);
}
template<size_t Bytes, bool Signed, typename T, class = __arithm_not_wide_int<T>>
constexpr bool operator<(const T& other,
                         const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::operator_less(wide_int<Bytes,Signed>(other), num);
}

template<size_t Bytes, bool Signed, typename T>
constexpr bool operator>(const wide_int<Bytes,Signed>& num,
                         const T& other) noexcept {
    return wide_int<Bytes,Signed>::operator_more(num, other);
}
template<size_t Bytes, bool Signed, typename T, class = __arithm_not_wide_int<T>>
constexpr bool operator>(const T& other,
                         const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::operator_more(wide_int<Bytes,Signed>(other), num);
}

template<size_t Bytes, bool Signed, typename T>
constexpr bool operator<=(const wide_int<Bytes,Signed>& num,
                          const T& other) noexcept {
    return wide_int<Bytes,Signed>::operator_less(num, other) ||
           wide_int<Bytes,Signed>::operator_eq(num, other);
}
template<size_t Bytes, bool Signed, typename T, class = __arithm_not_wide_int<T>>
constexpr bool operator<=(const T& other,
                          const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::operator_less(wide_int<Bytes,Signed>(other), num) ||
           wide_int<Bytes,Signed>::operator_eq(num, other);
}

template<size_t Bytes, bool Signed, typename T>
constexpr bool operator>=(const wide_int<Bytes,Signed>& num,
                          const T& other) noexcept {
    return wide_int<Bytes,Signed>::operator_more(num, other) ||
           wide_int<Bytes,Signed>::operator_eq(num, other);
}
template<size_t Bytes, bool Signed, typename T, class = __arithm_not_wide_int<T>>
constexpr bool operator>=(const T& other,
                          const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::operator_more(wide_int<Bytes,Signed>(other), num) ||
           wide_int<Bytes,Signed>::operator_eq(num, other);
}

template<size_t Bytes, bool Signed, typename T>
constexpr bool operator==(const wide_int<Bytes,Signed>& num,
                          const T& other) noexcept {
    return wide_int<Bytes,Signed>::operator_eq(num, other);
}
template<size_t Bytes, bool Signed, typename T, class = __arithm_not_wide_int<T>>
constexpr bool operator==(const T& other,
                          const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::operator_eq(num, other);
}

template<size_t Bytes, bool Signed, typename T>
constexpr auto operator&(const wide_int<Bytes,Signed>& num, const T& other) noexcept {
    return wide_int<Bytes,Signed>::operator_amp(num, other);
}
template<size_t Bytes, bool Signed, typename T, class = __arithm_not_wide_int<T>>
constexpr wide_int<Bytes,Signed> operator&(const T& other,
                                       const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::operator_amp(wide_int<Bytes,Signed>(other), num);
}

template<size_t Bytes, bool Signed, typename T>
constexpr auto operator|(const wide_int<Bytes,Signed>& num, const T& other) noexcept {
    return wide_int<Bytes,Signed>::operator_pipe(num, other);
}
template<size_t Bytes, bool Signed, typename T, class = __arithm_not_wide_int<T>>
constexpr wide_int<Bytes,Signed> operator|(const T& other,
                                       const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::operator_pipe(wide_int<Bytes,Signed>(other), num);
}

template<size_t Bytes, bool Signed, typename T>
constexpr auto operator^(const wide_int<Bytes,Signed>& num, const T& other) noexcept {
    return wide_int<Bytes,Signed>::operator_circumflex(num, other);
}
template<size_t Bytes, bool Signed, typename T, class = __arithm_not_wide_int<T>>
constexpr wide_int<Bytes,Signed> operator^(const T& other,
                                       const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::operator_circumflex(wide_int<Bytes,Signed>(other), num);
}

template<size_t Bytes, bool Signed>
constexpr wide_int<Bytes,Signed> operator<<(const wide_int<Bytes,Signed>& num, int n) noexcept {
    return wide_int<Bytes,Signed>::shift_left(num, n);
}
template<size_t Bytes, bool Signed>
constexpr wide_int<Bytes,Signed> operator>>(const wide_int<Bytes,Signed>& num, int n) noexcept {
    return wide_int<Bytes,Signed>::shift_right(num, n);
}


using int128_t = wide_int<16,true>;
using uint128_t = wide_int<16,false>;

using int256_t = wide_int<32,true>;
using uint256_t = wide_int<32,false>;

using int512_t = wide_int<64,true>;
using uint512_t = wide_int<64,false>;

constexpr int128_t operator "" _int128(const char* n) noexcept { return int128_t::from_str(n); }
constexpr int256_t operator "" _int256(const char* n) noexcept { return int256_t::from_str(n); }
constexpr int512_t operator "" _int512(const char* n) noexcept { return int512_t::from_str(n); }
constexpr uint128_t operator "" _uint128(const char* n) noexcept { return uint128_t::from_str(n); }
constexpr uint256_t operator "" _uint256(const char* n) noexcept { return uint256_t::from_str(n); }
constexpr uint512_t operator "" _uint512(const char* n) noexcept { return uint512_t::from_str(n); }

namespace std {

// numeric limits
template<size_t Bytes, bool Signed>
struct numeric_limits<wide_int<Bytes,Signed>> {
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = Signed;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = true;
    static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
    static constexpr bool has_denorm_loss = false;
    static constexpr std::float_round_style round_style = std::round_toward_zero;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = true;
    static constexpr int digits = CHAR_BIT*Bytes - (Signed ? 1 : 0);
    static constexpr int digits10 = digits * 38.2308 /*std::log10(2)*/;
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;
    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    static constexpr bool traps = true;
    static constexpr bool tinyness_before = false;

    static constexpr wide_int<Bytes,Signed> min() noexcept {
        if (Signed) {
            wide_int<Bytes,true> res {};
            res.m_arr[0] = std::numeric_limits<typename wide_int<Bytes,Signed>::signed_base_type>::min();
            return res;
        } else {
            return 0;
        }
    }

    static constexpr wide_int<Bytes,Signed> lowest() noexcept {
        return min();
    }

    static constexpr wide_int<Bytes,Signed> max() noexcept {
        wide_int<Bytes,Signed> res {};
        res.m_arr[0] = Signed ? std::numeric_limits<typename wide_int<Bytes,Signed>::signed_base_type>::max()
                           : std::numeric_limits<typename wide_int<Bytes,Signed>::base_type>::max();
        for (int i = 1; i < res.arr_size(); ++i) {
            res.m_arr[i] = std::numeric_limits<typename wide_int<Bytes,Signed>::base_type>::max();
        }
        return res;
    }

    static constexpr wide_int<Bytes,Signed> epsilon() noexcept {
        return 0;
    }

    static constexpr wide_int<Bytes,Signed> round_error() noexcept {
        return 0;
    }

    static constexpr wide_int<Bytes,Signed> infinity() noexcept {
        return 0;
    }

    static constexpr wide_int<Bytes,Signed> quiet_NaN() noexcept {
        return 0;
    }

    static constexpr wide_int<Bytes,Signed> signaling_NaN() noexcept {
        return 0;
    }

    static constexpr wide_int<Bytes,Signed> denorm_min() noexcept {
        return 0;
    }
};

} // namespace std
