///////////////////////////////////////////////////////////////
//  Distributed under the Boost Software License, Version 1.0.
//  (See at http://www.boost.org/LICENSE_1_0.txt)
///////////////////////////////////////////////////////////////

#include <boost/multiprecision/cpp_int.hpp> // for implementations

#include <type_traits>

namespace std {

// Forward declaration
template <size_t Bits, bool IsSigned = true> struct integer;

// detail:
template <class T> struct __is_integer_class: false_type{};
template <size_t Bits, bool IsSigned> struct __is_integer_class<integer<Bits, IsSigned> >: true_type{};



template <size_t Bits, bool IsSigned> struct is_arithmetic<integer<Bits, IsSigned>>: true_type{};
template <size_t Bits, bool IsSigned> struct is_integral<integer<Bits, IsSigned>>: true_type{};
template <size_t Bits, bool IsSigned> struct is_arithmetic<const integer<Bits, IsSigned>>: true_type{};
template <size_t Bits, bool IsSigned> struct is_integral<const integer<Bits, IsSigned>>: true_type{};
template <size_t Bits, bool IsSigned> struct is_arithmetic<const volatile integer<Bits, IsSigned>>: true_type{};
template <size_t Bits, bool IsSigned> struct is_integral<const volatile integer<Bits, IsSigned>>: true_type{};
template <size_t Bits, bool IsSigned> struct is_arithmetic<volatile integer<Bits, IsSigned>>: true_type{};
template <size_t Bits, bool IsSigned> struct is_integral<volatile integer<Bits, IsSigned>>: true_type{};


template <size_t Bits, bool IsSigned>
struct integer {
    // detail:
    boost::multiprecision::number<
        boost::multiprecision::cpp_int_backend<
            Bits, Bits, IsSigned ? boost::multiprecision::signed_magnitude : boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void
        >
    > __impl;


    template <class T>
    using __arithm_lref = typename enable_if< is_arithmetic<T>::value, integer<Bits, IsSigned>& >::type;
    template <class T>
    using __arithm_not_int_class = typename enable_if< is_arithmetic<T>::value && !__is_integer_class<T>::value, T&>::type;

    // public:
    constexpr integer() noexcept = default;
    constexpr integer(const integer&) noexcept = default;
    constexpr integer& operator=(const integer&) noexcept = default;


    template <class T, class = __arithm_not_int_class<T> > constexpr integer(const T& n) noexcept : __impl(n) {}
    template <size_t Bits1, bool IsSigned1> constexpr integer(const integer<Bits1, IsSigned1>& n) noexcept : __impl(n.__impl) {}
    template <class T> constexpr __arithm_lref<T> operator=(const T& n) noexcept { __impl = n; return *this; }

    // Member operators
    template <class T> constexpr __arithm_lref<T> operator+=(const T& n) noexcept { __impl += n; return *this; }
    template <class T> constexpr __arithm_lref<T> operator-=(const T& n) noexcept { __impl -= n; return *this; }
    template <class T> constexpr __arithm_lref<T> operator*=(const T& n) noexcept { __impl *= n; return *this; }
    template <class T> constexpr __arithm_lref<T> operator/=(const T& n) noexcept { __impl /= n; return *this; }

    constexpr integer& operator++() noexcept { ++__impl; return *this; }
    constexpr integer& operator--() noexcept { --__impl; return *this; }
    constexpr integer  operator++(int) noexcept { integer cpy(*this); ++(*this); return cpy; }
    constexpr integer  operator--(int) noexcept { integer cpy(*this); --(*this); return cpy; }

    template <class T> constexpr __arithm_lref<T> operator%=(const T& n) noexcept { __impl %= n; return *this; }
    template <class T> constexpr __arithm_lref<T> operator&=(const T& n) noexcept { __impl &= n; return *this; }
    template <class T> constexpr __arithm_lref<T> operator|=(const T& n) noexcept { __impl |= n; return *this; }
    template <class T> constexpr __arithm_lref<T> operator^=(const T& n) noexcept { __impl ^= n; return *this; }
    template <class T> constexpr __arithm_lref<T> operator<<=(const T& n) noexcept { __impl <<= n; return *this; }
    template <class T> constexpr __arithm_lref<T> operator>>=(const T& n) noexcept { __impl >>= n; return *this; }

