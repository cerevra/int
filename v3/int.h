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


struct _test;

namespace std {
template<size_t Bytes, bool Signed>
class wide_int;


template<size_t Bytes, bool Signed, size_t Bytes2, bool Signed2>
struct common_type<wide_int<Bytes, Signed>, wide_int<Bytes2, Signed2>>;

template<size_t Bytes, bool Signed, typename Arithmetic>
struct common_type<wide_int<Bytes, Signed>, Arithmetic>;


template<size_t Bytes, bool Signed>
class wide_int {
public:
    using base_type = uint8_t;
    using signed_base_type = int8_t;

    // ctors
    wide_int() = default;

    template<typename T> constexpr wide_int(T other) noexcept;
    template<size_t Bytes2, bool Signed2> constexpr wide_int(const wide_int<Bytes2,Signed2>& other) noexcept;

    // assignment
    template<size_t Bytes2, bool Signed2>
    constexpr wide_int<Bytes,Signed>& operator=(const wide_int<Bytes2,Signed2>& other) noexcept;

    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator=(T other) noexcept;


    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator*=(const T& other);

    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator/=(const T& other);

    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator+=(const T& other) noexcept(!Signed);

    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator-=(const T& other) noexcept(!Signed);

    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator%=(const T& other);

    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator&=(const T& other) noexcept;

    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator|=(const T& other) noexcept;

    template<typename T>
    constexpr wide_int<Bytes,Signed>& operator^=(const T& other) noexcept;

    constexpr wide_int<Bytes,Signed>& operator<<=(int n);
    constexpr wide_int<Bytes,Signed>& operator>>=(int n) noexcept;

    // observers

    constexpr explicit operator bool () const noexcept;

    template <class T>
    using __integral_not_wide_int_class = typename std::enable_if< std::is_integral<T>::value, T>::type;

    template <class T, class = __integral_not_wide_int_class<T> >
    constexpr operator T () const noexcept;

    constexpr operator long double () const noexcept;
    constexpr operator double () const noexcept;
    constexpr operator float () const noexcept;

    struct _impl;

private:
    friend struct ::_test;

    template<size_t Bytes2, bool Signed2>
    friend class wide_int;

    friend class numeric_limits<wide_int<Bytes, true>>;
    friend class numeric_limits<wide_int<Bytes, false>>;

