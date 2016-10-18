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

    constexpr integer(const char* n) noexcept : __impl()  { __impl.assign(n); }

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

template <size_t Bits, bool IsSigned>
constexpr integer<Bits, IsSigned> operator+(const integer<Bits, IsSigned>& rhs) noexcept {
    return rhs;
}

template <size_t Bits, bool IsSigned>
constexpr integer<Bits, IsSigned> operator-(integer<Bits, IsSigned> rhs) noexcept {
    rhs *= -1;
    return rhs;
}
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

// Comparison operators:
template <size_t Bits, bool IsSigned>
constexpr bool operator==(const integer<Bits, IsSigned>& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return lhs.__impl == rhs.__impl; }

template <size_t Bits, bool IsSigned>
constexpr bool operator!=(const integer<Bits, IsSigned>& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return lhs.__impl != rhs.__impl; }

template <size_t Bits, bool IsSigned>
constexpr bool operator< (const integer<Bits, IsSigned>& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return lhs.__impl < rhs.__impl; }

template <size_t Bits, bool IsSigned>
constexpr bool operator> (const integer<Bits, IsSigned>& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return lhs.__impl > rhs.__impl; }

template <size_t Bits, bool IsSigned>
constexpr bool operator<=(const integer<Bits, IsSigned>& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return lhs.__impl <= rhs.__impl; }

template <size_t Bits, bool IsSigned>
constexpr bool operator>=(const integer<Bits, IsSigned>& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return lhs.__impl >= rhs.__impl; }

///

template <class T>
using __arithm_bool = typename enable_if< is_arithmetic<T>::value && !__is_integer_class<T>::value, bool>::type;

template <class T, size_t Bits, bool IsSigned>
constexpr __arithm_bool<T> operator==(const T& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return lhs == rhs.__impl; }

template <class T, size_t Bits, bool IsSigned>
constexpr __arithm_bool<T> operator!=(const T& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return lhs != rhs.__impl; }

template <class T, size_t Bits, bool IsSigned>
constexpr __arithm_bool<T> operator< (const T& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return lhs < rhs.__impl; }

template <class T, size_t Bits, bool IsSigned>
constexpr __arithm_bool<T> operator> (const T& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return lhs > rhs.__impl; }

template <class T, size_t Bits, bool IsSigned>
constexpr __arithm_bool<T> operator<=(const T& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return lhs <= rhs.__impl; }

template <class T, size_t Bits, bool IsSigned>
constexpr __arithm_bool<T> operator>=(const T& lhs, const integer<Bits, IsSigned>& rhs) noexcept { return lhs >= rhs.__impl; }


template <size_t Bits, bool IsSigned, class T>
constexpr __arithm_bool<T> operator==(const integer<Bits, IsSigned>& lhs, const T& rhs) noexcept { return lhs.__impl == rhs; }

template <size_t Bits, bool IsSigned, class T>
constexpr __arithm_bool<T> operator!=(const integer<Bits, IsSigned>& lhs, const T& rhs) noexcept { return lhs.__impl != rhs; }

template <size_t Bits, bool IsSigned, class T>
constexpr __arithm_bool<T> operator< (const integer<Bits, IsSigned>& lhs, const T& rhs) noexcept { return lhs.__impl < rhs; }

template <size_t Bits, bool IsSigned, class T>
constexpr __arithm_bool<T> operator> (const integer<Bits, IsSigned>& lhs, const T& rhs) noexcept { return lhs.__impl > rhs; }

template <size_t Bits, bool IsSigned, class T>
constexpr __arithm_bool<T> operator<=(const integer<Bits, IsSigned>& lhs, const T& rhs) noexcept { return lhs.__impl <= rhs; }

template <size_t Bits, bool IsSigned, class T>
constexpr __arithm_bool<T> operator>=(const integer<Bits, IsSigned>& lhs, const T& rhs) noexcept { return lhs.__impl >= rhs; }

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

constexpr int128_t operator "" _int128(const char* n) noexcept { return int128_t{n}; }
constexpr int256_t operator "" _int256(const char* n) noexcept { return int256_t{n}; }
constexpr int512_t operator "" _int512(const char* n) noexcept { return int512_t{n}; }

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

    // TODO: static assert those:
    assert(1_int256 == 1_int512);
    assert(1_int512 == 1_int512);
    assert(1_int512 == 1);
    assert(1_int512 <= 1);
    assert(1_int512 >= 1);
    assert(1 == 1_int512);
    assert(1 <= 1_int512);
    assert(1 >= 1_int512);
    assert(1_int256 >= 1_int512);
    assert(1_int512 <= 1_int256);


    //assert(0_int512 == 0.0);
    assert(0_int512 <= 0.0);
/*    assert(0_int512 >= 0.0);
    assert(0 == 0_int512);
    assert(0 <= 0_int512);
    assert(0 >= 0_int512);*/

    // TODO:
    // static_assert(1_int256 == 1_int512, "");
    // TODO: static_assert(is_pod<int256_t>::value, "");
}