    template <class T, class = __arithm_not_int_class<T> >
    constexpr operator T () const noexcept {
        return static_cast<T>(__impl );
    }
};

/// unary operators
template <size_t Bits, bool IsSigned>
constexpr integer<Bits, IsSigned> operator+(const integer<Bits, IsSigned>& rhs) noexcept {
    return rhs;
}

template <size_t Bits, bool IsSigned>
constexpr integer<Bits, IsSigned> operator-(integer<Bits, IsSigned> rhs) noexcept {
    rhs *= -1;
    return rhs;
}


template <class NativeNum, size_t Bits, bool IsSigned>
using __promote_type = conditional_t <
    is_floating_point<NativeNum>::value,
    NativeNum,
    conditional_t<
        (sizeof(NativeNum) < sizeof(int) || Bits / 8 < sizeof(int)),
        int,
        conditional_t< // integral conversions
            is_signed<NativeNum>::value == IsSigned,
            conditional_t<(sizeof(NativeNum) < Bits / 8), integer<Bits, IsSigned>, NativeNum>,
            conditional_t<(sizeof(NativeNum) < Bits / 8), integer<Bits, IsSigned>, NativeNum> // TODO:
        >
    >
>;


template <class T, class Integer>
constexpr auto __to_long_double_if_required(const Integer& n) noexcept {
    return static_cast<
        conditional_t<is_floating_point<T>::value, long double, Integer>
    >(n);
}


template <class T>
using __arithm_bool = typename enable_if< is_arithmetic<T>::value && !__is_integer_class<T>::value, bool>::type;

/// binary operators


template <class T, size_t Bits, bool IsSigned, class = __arithm_bool<T> >
constexpr auto operator+(const T& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return __promote_type<T, Bits, IsSigned>{lhs + __to_long_double_if_required<T>(rhs.__impl)}; }

template <class T, size_t Bits, bool IsSigned, class = __arithm_bool<T> >
constexpr auto operator-(const T& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return __promote_type<T, Bits, IsSigned>{lhs - __to_long_double_if_required<T>(rhs.__impl)}; }

template <class T, size_t Bits, bool IsSigned, class = __arithm_bool<T> >
constexpr auto operator*(const T& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return __promote_type<T, Bits, IsSigned>{lhs * __to_long_double_if_required<T>(rhs.__impl)}; }

template <class T, size_t Bits, bool IsSigned, class = __arithm_bool<T> >
constexpr auto operator/(const T& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return __promote_type<T, Bits, IsSigned>{lhs / __to_long_double_if_required<T>(rhs.__impl)}; }

///

template <size_t Bits, bool IsSigned, class T, class = __arithm_bool<T> >
constexpr auto operator+(const integer<Bits, IsSigned>& lhs, const T& rhs) noexcept { return __promote_type<T, Bits, IsSigned>{__to_long_double_if_required<T>(lhs.__impl) + rhs}; }

template <size_t Bits, bool IsSigned, class T, class = __arithm_bool<T> >
constexpr auto operator-(const integer<Bits, IsSigned>& lhs, const T& rhs) noexcept { return __promote_type<T, Bits, IsSigned>{__to_long_double_if_required<T>(lhs.__impl) - rhs}; }

template <size_t Bits, bool IsSigned, class T, class = __arithm_bool<T> >
constexpr auto operator*(const integer<Bits, IsSigned>& lhs, const T& rhs) noexcept { return __promote_type<T, Bits, IsSigned>{__to_long_double_if_required<T>(lhs.__impl) * rhs}; }

template <size_t Bits, bool IsSigned, class T, class = __arithm_bool<T> >
constexpr auto operator/(const integer<Bits, IsSigned>& lhs, const T& rhs) noexcept { return __promote_type<T, Bits, IsSigned>{__to_long_double_if_required<T>(lhs.__impl) / rhs}; }

/*

// Non member operators: TODO:
unmentionable-expression-template-type operator+(const see-below&, const see-below&);
unmentionable-expression-template-type operator-(const see-below&, const see-below&);
unmentionable-expression-template-type operator*(const see-below&, const see-below&);
unmentionable-expression-template-type operator/(const see-below&, const see-below&);

// Integer only operations:
unmentionable-expression-template-type operator%(const see-below&, const see-below&);
unmentionable-expression-template-type operator&(const see-below&, const see-below&);
unmentionable-expression-template-type operator|(const see-below&, const see-below&);
unmentionable-expression-template-type operator^(const see-below&, const see-below&);

*/

/// comparisons
template <class T, size_t Bits, bool IsSigned>
constexpr __arithm_bool<T> operator==(const T& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return lhs == __to_long_double_if_required<T>(rhs.__impl); }

template <class T, size_t Bits, bool IsSigned>
constexpr __arithm_bool<T> operator!=(const T& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return lhs != __to_long_double_if_required<T>(rhs.__impl); }

template <class T, size_t Bits, bool IsSigned>
constexpr __arithm_bool<T> operator< (const T& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return lhs < __to_long_double_if_required<T>(rhs.__impl); }

template <class T, size_t Bits, bool IsSigned>
constexpr __arithm_bool<T> operator> (const T& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return lhs > __to_long_double_if_required<T>(rhs.__impl); }

template <class T, size_t Bits, bool IsSigned>
constexpr __arithm_bool<T> operator<=(const T& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return lhs <= __to_long_double_if_required<T>(rhs.__impl); }

template <class T, size_t Bits, bool IsSigned>
constexpr __arithm_bool<T> operator>=(const T& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return lhs >= __to_long_double_if_required<T>(rhs.__impl); }


template <size_t Bits, bool IsSigned, class T>
constexpr __arithm_bool<T> operator==(const integer<Bits, IsSigned>& lhs, const T& rhs) noexcept { return __to_long_double_if_required<T>(lhs.__impl) == rhs; }

template <size_t Bits, bool IsSigned, class T>
constexpr __arithm_bool<T> operator!=(const integer<Bits, IsSigned>& lhs, const T& rhs) noexcept { return __to_long_double_if_required<T>(lhs.__impl) != rhs; }

template <size_t Bits, bool IsSigned, class T>
constexpr __arithm_bool<T> operator< (const integer<Bits, IsSigned>& lhs, const T& rhs) noexcept { return __to_long_double_if_required<T>(lhs.__impl) < rhs; }

template <size_t Bits, bool IsSigned, class T>
constexpr __arithm_bool<T> operator> (const integer<Bits, IsSigned>& lhs, const T& rhs) noexcept { return __to_long_double_if_required<T>(lhs.__impl) > rhs; }

template <size_t Bits, bool IsSigned, class T>
constexpr __arithm_bool<T> operator<=(const integer<Bits, IsSigned>& lhs, const T& rhs) noexcept { return __to_long_double_if_required<T>(lhs.__impl) <= rhs; }

template <size_t Bits, bool IsSigned, class T>
constexpr __arithm_bool<T> operator>=(const integer<Bits, IsSigned>& lhs, const T& rhs) noexcept { return __to_long_double_if_required<T>(lhs.__impl) >= rhs; }

///

template <size_t Bits1, bool IsSigned1, size_t Bits2, bool IsSigned2>
constexpr bool operator==(const integer<Bits1, IsSigned1>& lhs, const integer<Bits2, IsSigned2>& rhs) noexcept { return lhs.__impl == rhs.__impl; }

template <size_t Bits1, bool IsSigned1, size_t Bits2, bool IsSigned2>
constexpr bool operator!=(const integer<Bits1, IsSigned1>& lhs, const integer<Bits2, IsSigned2>& rhs) noexcept { return lhs.__impl != rhs.__impl; }


template <size_t Bits1, bool IsSigned1, size_t Bits2, bool IsSigned2>
constexpr bool operator< (const integer<Bits1, IsSigned1>& lhs, const integer<Bits2, IsSigned2>& rhs) noexcept { return lhs.__impl < rhs.__impl; }

template <size_t Bits1, bool IsSigned1, size_t Bits2, bool IsSigned2>
constexpr bool operator> (const integer<Bits1, IsSigned1>& lhs, const integer<Bits2, IsSigned2>& rhs) noexcept { return lhs.__impl > rhs.__impl; }

template <size_t Bits1, bool IsSigned1, size_t Bits2, bool IsSigned2>
constexpr bool operator<=(const integer<Bits1, IsSigned1>& lhs, const integer<Bits2, IsSigned2>& rhs) noexcept { return lhs.__impl <= rhs.__impl; }

template <size_t Bits1, bool IsSigned1, size_t Bits2, bool IsSigned2>
constexpr bool operator>=( const integer<Bits1, IsSigned1>& lhs, const integer<Bits2, IsSigned2>& rhs) noexcept { return lhs.__impl >= rhs.__impl; }



// iostream support:
template <class Char, class Traits, size_t Bits, bool IsSigned>
std::basic_ostream<Char, Traits>& operator << (std::basic_ostream<Char, Traits>& os, const integer<Bits, IsSigned>& n) { return os << n.__impl; }


template <class Char, class Traits, size_t Bits, bool IsSigned>
std::basic_istream<Char, Traits>& operator >> (std::basic_istream<Char, Traits>& is, integer<Bits, IsSigned>& n) { return is >> n; }

using int128_t = integer<128>;
using int256_t = integer<256>;
using int512_t = integer<512>;

template <class To>
constexpr To __convert(const char* n) noexcept {
    To res {0};
    while (*n) {
        res *= 10;
        res += *n - '0';
        ++ n;
    }

    return res;
}

constexpr int128_t operator "" _int128(const char* n) noexcept { return __convert<int128_t>(n); }
constexpr int256_t operator "" _int256(const char* n) noexcept { return __convert<int256_t>(n); }
constexpr int512_t operator "" _int512(const char* n) noexcept { return __convert<int512_t>(n); }

} // namespace std