    base_type m_arr[_impl::arr_size()];
};

// Unary operators
template<size_t Bytes, bool Signed>
constexpr wide_int<Bytes,Signed> operator~(const wide_int<Bytes,Signed>& num) noexcept;

template<size_t Bytes, bool Signed>
constexpr wide_int<Bytes,Signed> operator-(const wide_int<Bytes,Signed>& num) noexcept(!Signed);

template<size_t Bytes, bool Signed>
constexpr wide_int<Bytes,Signed> operator+(const wide_int<Bytes,Signed>& num) noexcept(!Signed);


// Binary operators
template<size_t Bytes, bool Signed, typename T>
std::common_type_t<wide_int<Bytes,Signed>, T>
constexpr operator*(const wide_int<Bytes,Signed>& num, const T& other);
template<size_t Bytes, bool Signed, typename Arithmetic, class>
std::common_type_t<wide_int<Bytes,Signed>, Arithmetic>
constexpr operator*(const Arithmetic& other, const wide_int<Bytes,Signed>& num);

template<size_t Bytes, bool Signed, typename T>
std::common_type_t<wide_int<Bytes,Signed>, T>
constexpr operator/(const wide_int<Bytes,Signed>& num, const T& other);
template<size_t Bytes, bool Signed, typename Arithmetic, class>
std::common_type_t<wide_int<Bytes,Signed>, Arithmetic>
constexpr operator/(const Arithmetic& other, const wide_int<Bytes,Signed>& num);

template<size_t Bytes, bool Signed, typename T>
std::common_type_t<wide_int<Bytes,Signed>, T>
constexpr operator+(const wide_int<Bytes,Signed>& num, const T& other) noexcept(!Signed);
template<size_t Bytes, bool Signed, typename Arithmetic, class>
std::common_type_t<wide_int<Bytes,Signed>, Arithmetic>
constexpr operator+(const Arithmetic& other, const wide_int<Bytes,Signed>& num) noexcept(!Signed);

template<size_t Bytes, bool Signed, typename T>
std::common_type_t<wide_int<Bytes,Signed>, T>
constexpr operator-(const wide_int<Bytes,Signed>& num, const T& other) noexcept(!Signed);
template<size_t Bytes, bool Signed, typename Arithmetic, class>
std::common_type_t<wide_int<Bytes,Signed>, Arithmetic>
constexpr operator-(const Arithmetic& other, const wide_int<Bytes,Signed>& num) noexcept(!Signed);

template<size_t Bytes, bool Signed, typename T>
std::common_type_t<wide_int<Bytes,Signed>, T>
constexpr operator%(const wide_int<Bytes,Signed>& num, const T& other);
template<size_t Bytes, bool Signed, typename Integral, class>
std::common_type_t<wide_int<Bytes,Signed>, Integral>
constexpr operator%(const Integral& other, const wide_int<Bytes,Signed>& num);

template<size_t Bytes, bool Signed, typename T>
std::common_type_t<wide_int<Bytes,Signed>, T>
constexpr operator&(const wide_int<Bytes,Signed>& num, const T& other) noexcept;
template<size_t Bytes, bool Signed, typename Integral, class>
std::common_type_t<wide_int<Bytes,Signed>, Integral>
constexpr operator&(const Integral& other, const wide_int<Bytes,Signed>& num) noexcept;

template<size_t Bytes, bool Signed, typename T>
std::common_type_t<wide_int<Bytes,Signed>, T>
constexpr operator|(const wide_int<Bytes,Signed>& num, const T& other) noexcept;
template<size_t Bytes, bool Signed, typename Integral, class>
std::common_type_t<wide_int<Bytes,Signed>, Integral>
constexpr operator|(const Integral& other, const wide_int<Bytes,Signed>& num) noexcept;

template<size_t Bytes, bool Signed, typename T>
std::common_type_t<wide_int<Bytes,Signed>, T>
constexpr operator^(const wide_int<Bytes,Signed>& num, const T& other) noexcept;
template<size_t Bytes, bool Signed, typename Integral, class>
std::common_type_t<wide_int<Bytes,Signed>, Integral>
constexpr operator^(const Integral& other, const wide_int<Bytes,Signed>& num) noexcept;

template<size_t Bytes, bool Signed>
constexpr wide_int<Bytes,Signed> operator<<(const wide_int<Bytes,Signed>& num, int n) noexcept;
template<size_t Bytes, bool Signed>
constexpr wide_int<Bytes,Signed> operator>>(const wide_int<Bytes,Signed>& num, int n) noexcept;

template<size_t Bytes, bool Signed, typename T>
constexpr bool operator<(const wide_int<Bytes,Signed>& num, const T& other) noexcept;
template<size_t Bytes, bool Signed, typename Arithmetic, class>
constexpr bool operator<(const Arithmetic& other, const wide_int<Bytes,Signed>& num) noexcept;

template<size_t Bytes, bool Signed, typename T>
constexpr bool operator>(const wide_int<Bytes,Signed>& num, const T& other) noexcept;
template<size_t Bytes, bool Signed, typename Arithmetic, class>
constexpr bool operator>(const Arithmetic& other, const wide_int<Bytes,Signed>& num) noexcept;

template<size_t Bytes, bool Signed, typename T>
constexpr bool operator<=(const wide_int<Bytes,Signed>& num, const T& other) noexcept;
template<size_t Bytes, bool Signed, typename Arithmetic, class>
constexpr bool operator<=(const Arithmetic& other, const wide_int<Bytes,Signed>& num) noexcept;

template<size_t Bytes, bool Signed, typename T>
constexpr bool operator>=(const wide_int<Bytes,Signed>& num, const T& other) noexcept;
template<size_t Bytes, bool Signed, typename Arithmetic, class>
constexpr bool operator>=(const Arithmetic& other, const wide_int<Bytes,Signed>& num) noexcept;

template<size_t Bytes, bool Signed, typename T>
constexpr bool operator==(const wide_int<Bytes,Signed>& num, const T& other) noexcept;
template<size_t Bytes, bool Signed, typename Arithmetic, class>
constexpr bool operator==(const Arithmetic& other, const wide_int<Bytes,Signed>& num) noexcept;

template<size_t Bytes, bool Signed, typename T>
constexpr bool operator!=(const wide_int<Bytes,Signed>& num, const T& other) noexcept;
template<size_t Bytes, bool Signed, typename Arithmetic, class>
constexpr bool operator!=(const Arithmetic& other, const wide_int<Bytes,Signed>& num) noexcept;


template<size_t Bytes, bool Signed>
std::string to_string(const wide_int<Bytes,Signed>& n);

template<size_t Bytes, bool Signed>
std::wstring to_wstring(const wide_int<Bytes,Signed>& n);

template<size_t Bytes, bool Signed>
std::ostream& operator<<(std::ostream& out, const wide_int<Bytes,Signed>& n);

template<size_t Bytes, bool Signed>
std::wostream& operator<<(std::wostream& out, const wide_int<Bytes,Signed>& n);

template<size_t Bytes, bool Signed>
std::istream& operator>>(std::istream& in, wide_int<Bytes,Signed>& n);

template<size_t Bytes, bool Signed>
std::wistream& operator>>(std::wistream& in, wide_int<Bytes,Signed>& n);

inline namespace literals {
inline namespace wide_int_literals {

using int128_t = wide_int<16,true>;
using uint128_t = wide_int<16,false>;

using int256_t = wide_int<32,true>;
using uint256_t = wide_int<32,false>;

using int512_t = wide_int<64,true>;
using uint512_t = wide_int<64,false>;

} // namespace wide_int_literals
} // namespace literals

constexpr int128_t operator "" _int128(const char* n);
constexpr int256_t operator "" _int256(const char* n);
constexpr int512_t operator "" _int512(const char* n);
constexpr uint128_t operator "" _uint128(const char* n);
constexpr uint256_t operator "" _uint256(const char* n);
constexpr uint512_t operator "" _uint512(const char* n);

// numeric limits
template<size_t Bytes, bool Signed>
class numeric_limits<wide_int<Bytes,Signed>>;


template<size_t Bytes, bool Signed>
struct hash<wide_int<Bytes, Signed>>;

} // namespace std

#include "impl.h"