int main() {
    using namespace std;

    int256_t tmp = 1; (void)tmp;
    int256_t v = 1_int256;
    int256_t tmp2{}; tmp2 = tmp;
    for (unsigned i = 1; i <= 20; ++i) {
        v *= i;
    }

    std::cout << v << std::endl; // prints 20!


    int256_t v2 = 1_int512; (void)v2;
    static_assert(is_standard_layout<int256_t>::value, "");
    int i = 1_int512; (void)i;
    if (v2) {
        assert(true);
    } else {
        assert(false);
    }

    // TODO: cahnge to static_assert all the asserts:
    /// Comparisons
    assert(1_int512 == 1_int512);
    assert(1_int256 == 1_int512);
    assert(1_int512 == 1);
    assert(1_int512 <= 1);
    assert(1_int512 >= 1);
    assert(1 == 1_int512);
    assert(1 <= 1_int512);
    assert(1 >= 1_int512);
    assert(1_int512 >= 1_int512);
    assert(2_int512 > 1_int512);
    assert(1_int256 >= 1_int512);
    assert(1_int512 <= 1_int256);
    assert(0 == 0_int512);
    assert(0 <= 0_int512);
    assert(0 >= 0_int512);


    /// Comparisons with floating point numbers
    assert(0_int512 == 0.0);
    assert(0_int512 <= 0.0);
    assert(0_int512 >= 0.0);
    assert(0_int512 == 0.0f);
    assert(0_int512 <= 0.0f);
    assert(0_int512 >= 0.0f);


    /// Arithmetics on floating points
    assert(0.0 + 0_int512 == 0);      static_assert( std::is_same<decltype(0.0 + 0_int512), double>::value, "");
    assert(0.0 + 1_int512 == 1);      static_assert( std::is_same<decltype(0.0f + 0_int512), float>::value, "");
    assert(0.0 + 2_int512 == 2);

    assert(0.0 - 0_int512 == 0);      static_assert( std::is_same<decltype(0.0 - 0_int512), double>::value, "");
    assert(0.0 - 1_int512 == -1);     static_assert( std::is_same<decltype(0.0f - 0_int512), float>::value, "");
    assert(0.0 - 2_int512 == -2);

    assert(0.0 * 0_int512 == 0);      static_assert( std::is_same<decltype(0.0 * 0_int512), double>::value, "");
    assert(1.0 * 100_int512 == 100);  static_assert( std::is_same<decltype(0.0f * 0_int512), float>::value, "");
    assert(-1.0 * 2_int512 == -2);

    assert(0.0 / 1_int512 == 0);      static_assert( std::is_same<decltype(0.0 / 1_int512), double>::value, "");
    assert(1.0 / 100_int512 == 1.0 / 100);  static_assert( std::is_same<decltype(0.0f / 1_int512), float>::value, "");
    assert(-1.0 / 1_int512 == -1);


    assert(0_int512 + 0.0 == 0);      static_assert( std::is_same<decltype(0_int512 + 0.0), double>::value, "");
    assert(1_int512 + 0.0 == 1);      static_assert( std::is_same<decltype(0_int512 + 0.0f), float>::value, "");
    assert(2_int512 + 0.0 == 2);

    assert(0_int512 - 0.0 == 0);      static_assert( std::is_same<decltype(0_int512 - 0.0), double>::value, "");
    assert(4_int512 - 5.0 == -1);     static_assert( std::is_same<decltype(0_int512 - 0.0f), float>::value, "");
    assert(4_int512 - 6.0 == -2);

    assert(0_int512 * 0.0 == 0);      static_assert( std::is_same<decltype(0_int512 * 0.0), double>::value, "");
    assert(100_int512 * 1.0 == 100);  static_assert( std::is_same<decltype(0_int512 * 0.0f), float>::value, "");
    assert(2_int512 * -1.0 == -2);

    assert(0_int512 / 1.0 == 0);      static_assert( std::is_same<decltype(0_int512 / 1.0), double>::value, "");
    assert(1_int512 / 100.0 == 1 / 100.0); static_assert( std::is_same<decltype(0_int512 / 1.0f), float>::value, "");
    assert(-1_int512 / 1.0 == -1);


    /// Arithmetics on integers
    assert(-tmp == -1);
    assert(+tmp == 1);

    assert(0 + 0_int512 == 0);      static_assert( std::is_same<decltype(0 + 0_int512), int512_t>::value, "");
    assert(0 + 1_int512 == 1);
    assert(0 + 2_int512 == 2);

    assert(0 - 0_int512 == 0);      static_assert( std::is_same<decltype(0 - 0_int512), int512_t>::value, "");
    assert(0 - 1_int512 == -1);
    assert(0 - 2_int512 == -2);

    assert(0 * 0_int512 == 0);      static_assert( std::is_same<decltype(0 * 0_int512), int512_t>::value, "");
    assert(1 * 100_int512 == 100);
    assert(-1 * 2_int512 == -2);

    assert(0 / 1_int512 == 0);      static_assert( std::is_same<decltype(0 / 1_int512), int512_t>::value, "");
    assert(1 / 100_int512 == 0);
    assert(-1 / 1_int512 == -1);


    assert(0_int512 + 0 == 0);      static_assert( std::is_same<decltype(0_int512 + 0), int512_t>::value, "");
    assert(1_int512 + 0 == 1);
    assert(2_int512 + 0 == 2);

    assert(0_int512 - 0 == 0);      static_assert( std::is_same<decltype(0_int512 - 0), int512_t>::value, "");
    assert(4_int512 - 5 == -1);
    assert(4_int512 - 6 == -2);

    assert(0_int512 * 0 == 0);      static_assert( std::is_same<decltype(0_int512 * 0), int512_t>::value, "");
    assert(100_int512 * 1 == 100);
    assert(2_int512 * -1 == -2);

    assert(0_int512 / 1 == 0);      static_assert( std::is_same<decltype(0_int512 / 1), int512_t>::value, "");
    assert(1_int512 / 100 == 0);
    assert(-1_int512 / 1 == -1);


    // TODO:
    // signed with unsigned aruthmetic promotions and operators check
    // test signed with unsigned comparisons
    // fixing all the constexpr
    // solve some real use case in test
    // make_signed, make_unsigned
    // clenup detail
    // static_assert(1_int256 == 1_int512, "");
    // static_assert(is_pod<int256_t>::value, "");
    // integer<96> tests
    // What to do with integer<16> or integer<8>
}
