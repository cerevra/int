#pragma once

#include "int.h"

#include <array>
#include <cstring>

namespace std {
#define CT(x)                                                                      \
    std::common_type_t<std::decay_t<decltype(rhs)>, std::decay_t<decltype(lhs)>> { \
        x                                                                          \
    }

// numeric limits
template <size_t MachineWords, wide_integer_s Signed>
class numeric_limits<wide_integer<MachineWords, Signed>> {
public:
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = Signed == wide_integer_s::Signed;
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
    static constexpr int digits = CHAR_BIT * MachineWords - (Signed == wide_integer_s::Signed ? 1 : 0);
    static constexpr int digits10 = digits * 38.2308 /*std::log10(2)*/;
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;
    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    static constexpr bool traps = true;
    static constexpr bool tinyness_before = false;

    static constexpr wide_integer<MachineWords, Signed> min() noexcept {
        if (Signed == wide_integer_s::Signed) {
            wide_integer<MachineWords, wide_integer_s::Signed> res{};
            res.m_arr[0] = std::numeric_limits<typename wide_integer<MachineWords, Signed>::signed_base_type>::min();
            return res;
        } else {
            return 0;
        }
    }

    static constexpr wide_integer<MachineWords, Signed> lowest() noexcept {
        return min();
    }

    static constexpr wide_integer<MachineWords, Signed> max() noexcept {
        wide_integer<MachineWords, Signed> res{};
        res.m_arr[0] = Signed == wide_integer_s::Signed
                           ? std::numeric_limits<typename wide_integer<MachineWords, Signed>::signed_base_type>::max()
                           : std::numeric_limits<typename wide_integer<MachineWords, Signed>::base_type>::max();
        for (int i = 1; i < res.arr_size; ++i) {
            res.m_arr[i] = std::numeric_limits<typename wide_integer<MachineWords, Signed>::base_type>::max();
        }
        return res;
    }

    static constexpr wide_integer<MachineWords, Signed> epsilon() noexcept {
        return 0;
    }

    static constexpr wide_integer<MachineWords, Signed> round_error() noexcept {
        return 0;
    }

    static constexpr wide_integer<MachineWords, Signed> infinity() noexcept {
        return 0;
    }

    static constexpr wide_integer<MachineWords, Signed> quiet_NaN() noexcept {
        return 0;
    }

    static constexpr wide_integer<MachineWords, Signed> signaling_NaN() noexcept {
        return 0;
    }

    static constexpr wide_integer<MachineWords, Signed> denorm_min() noexcept {
        return 0;
    }
};

namespace detail {
template <class T>
constexpr bool valid_specialized_numeric_limits(std::false_type /*is_array*/) {
    return std::numeric_limits<T>::is_specialized;
}
template <class T>
constexpr bool valid_specialized_numeric_limits(std::true_type /*is_array*/) {
    return false;
}
}

template <typename T>
static constexpr bool ArithmeticConcept() noexcept {
    return std::detail::valid_specialized_numeric_limits<T>(std::is_array<T>());
}

namespace detail {
template <class T>
constexpr bool valid_specialized_numeric_limits_and_integral(std::false_type /*is_array*/) {
    return std::numeric_limits<T>::is_specialized && std::numeric_limits<T>::is_integer;
}
template <class T>
constexpr bool valid_specialized_numeric_limits_and_integral(std::true_type /*is_array*/) {
    return false;
}
}

template <typename T>
static constexpr bool IntegralConcept() noexcept {
    return std::detail::valid_specialized_numeric_limits_and_integral<T>(std::is_array<T>());
}

// type traits
template <size_t MachineWords, wide_integer_s Signed, size_t MachineWords2, wide_integer_s Signed2>
struct common_type<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>> {
    using type = std::conditional_t <
                     MachineWords ==
                 MachineWords2,
          wide_integer<MachineWords,
                       (Signed == wide_integer_s::Signed && Signed2 == wide_integer_s::Signed)
                           ? wide_integer_s::Signed
                           : wide_integer_s::Unsigned>,
          std::conditional_t<
              MachineWords2<MachineWords,
                            wide_integer<MachineWords, Signed>,
                            wide_integer<MachineWords2, Signed2>>>;
};

template <size_t MachineWords, wide_integer_s Signed, typename Arithmetic>
struct common_type<wide_integer<MachineWords, Signed>, Arithmetic> {
    static_assert(ArithmeticConcept<Arithmetic>(), "");

    using type = std::conditional_t <
                 std::is_floating_point<Arithmetic>::value,
          Arithmetic,
          std::conditional_t<
              sizeof(Arithmetic) < MachineWords,
              wide_integer<MachineWords, Signed>,
              std::conditional_t<
                  MachineWords<sizeof(Arithmetic),
                               Arithmetic,
                               std::conditional_t<
                                   MachineWords == sizeof(Arithmetic) && (Signed == wide_integer_s::Signed || std::is_signed<Arithmetic>::value),
                                   Arithmetic,
                                   wide_integer<MachineWords, Signed>>>>>;
};

template <typename Arithmetic, size_t MachineWords, wide_integer_s Signed>
struct common_type<Arithmetic, wide_integer<MachineWords, Signed>>
    : std::common_type<wide_integer<MachineWords, Signed>, Arithmetic> {};

template <size_t MachineWords, wide_integer_s Signed>
struct wide_integer<MachineWords, Signed>::_impl {
    // utils
    static const int arr_size = sizeof(long) * MachineWords;
    static const int base_bits = sizeof(base_type) * CHAR_BIT;

    template <size_t B, wide_integer_s S>
    constexpr static bool is_negative(const wide_integer<B, S>& n) noexcept {
        return S == wide_integer_s::Signed && static_cast<signed_base_type>(n.m_arr[0]) < 0;
    }

    template <size_t B, wide_integer_s S>
    constexpr static wide_integer<B, S> make_positive(const wide_integer<B, S>& n) noexcept {
        return is_negative(n) ? operator_unary_minus(n) : n;
    }

    template <typename T, class = typename std::enable_if<std::is_signed<T>::value, T>::type>
    constexpr static int64_t to_Integral(T f) noexcept {
        return static_cast<int64_t>(f);
    }
    template <typename T, class = typename std::enable_if<!std::is_signed<T>::value, T>::type>
    constexpr static uint64_t to_Integral(T f) noexcept {
        return static_cast<uint64_t>(f);
    }

    template <typename Integral>
    constexpr static void wide_integer_from_Integral(wide_integer<MachineWords, Signed>& self, Integral rhs) noexcept {
        int r_idx = 0;

        for (; static_cast<size_t>(r_idx) < sizeof(Integral) && r_idx < arr_size; ++r_idx) {
            base_type& curr = self.m_arr[arr_size - 1 - r_idx];
            base_type curr_rhs = (rhs >> (r_idx * CHAR_BIT)) & std::numeric_limits<base_type>::max();
            curr = curr_rhs;
        }

        for (; r_idx < arr_size; ++r_idx) {
            base_type& curr = self.m_arr[arr_size - 1 - r_idx];
            curr = rhs < 0 ? std::numeric_limits<base_type>::max() : 0;
        }
    }

    template <size_t MachineWords2, wide_integer_s Signed2>
    constexpr static void wide_integer_from_wide_integer(wide_integer<MachineWords, Signed>& self, const wide_integer<MachineWords2, Signed2>& rhs) noexcept {
        //        int MachineWords_to_copy = std::min(arr_size, rhs.arr_size);
        auto rhs_arr_size = wide_integer<MachineWords2, Signed2>::_impl::arr_size;
        int base_elems_to_copy = _impl::arr_size < rhs_arr_size ? _impl::arr_size
                                                                : rhs_arr_size;
        for (int i = 0; i < base_elems_to_copy; ++i) {
            self.m_arr[_impl::arr_size - 1 - i] = rhs.m_arr[rhs_arr_size - 1 - i];
        }
        for (int i = 0; i < arr_size - base_elems_to_copy; ++i) {
            self.m_arr[i] = is_negative(rhs) ? std::numeric_limits<base_type>::max() : 0;
        }
    }

    template <typename T>
    using __keep_size = typename std::enable_if<sizeof(T) <= arr_size, wide_integer<MachineWords, Signed>>::type;
    template <size_t MachineWords2, wide_integer_s Signed2>
    using __need_increase_size = typename std::enable_if < MachineWords<MachineWords2, wide_integer<MachineWords2, Signed>>::type;

    constexpr static wide_integer<MachineWords, wide_integer_s::Unsigned> shift_left(const wide_integer<MachineWords, wide_integer_s::Unsigned>& rhs, int n) {
        if (static_cast<size_t>(n) >= base_bits * arr_size)
            return 0;
        if (n <= 0)
            return rhs;

        wide_integer<MachineWords, Signed> lhs = rhs;
        int cur_shift = n % base_bits;
        if (cur_shift) {
            lhs.m_arr[0] <<= cur_shift;
            for (int i = 1; i < arr_size; ++i) {
                lhs.m_arr[i - 1] |= lhs.m_arr[i] >> (base_bits - cur_shift);
                lhs.m_arr[i] <<= cur_shift;
            }
            n -= cur_shift;
        }
        if (n) {
            int i = 0;
            for (; i < arr_size - n / base_bits; ++i) {
                lhs.m_arr[i] = lhs.m_arr[i + n / base_bits];
            }
            for (; i < arr_size; ++i) {
                lhs.m_arr[i] = 0;
            }
        }
        return lhs;
    }

    constexpr static wide_integer<MachineWords, wide_integer_s::Signed> shift_left(const wide_integer<MachineWords, wide_integer_s::Signed>& rhs, int n) {
        // static_assert(is_negative(rhs), "shift left for negative lhsbers is underfined!");
        if (is_negative(rhs)) {
            throw std::runtime_error("shift left for negative lhsbers is underfined!");
        }
        return wide_integer<MachineWords, wide_integer_s::Signed>(shift_left(wide_integer<MachineWords, wide_integer_s::Unsigned>(rhs), n));
    }

    constexpr static wide_integer<MachineWords, wide_integer_s::Unsigned> shift_right(const wide_integer<MachineWords, wide_integer_s::Unsigned>& rhs, int n) noexcept {
        if (static_cast<size_t>(n) >= base_bits * arr_size)
            return 0;
        if (n <= 0)
            return rhs;

        wide_integer<MachineWords, Signed> lhs = rhs;
        int cur_shift = n % base_bits;
        if (cur_shift) {
            lhs.m_arr[arr_size - 1] >>= cur_shift;
            for (int i = arr_size - 2; i >= 0; --i) {
                lhs.m_arr[i + 1] |= lhs.m_arr[i] << (base_bits - cur_shift);
                lhs.m_arr[i] >>= cur_shift;
            }
            n -= cur_shift;
        }
        if (n) {
            int i = arr_size - 1;
            for (; i >= static_cast<int>(n / base_bits); --i) {
                lhs.m_arr[i] = lhs.m_arr[i - n / base_bits];
            }
            for (; i >= 0; --i) {
                lhs.m_arr[i] = 0;
            }
        }
        return lhs;
    }

    constexpr static wide_integer<MachineWords, wide_integer_s::Signed> shift_right(const wide_integer<MachineWords, wide_integer_s::Signed>& rhs, int n) noexcept {
        if (static_cast<size_t>(n) >= base_bits * arr_size)
            return 0;
        if (n <= 0)
            return rhs;

        bool is_neg = is_negative(rhs);
        if (!is_neg) {
            return shift_right(wide_integer<MachineWords, wide_integer_s::Unsigned>(rhs), n);
        }

        wide_integer<MachineWords, Signed> lhs = rhs;
        int cur_shift = n % base_bits;
        if (cur_shift) {
            lhs = shift_right(wide_integer<MachineWords, wide_integer_s::Unsigned>(lhs), cur_shift);
            lhs.m_arr[0] |= std::numeric_limits<base_type>::max() << (base_bits - cur_shift);
            n -= cur_shift;
        }
        if (n) {
            int i = arr_size - 1;
            for (; i >= static_cast<int>(n / base_bits); --i) {
                lhs.m_arr[i] = lhs.m_arr[i - n / base_bits];
            }
            for (; i >= 0; --i) {
                lhs.m_arr[i] = std::numeric_limits<base_type>::max();
            }
        }
        return lhs;
    }

    template <typename T>
    constexpr static wide_integer<MachineWords, Signed> operator_plus_T(const wide_integer<MachineWords, Signed>& lhs, T rhs) noexcept(Signed == wide_integer_s::Unsigned) {
        if (rhs < 0) {
            return _operator_minus_T(lhs, -rhs);
        } else {
            return _operator_plus_T(lhs, rhs);
        }
    }

private:
    template <typename T>
    constexpr static wide_integer<MachineWords, Signed> _operator_minus_T(const wide_integer<MachineWords, Signed>& lhs, T rhs) noexcept(Signed == wide_integer_s::Unsigned) {
        wide_integer<MachineWords, Signed> res = lhs;

        bool is_underflow = false;
        int r_idx = 0;
        for (; static_cast<size_t>(r_idx) < sizeof(T) && r_idx < arr_size; ++r_idx) {
            base_type& res_i = res.m_arr[arr_size - 1 - r_idx];
            base_type curr_rhs = (rhs >> (r_idx * CHAR_BIT)) & std::numeric_limits<base_type>::max();

            if (is_underflow) {
                --res_i;
                is_underflow = res_i == std::numeric_limits<base_type>::max();
            }

            if (res_i < curr_rhs) {
                is_underflow = true;
            }
            res_i -= curr_rhs;
        }

        if (is_underflow && r_idx < arr_size) {
            --res.m_arr[arr_size - 1 - r_idx];
            for (int i = arr_size - 1 - r_idx - 1; i >= 0; --i) {
                if (res.m_arr[i + 1] == std::numeric_limits<base_type>::max()) {
                    --res.m_arr[i];
                } else {
                    break;
                }
            }
        }

        return res;
    }

    template <typename T>
    constexpr static wide_integer<MachineWords, Signed> _operator_plus_T(const wide_integer<MachineWords, Signed>& lhs, T rhs) noexcept(Signed == wide_integer_s::Unsigned) {
        wide_integer<MachineWords, Signed> res = lhs;

        bool is_overflow = false;
        int r_idx = 0;
        for (; static_cast<size_t>(r_idx) < sizeof(T) && r_idx < arr_size; ++r_idx) {
            base_type& res_i = res.m_arr[arr_size - 1 - r_idx];
            base_type curr_rhs = (rhs >> (r_idx * CHAR_BIT)) & std::numeric_limits<base_type>::max();

            if (is_overflow) {
                ++res_i;
                is_overflow = res_i == 0;
            }

            res_i += curr_rhs;
            if (res_i < curr_rhs) {
                is_overflow = true;
            }
        }

        if (is_overflow && r_idx < arr_size) {
            ++res.m_arr[arr_size - 1 - r_idx];
            for (int i = arr_size - 1 - r_idx - 1; i >= 0; --i) {
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
    constexpr static wide_integer<MachineWords, Signed> operator_unary_tilda(const wide_integer<MachineWords, Signed>& lhs) noexcept {
        wide_integer<MachineWords, Signed> res{};
        for (int i = 0; i < arr_size; ++i) {
            res.m_arr[i] = ~lhs.m_arr[i];
        }
        return res;
    }

    constexpr static wide_integer<MachineWords, Signed> operator_unary_minus(const wide_integer<MachineWords, Signed>& lhs) noexcept(Signed == wide_integer_s::Unsigned) {
        return operator_plus_T(operator_unary_tilda(lhs), 1);
    }

    template <typename T, class = __keep_size<T>>
    constexpr static wide_integer<MachineWords, Signed> operator_plus(const wide_integer<MachineWords, Signed>& lhs,
                                                                      const T& rhs) noexcept(Signed == wide_integer_s::Unsigned) {
        wide_integer<MachineWords, Signed> t = rhs;
        if (is_negative(t)) {
            return _operator_minus_wide_integer(lhs, operator_unary_minus(t));
        } else {
            return _operator_plus_wide_integer(lhs, t);
        }
    }

    template <size_t MachineWords2, wide_integer_s Signed2, class = __need_increase_size<MachineWords2, Signed2>>
    constexpr static wide_integer<MachineWords2, Signed> operator_plus(const wide_integer<MachineWords, Signed>& lhs,
                                                                       const wide_integer<MachineWords2, Signed2>& rhs) noexcept(Signed == wide_integer_s::Unsigned) {
        return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_plus(wide_integer<MachineWords2, Signed>(lhs), rhs);
    }

    template <typename T, class = __keep_size<T>>
    constexpr static wide_integer<MachineWords, Signed> operator_minus(const wide_integer<MachineWords, Signed>& lhs,
                                                                       const T& rhs) noexcept(Signed == wide_integer_s::Unsigned) {
        wide_integer<MachineWords, Signed> t = rhs;
        if (is_negative(t)) {
            return _operator_plus_wide_integer(lhs, operator_unary_minus(t));
        } else {
            return _operator_minus_wide_integer(lhs, t);
        }
    }

    template <size_t MachineWords2, wide_integer_s Signed2, class = __need_increase_size<MachineWords2, Signed2>>
    constexpr static wide_integer<MachineWords2, Signed> operator_minus(const wide_integer<MachineWords, Signed>& lhs,
                                                                        const wide_integer<MachineWords2, Signed2>& rhs) noexcept(Signed == wide_integer_s::Unsigned) {
        return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_minus(wide_integer<MachineWords2, Signed>(lhs), rhs);
    }

private:
    constexpr static wide_integer<MachineWords, Signed> _operator_minus_wide_integer(const wide_integer<MachineWords, Signed>& lhs,
                                                                                     const wide_integer<MachineWords, Signed>& rhs) noexcept(Signed == wide_integer_s::Unsigned) {
        wide_integer<MachineWords, Signed> res = lhs;

        bool is_underflow = false;
        for (int idx = arr_size - 1; idx >= 0; --idx) {
            base_type& res_i = res.m_arr[idx];
            const base_type rhs_i = rhs.m_arr[idx];

            if (is_underflow) {
                --res_i;
                is_underflow = res_i == std::numeric_limits<base_type>::max();
            }

            if (res_i < rhs_i) {
                is_underflow = true;
            }
            res_i -= rhs_i;
        }

        return res;
    }

    constexpr static wide_integer<MachineWords, Signed> _operator_plus_wide_integer(const wide_integer<MachineWords, Signed>& lhs,
                                                                                    const wide_integer<MachineWords, Signed>& rhs) noexcept(Signed == wide_integer_s::Unsigned) {
        wide_integer<MachineWords, Signed> res = lhs;

        bool is_overflow = false;
        for (int idx = arr_size - 1; idx >= 0; --idx) {
            base_type& res_i = res.m_arr[idx];
            const base_type rhs_i = rhs.m_arr[idx];

            if (is_overflow) {
                ++res_i;
                is_overflow = res_i == 0;
            }

            res_i += rhs_i;
            if (res_i < rhs_i) {
                is_overflow = true;
            }
        }

        return res;
    }

public:
    template <typename T, class = __keep_size<T>>
    constexpr static wide_integer<MachineWords, Signed> operator_star(const wide_integer<MachineWords, Signed>& lhs,
                                                                      const T& rhs) {
        wide_integer<MachineWords, Signed> a = make_positive(lhs);
        wide_integer<MachineWords, Signed> t = make_positive(wide_integer<MachineWords, Signed>(rhs));

        wide_integer<MachineWords, Signed> res = 0;

        for (size_t i = 0; i < arr_size; ++i) {
            if ((t.m_arr[arr_size - 1] & 1) != 0) {
                res = operator_plus(res, (shift_left(a, i)));
            }

            t = shift_right(t, 1);
        }

        if (Signed == wide_integer_s::Signed &&
            is_negative(wide_integer<MachineWords, Signed>(rhs)) != is_negative(lhs)) {
            res = operator_unary_minus(res);
        }

        return res;
    }

    template <size_t MachineWords2, wide_integer_s Signed2, class = __need_increase_size<MachineWords2, Signed2>>
    constexpr static wide_integer<MachineWords2, Signed2> operator_star(const wide_integer<MachineWords, Signed>& lhs,
                                                                        const wide_integer<MachineWords2, Signed2>& rhs) {
        return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_star(wide_integer<MachineWords2, Signed2>(lhs), rhs);
    }

    template <typename T, class = __keep_size<T>>
    constexpr static bool operator_more(const wide_integer<MachineWords, Signed>& lhs,
                                        const T& rhs) noexcept {
        //        static_assert(Signed == std::is_signed<T>::value,
        //                      "warning: operator_more: comparison of integers of different signs");

        wide_integer<MachineWords, Signed> t = rhs;

        if (std::is_signed<T>::value && (is_negative(lhs) != is_negative(t))) {
            return is_negative(t);
        }

        for (int i = 0; i < arr_size; ++i) {
            if (lhs.m_arr[i] != t.m_arr[i]) {
                return lhs.m_arr[i] > t.m_arr[i];
            }
        }

        return false;
    }

    template <size_t MachineWords2, class = __need_increase_size<MachineWords2, Signed>>
    constexpr static bool operator_more(const wide_integer<MachineWords, Signed>& lhs,
                                        const wide_integer<MachineWords2, Signed>& rhs) noexcept {
        return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed>>::_impl::operator_more(wide_integer<MachineWords2, Signed>(lhs), rhs);
    }

    template <typename T, class = __keep_size<T>>
    constexpr static bool operator_less(const wide_integer<MachineWords, Signed>& lhs,
                                        const T& rhs) noexcept {
        //        static_assert(Signed == std::is_signed<T>::value,
        //                      "warning: operator_less: comparison of integers of different signs");

        wide_integer<MachineWords, Signed> t = rhs;

        if (std::is_signed<T>::value && (is_negative(lhs) != is_negative(t))) {
            return is_negative(lhs);
        }

        for (int i = 0; i < arr_size; ++i) {
            if (lhs.m_arr[i] != t.m_arr[i]) {
                return lhs.m_arr[i] < t.m_arr[i];
            }
        }

        return false;
    }

    template <size_t MachineWords2, class = __need_increase_size<MachineWords2, Signed>>
    constexpr static bool operator_less(const wide_integer<MachineWords, Signed>& lhs,
                                        const wide_integer<MachineWords2, Signed>& rhs) noexcept {
        return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed>>::_impl::operator_less(wide_integer<MachineWords2, Signed>(lhs), rhs);
    }

    template <typename T, class = __keep_size<T>>
    constexpr static bool operator_eq(const wide_integer<MachineWords, Signed>& lhs,
                                      const T& rhs) noexcept {
        wide_integer<MachineWords, Signed> t = rhs;

        for (int i = 0; i < arr_size; ++i) {
            if (lhs.m_arr[i] != t.m_arr[i]) {
                return false;
            }
        }

        return true;
    }

    template <size_t MachineWords2, class = __need_increase_size<MachineWords2, Signed>>
    constexpr static bool operator_eq(const wide_integer<MachineWords, Signed>& lhs,
                                      const wide_integer<MachineWords2, Signed>& rhs) noexcept {
        return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed>>::_impl::operator_eq(wide_integer<MachineWords2, Signed>(lhs), rhs);
    }

    template <typename T, class = __keep_size<T>>
    constexpr static wide_integer<MachineWords, Signed> operator_pipe(const wide_integer<MachineWords, Signed>& lhs,
                                                                      const T& rhs) noexcept {
        wide_integer<MachineWords, Signed> t = rhs;
        wide_integer<MachineWords, Signed> res = lhs;

        for (int i = 0; i < arr_size; ++i) {
            res.m_arr[i] |= t.m_arr[i];
        }

        return res;
    }

    template <size_t MachineWords2, class = __need_increase_size<MachineWords2, Signed>>
    constexpr static wide_integer<MachineWords2, Signed> operator_pipe(const wide_integer<MachineWords, Signed>& lhs,
                                                                       const wide_integer<MachineWords2, Signed>& rhs) noexcept {
        return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed>>::_impl::operator_pipe(wide_integer<MachineWords2, Signed>(lhs), rhs);
    }

    template <typename T, class = __keep_size<T>>
    constexpr static wide_integer<MachineWords, Signed> operator_amp(const wide_integer<MachineWords, Signed>& lhs,
                                                                     const T& rhs) noexcept {
        wide_integer<MachineWords, Signed> t = rhs;
        wide_integer<MachineWords, Signed> res = lhs;

        for (int i = 0; i < arr_size; ++i) {
            res.m_arr[i] &= t.m_arr[i];
        }

        return res;
    }

    template <size_t MachineWords2, class = __need_increase_size<MachineWords2, Signed>>
    constexpr static wide_integer<MachineWords2, Signed> operator_amp(const wide_integer<MachineWords, Signed>& lhs,
                                                                      const wide_integer<MachineWords2, Signed>& rhs) noexcept {
        return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed>>::_impl::operator_amp(wide_integer<MachineWords2, Signed>(lhs), rhs);
    }

private:
    template <typename T>
    constexpr static void divide(const T& lhserator, const T& denominator, T& quotient, T& remainder) {
        bool is_zero = true;
        for (auto c : denominator.m_arr) {
            if (c != 0) {
                is_zero = false;
                break;
            }
        }

        if (is_zero) {
            throw std::domain_error("divide by zero");
        }

        T n = lhserator;
        T d = denominator;
        T x = 1;
        T answer = 0;

        while (!operator_more(d, n) && operator_eq(operator_amp(shift_right(d, base_bits * arr_size - 1), 1), 0)) {
            x = shift_left(x, 1);
            d = shift_left(d, 1);
        }

        while (!operator_eq(x, 0)) {
            if (!operator_more(d, n)) {
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
    template <typename T, class = __keep_size<T>>
    constexpr static wide_integer<MachineWords, Signed> operator_slash(const wide_integer<MachineWords, Signed>& lhs,
                                                                       const T& rhs) {
        wide_integer<MachineWords, Signed> o = rhs;
        wide_integer<MachineWords, Signed> quotient{}, remainder{};
        divide(make_positive(lhs), o, quotient, remainder);

        if (Signed == wide_integer_s::Signed &&
            is_negative(o) != is_negative(lhs)) {
            quotient = operator_unary_minus(quotient);
        }

        return quotient;
    }

    template <size_t MachineWords2, wide_integer_s Signed2, class = __need_increase_size<MachineWords2, Signed2>>
    constexpr static wide_integer<MachineWords2, Signed2> operator_slash(const wide_integer<MachineWords, Signed>& lhs,
                                                                         const wide_integer<MachineWords2, Signed2>& rhs) {
        return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed>>::operator_slash(wide_integer<MachineWords2, Signed2>(lhs), rhs);
    }

    template <typename T, class = __keep_size<T>>
    constexpr static wide_integer<MachineWords, Signed> operator_percent(const wide_integer<MachineWords, Signed>& lhs,
                                                                         const T& rhs) {
        wide_integer<MachineWords, Signed> quotient{}, remainder{};
        divide(make_positive(lhs), wide_integer<MachineWords, Signed>(rhs), quotient, remainder);
        return remainder;
    }

    template <size_t MachineWords2, wide_integer_s Signed2, class = __need_increase_size<MachineWords2, Signed2>>
    constexpr static wide_integer<MachineWords2, Signed2> operator_percent(const wide_integer<MachineWords, Signed>& lhs,
                                                                           const wide_integer<MachineWords2, Signed2>& rhs) {
        return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed>>::operator_percent(wide_integer<MachineWords2, Signed2>(lhs), rhs);
    }

    // ^
    template <typename T, class = __keep_size<T>>
    constexpr static wide_integer<MachineWords, Signed> operator_circumflex(const wide_integer<MachineWords, Signed>& lhs,
                                                                            const T& rhs) noexcept {
        wide_integer<MachineWords, Signed> t(rhs);
        wide_integer<MachineWords, Signed> res = lhs;

        for (int i = 0; i < arr_size; ++i) {
            res.m_arr[i] ^= t.m_arr[i];
        }

        return res;
    }

    template <size_t MachineWords2, wide_integer_s Signed2, class = __need_increase_size<MachineWords2, Signed2>>
    constexpr static wide_integer<MachineWords2, Signed2> operator_circumflex(const wide_integer<MachineWords, Signed>& lhs,
                                                                              const wide_integer<MachineWords2, Signed2>& rhs) noexcept {
        return wide_integer<MachineWords2, Signed2>::operator_circumflex(wide_integer<MachineWords2, Signed2>(lhs), rhs);
    }

    constexpr static wide_integer<MachineWords, Signed> from_str(const char* c) {
        wide_integer<MachineWords, Signed> res = 0;

        bool is_neg = Signed == wide_integer_s::Signed && *c == '-';
        if (is_neg) {
            ++c;
        }

        if (*c == '0' && *(c + 1) == 'x') { // hex
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

    constexpr static wide_integer<MachineWords, Signed> from_str(const wchar_t* c) {
        wide_integer<MachineWords, Signed> res = 0;

        bool is_neg = Signed == wide_integer_s::Signed && *c == L'-';
        if (is_neg) {
            ++c;
        }

        if (*c == L'0' && *(c + 1) == L'x') { // hex
            ++c;
            ++c;
            while (*c) {
                if (*c >= L'0' && *c <= L'9') {
                    res = operator_star(res, 16U);
                    res = operator_plus_T(res, *c - L'0');
                    ++c;
                } else if (*c >= L'a' && *c <= L'f') {
                    res = operator_star(res, 16U);
                    res = operator_plus_T(res, *c - L'a' + 10U);
                    ++c;
                } else if (*c >= L'A' && *c <= L'F') { // tolower must be used, but it is not constexpr
                    res = operator_star(res, 16U);
                    res = operator_plus_T(res, *c - L'A' + 10U);
                    ++c;
                } else {
                    throw std::runtime_error("invalid char from");
                }
            }
        } else { // dec
            while (*c) {
                if (*c < L'0' || *c > L'9') {
                    throw std::runtime_error("invalid char from");
                }
                res = operator_star(res, 10U);
                res = operator_plus_T(res, *c - L'0');
                ++c;
            }
        }

        if (is_neg) {
            res = operator_unary_minus(res);
        }

        return res;
    }
};

// Members

template <size_t MachineWords, wide_integer_s Signed>
template <typename T>
constexpr wide_integer<MachineWords, Signed>::wide_integer(T rhs) noexcept
    : m_arr{} {
    _impl::wide_integer_from_Integral(*this, _impl::to_Integral(rhs));
}

template <size_t MachineWords, wide_integer_s Signed>
template <size_t MachineWords2, wide_integer_s Signed2>
constexpr wide_integer<MachineWords, Signed>::wide_integer(const wide_integer<MachineWords2, Signed2>& rhs) noexcept
    : m_arr{} {
    _impl::wide_integer_from_wide_integer(*this, rhs);
}

template <size_t MachineWords, wide_integer_s Signed>
template <size_t MachineWords2, wide_integer_s Signed2>
constexpr wide_integer<MachineWords, Signed>& wide_integer<MachineWords, Signed>::operator=(const wide_integer<MachineWords2, Signed2>& rhs) noexcept {
    _impl::wide_integer_from_wide_integer(*this, rhs);
    return *this;
}

template <size_t MachineWords, wide_integer_s Signed>
template <typename T>
constexpr wide_integer<MachineWords, Signed>& wide_integer<MachineWords, Signed>::operator=(T rhs) noexcept {
    _impl::wide_integer_from_Integral(*this, rhs);
    return *this;
}

template <size_t MachineWords, wide_integer_s Signed>
template <typename T>
constexpr wide_integer<MachineWords, Signed>& wide_integer<MachineWords, Signed>::operator*=(const T& rhs) {
    *this = _impl::operator_star(*this, wide_integer<MachineWords, Signed>(rhs));
    return *this;
}

template <size_t MachineWords, wide_integer_s Signed>
template <typename T>
constexpr wide_integer<MachineWords, Signed>& wide_integer<MachineWords, Signed>::operator/=(const T& rhs) {
    *this = _impl::operator_slash(*this, wide_integer<MachineWords, Signed>(rhs));
    return *this;
}

template <size_t MachineWords, wide_integer_s Signed>
template <typename T>
constexpr wide_integer<MachineWords, Signed>& wide_integer<MachineWords, Signed>::operator+=(const T& rhs) noexcept(Signed == wide_integer_s::Unsigned) {
    *this = _impl::operator_plus(*this, wide_integer<MachineWords, Signed>(rhs));
    return *this;
}

template <size_t MachineWords, wide_integer_s Signed>
template <typename T>
constexpr wide_integer<MachineWords, Signed>& wide_integer<MachineWords, Signed>::operator-=(const T& rhs) noexcept(Signed == wide_integer_s::Unsigned) {
    *this = _impl::operator_minus(*this, wide_integer<MachineWords, Signed>(rhs));
    return *this;
}

template <size_t MachineWords, wide_integer_s Signed>
template <typename T>
constexpr wide_integer<MachineWords, Signed>& wide_integer<MachineWords, Signed>::operator%=(const T& rhs) {
    *this = _impl::operator_percent(*this, wide_integer<MachineWords, Signed>(rhs));
    return *this;
}

template <size_t MachineWords, wide_integer_s Signed>
template <typename T>
constexpr wide_integer<MachineWords, Signed>& wide_integer<MachineWords, Signed>::operator&=(const T& rhs) noexcept {
    *this = _impl::operator_amp(*this, wide_integer<MachineWords, Signed>(rhs));
    return *this;
}

template <size_t MachineWords, wide_integer_s Signed>
template <typename T>
constexpr wide_integer<MachineWords, Signed>& wide_integer<MachineWords, Signed>::operator|=(const T& rhs) noexcept {
    *this = _impl::operator_pipe(*this, wide_integer<MachineWords, Signed>(rhs));
    return *this;
}

template <size_t MachineWords, wide_integer_s Signed>
template <typename T>
constexpr wide_integer<MachineWords, Signed>& wide_integer<MachineWords, Signed>::operator^=(const T& rhs) noexcept {
    *this = _impl::operator_circumflex(*this, wide_integer<MachineWords, Signed>(rhs));
    return *this;
}

template <size_t MachineWords, wide_integer_s Signed>
constexpr wide_integer<MachineWords, Signed>& wide_integer<MachineWords, Signed>::operator<<=(int n) {
    *this = _impl::shift_left(*this, n);
    return *this;
}

template <size_t MachineWords, wide_integer_s Signed>
constexpr wide_integer<MachineWords, Signed>& wide_integer<MachineWords, Signed>::operator>>=(int n) noexcept {
    *this = _impl::shift_right(*this, n);
    return *this;
}

template <size_t MachineWords, wide_integer_s Signed>
constexpr wide_integer<MachineWords, Signed>::operator bool() const noexcept {
    return !_impl::operator_eq(*this, 0);
}

template <size_t MachineWords, wide_integer_s Signed>
template <class T, class>
constexpr wide_integer<MachineWords, Signed>::operator T() const noexcept {
    static_assert(std::numeric_limits<T>::is_integer, "");
    T res = 0;
    for (size_t r_idx = 0; r_idx < _impl::arr_size && r_idx < sizeof(T); ++r_idx) {
        res |= (T(m_arr[_impl::arr_size - 1 - r_idx]) << (_impl::base_bits * r_idx));
    }
    return res;
}

template <size_t MachineWords, wide_integer_s Signed>
constexpr wide_integer<MachineWords, Signed>::operator long double() const noexcept {
    if (_impl::operator_eq(*this, 0)) {
        return 0;
    }

    long double res = 0;
    for (size_t idx = 0; idx < _impl::arr_size; ++idx) {
        long double t = res;
        res *= std::numeric_limits<base_type>::max();
        res += t;
        res += m_arr[idx];
    }
    return res;
}

template <size_t MachineWords, wide_integer_s Signed>
constexpr wide_integer<MachineWords, Signed>::operator double() const noexcept {
    return static_cast<long double>(*this);
}

template <size_t MachineWords, wide_integer_s Signed>
constexpr wide_integer<MachineWords, Signed>::operator float() const noexcept {
    return static_cast<long double>(*this);
}

// Unary operators
template <size_t MachineWords, wide_integer_s Signed>
constexpr wide_integer<MachineWords, Signed> operator~(const wide_integer<MachineWords, Signed>& lhs) noexcept {
    return wide_integer<MachineWords, Signed>::_impl::operator_unary_tilda(lhs);
}

template <size_t MachineWords, wide_integer_s Signed>
constexpr wide_integer<MachineWords, Signed> operator-(const wide_integer<MachineWords, Signed>& lhs) noexcept(Signed == wide_integer_s::Unsigned) {
    return wide_integer<MachineWords, Signed>::_impl::operator_unary_minus(lhs);
}

template <size_t MachineWords, wide_integer_s Signed>
constexpr wide_integer<MachineWords, Signed> operator+(const wide_integer<MachineWords, Signed>& lhs) noexcept(Signed == wide_integer_s::Unsigned) {
    return lhs;
}

// TODO: __is_wide_integer and __arithm_not_wide_integer must be removed
// TODO: Do we whant that behavior:
//                               std::int512_t variable_1 = 1;
//                               std::cout << variable_1 + "123" << '\n'; // "23"
template <class T>
struct __is_wide_integer : std::false_type {};
template <size_t MachineWords, wide_integer_s Signed>
struct __is_wide_integer<wide_integer<MachineWords, Signed>> : std::true_type {};

template <class T>
using __arithm_not_wide_integer = typename std::enable_if<ArithmeticConcept<T>() && !__is_wide_integer<T>::value, T&>::type;

// Binary operators
template <size_t MachineWords, wide_integer_s Signed, size_t MachineWords2, wide_integer_s Signed2>
std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>> constexpr operator*(const wide_integer<MachineWords, Signed>& lhs, const wide_integer<MachineWords2, Signed2>& rhs) {
    return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_star(lhs, rhs);
}

template <typename Arithmetic, typename Arithmetic2, class>
std::common_type_t<Arithmetic, Arithmetic2> constexpr operator*(const Arithmetic& lhs, const Arithmetic2& rhs) {
    return CT(lhs) * CT(rhs);
}

template <size_t MachineWords, wide_integer_s Signed, size_t MachineWords2, wide_integer_s Signed2>
std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>> constexpr operator/(const wide_integer<MachineWords, Signed>& lhs, const wide_integer<MachineWords2, Signed2>& rhs) {
    return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_slash(lhs, rhs);
}
template <typename Arithmetic, typename Arithmetic2, class>
std::common_type_t<Arithmetic, Arithmetic2> constexpr operator/(const Arithmetic& lhs, const Arithmetic2& rhs) {
    return CT(lhs) / CT(rhs);
}

template <size_t MachineWords, wide_integer_s Signed, size_t MachineWords2, wide_integer_s Signed2>
std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>> constexpr operator+(const wide_integer<MachineWords, Signed>& lhs, const wide_integer<MachineWords2, Signed2>& rhs) {
    return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_plus(lhs, rhs);
}
template <typename Arithmetic, typename Arithmetic2, class>
std::common_type_t<Arithmetic, Arithmetic2> constexpr operator+(const Arithmetic& lhs, const Arithmetic2& rhs) {
    return CT(lhs) + CT(rhs);
}

template <size_t MachineWords, wide_integer_s Signed, size_t MachineWords2, wide_integer_s Signed2>
std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>> constexpr operator-(const wide_integer<MachineWords, Signed>& lhs, const wide_integer<MachineWords2, Signed2>& rhs) {
    return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_minus(lhs, rhs);
}
template <typename Arithmetic, typename Arithmetic2, class>
std::common_type_t<Arithmetic, Arithmetic2> constexpr operator-(const Arithmetic& lhs, const Arithmetic2& rhs) {
    return CT(lhs) - CT(rhs);
}

template <size_t MachineWords, wide_integer_s Signed, size_t MachineWords2, wide_integer_s Signed2>
std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>> constexpr operator%(const wide_integer<MachineWords, Signed>& lhs, const wide_integer<MachineWords2, Signed2>& rhs) {
    return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_percent(lhs, rhs);
}
template <typename Integral, typename Integral2, class>
std::common_type_t<Integral, Integral2> constexpr operator%(const Integral& lhs, const Integral2& rhs) {
    return CT(lhs) % CT(rhs);
}

template <size_t MachineWords, wide_integer_s Signed, size_t MachineWords2, wide_integer_s Signed2>
std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>> constexpr operator&(const wide_integer<MachineWords, Signed>& lhs, const wide_integer<MachineWords2, Signed2>& rhs) {
    return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_amp(lhs, rhs);
}
template <typename Integral, typename Integral2, class>
std::common_type_t<Integral, Integral2> constexpr operator&(const Integral& lhs, const Integral2& rhs) {
    return CT(lhs) & CT(rhs);
}

template <size_t MachineWords, wide_integer_s Signed, size_t MachineWords2, wide_integer_s Signed2>
std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>> constexpr operator|(const wide_integer<MachineWords, Signed>& lhs, const wide_integer<MachineWords2, Signed2>& rhs) {
    return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_pipe(lhs, rhs);
}
template <typename Integral, typename Integral2, class>
std::common_type_t<Integral, Integral2> constexpr operator|(const Integral& lhs, const Integral2& rhs) {
    return CT(lhs) | CT(rhs);
}

template <size_t MachineWords, wide_integer_s Signed, size_t MachineWords2, wide_integer_s Signed2>
std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>> constexpr operator^(const wide_integer<MachineWords, Signed>& lhs, const wide_integer<MachineWords2, Signed2>& rhs) {
    return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_circumflex(lhs, rhs);
}
template <typename Integral, typename Integral2, class>
std::common_type_t<Integral, Integral2> constexpr operator^(const Integral& lhs, const Integral2& rhs) {
    return CT(lhs) ^ CT(rhs);
}

template <size_t MachineWords, wide_integer_s Signed>
constexpr wide_integer<MachineWords, Signed> operator<<(const wide_integer<MachineWords, Signed>& lhs, int n) noexcept {
    return wide_integer<MachineWords, Signed>::_impl::shift_left(lhs, n);
}
template <size_t MachineWords, wide_integer_s Signed>
constexpr wide_integer<MachineWords, Signed> operator>>(const wide_integer<MachineWords, Signed>& lhs, int n) noexcept {
    return wide_integer<MachineWords, Signed>::_impl::shift_right(lhs, n);
}

template <size_t MachineWords, wide_integer_s Signed, size_t MachineWords2, wide_integer_s Signed2>
constexpr bool operator<(const wide_integer<MachineWords, Signed>& lhs,
                         const wide_integer<MachineWords2, Signed2>& rhs) {
    return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_less(lhs, rhs);
}
template <typename Arithmetic, typename Arithmetic2, class>
constexpr bool operator<(const Arithmetic& lhs, const Arithmetic2& rhs) {
    return CT(lhs) < CT(rhs);
}

template <size_t MachineWords, wide_integer_s Signed, size_t MachineWords2, wide_integer_s Signed2>
constexpr bool operator>(const wide_integer<MachineWords, Signed>& lhs,
                         const wide_integer<MachineWords2, Signed2>& rhs) {
    return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_more(lhs, rhs);
}
template <typename Arithmetic, typename Arithmetic2, class>
constexpr bool operator>(const Arithmetic& lhs, const Arithmetic2& rhs) {
    return CT(lhs) > CT(rhs);
}

template <size_t MachineWords, wide_integer_s Signed, size_t MachineWords2, wide_integer_s Signed2>
constexpr bool operator<=(const wide_integer<MachineWords, Signed>& lhs,
                          const wide_integer<MachineWords2, Signed2>& rhs) {
    return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_less(lhs, rhs) ||
           std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_eq(lhs, rhs);
}
template <typename Arithmetic, typename Arithmetic2, class>
constexpr bool operator<=(const Arithmetic& lhs, const Arithmetic2& rhs) {
    return CT(lhs) <= CT(rhs);
}

template <size_t MachineWords, wide_integer_s Signed, size_t MachineWords2, wide_integer_s Signed2>
constexpr bool operator>=(const wide_integer<MachineWords, Signed>& lhs,
                          const wide_integer<MachineWords2, Signed2>& rhs) {
    return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_more(lhs, rhs) ||
           std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_eq(lhs, rhs);
}
template <typename Arithmetic, typename Arithmetic2, class>
constexpr bool operator>=(const Arithmetic& lhs, const Arithmetic2& rhs) {
    return CT(lhs) >= CT(rhs);
}

template <size_t MachineWords, wide_integer_s Signed, size_t MachineWords2, wide_integer_s Signed2>
constexpr bool operator==(const wide_integer<MachineWords, Signed>& lhs,
                          const wide_integer<MachineWords2, Signed2>& rhs) {
    return std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_eq(lhs, rhs);
}
template <typename Arithmetic, typename Arithmetic2, class>
constexpr bool operator==(const Arithmetic& lhs, const Arithmetic2& rhs) {
    return CT(lhs) == CT(rhs);
}

template <size_t MachineWords, wide_integer_s Signed, size_t MachineWords2, wide_integer_s Signed2>
constexpr bool operator!=(const wide_integer<MachineWords, Signed>& lhs,
                          const wide_integer<MachineWords2, Signed2>& rhs) {
    return !std::common_type_t<wide_integer<MachineWords, Signed>, wide_integer<MachineWords2, Signed2>>::_impl::operator_eq(lhs, rhs);
}
template <typename Arithmetic, typename Arithmetic2, class>
constexpr bool operator!=(const Arithmetic& lhs, const Arithmetic2& rhs) {
    return CT(lhs) != CT(rhs);
}

template <size_t MachineWords, wide_integer_s Signed>
std::string to_string(const wide_integer<MachineWords, Signed>& n) {
    std::string res;
    if (wide_integer<MachineWords, Signed>::_impl::operator_eq(n, 0U)) {
        return "0";
    }

    wide_integer<MachineWords, wide_integer_s::Unsigned> t;
    bool is_neg = wide_integer<MachineWords, Signed>::_impl::is_negative(n);
    if (is_neg) {
        t = wide_integer<MachineWords, Signed>::_impl::operator_unary_minus(n);
    } else {
        t = n;
    }

    while (!wide_integer<MachineWords, wide_integer_s::Unsigned>::_impl::operator_eq(t, 0U)) {
        res.insert(res.begin(), '0' + char(wide_integer<MachineWords, wide_integer_s::Unsigned>::_impl::operator_percent(t, 10U)));
        t = wide_integer<MachineWords, wide_integer_s::Unsigned>::_impl::operator_slash(t, 10U);
    }

    if (is_neg) {
        res.insert(res.begin(), '-');
    }

    return res;
}

template <size_t MachineWords, wide_integer_s Signed>
std::wstring to_wstring(const wide_integer<MachineWords, Signed>& n) {
    std::wstring res;
    if (wide_integer<MachineWords, Signed>::_impl::operator_eq(n, 0U)) {
        return L"0";
    }

    wide_integer<MachineWords, wide_integer_s::Unsigned> t;
    bool is_neg = wide_integer<MachineWords, Signed>::_impl::is_negative(n);
    if (is_neg) {
        t = wide_integer<MachineWords, Signed>::_impl::operator_unary_minus(n);
    } else {
        t = n;
    }

    while (!wide_integer<MachineWords, wide_integer_s::Unsigned>::_impl::operator_eq(t, 0U)) {
        res.insert(res.begin(), '0' + wchar_t(wide_integer<MachineWords, wide_integer_s::Unsigned>::_impl::operator_percent(t, 10U)));
        t = wide_integer<MachineWords, wide_integer_s::Unsigned>::_impl::operator_slash(t, 10U);
    }

    if (is_neg) {
        res.insert(res.begin(), '-');
    }

    return res;
}

template <size_t MachineWords, wide_integer_s Signed>
std::ostream& operator<<(std::ostream& out, const wide_integer<MachineWords, Signed>& n) {
    out << to_string(n);
    return out;
}

template <size_t MachineWords, wide_integer_s Signed>
std::wostream& operator<<(std::wostream& out, const wide_integer<MachineWords, Signed>& n) {
    out << to_wstring(n);
    return out;
}

template <size_t MachineWords, wide_integer_s Signed>
std::istream& operator>>(std::istream& in, wide_integer<MachineWords, Signed>& n) {
    std::string s;
    in >> s;
    n = wide_integer<MachineWords, Signed>::_impl::from_str(s.c_str());
    return in;
}

template <size_t MachineWords, wide_integer_s Signed>
std::wistream& operator>>(std::wistream& in, wide_integer<MachineWords, Signed>& n) {
    std::wstring s;
    in >> s;
    n = wide_integer<MachineWords, Signed>::_impl::from_str(s.c_str());
    return in;
}

template <size_t MachineWords, wide_integer_s Signed>
to_chars_result to_chars(char* first,
                         char* last,
                         const wide_integer<MachineWords, Signed>& value,
                         int base) {
    if (base < 2 || base > 36) {
        return {last,
                std::make_error_code(std::errc::invalid_argument)};
    }
    if (first >= last) {
        return {last,
                std::make_error_code(std::errc::invalid_argument)};
    }

    if (value == 0) {
        *first = '0';
        *(++first) = '\0';
        return {++first, {}};
    }

    wide_integer<MachineWords, Signed> v = value;
    if (v < 0) {
        v = -v;
        *(first++) = '-';
    }

    char* cur = last;

    while (v != 0 && --cur >= first) {
        static const char ALPHA[] = "0123456789abcdefghijklmnopqrstuvwxyz";
        *cur = ALPHA[v % base];
        v /= base;
    }

    if (v && cur + 1 == first) {
        return {nullptr,
                std::make_error_code(std::errc::value_too_large)};
    }

    while (cur < last) {
        *(first++) = *(cur++);
    }
    if (first < last) {
        *first = '\0';
    }

    return {first, {}};
}

std::array<char, 256> genReverseAlpha() noexcept {
    static const char ALPHA[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    std::array<char, 256> res;
    res.fill(-1);
    for (size_t i = 0; i < sizeof(ALPHA); ++i) {
        res[ALPHA[i]] = i;
    }
    return res;
}

template <size_t MachineWords, wide_integer_s Signed>
from_chars_result from_chars(const char* first,
                             const char* last,
                             wide_integer<MachineWords, Signed>& value,
                             int base) {
    if (base < 2 || base > 36) {
        return {first,
                std::make_error_code(std::errc::invalid_argument)};
    }
    if (first >= last) {
        return {first,
                std::make_error_code(std::errc::invalid_argument)};
    }

    bool is_negative = *first == '-';
    if (is_negative) {
        if (Signed == wide_integer_s::Unsigned) {
            return {first,
                    std::make_error_code(std::errc::result_out_of_range)};
        }
        if (++first >= last) {
            return {first,
                    std::make_error_code(std::errc::invalid_argument)};
        }
    }

    wide_integer<MachineWords, Signed> v = 0;
    const char* cur = first;

    do {
        static const std::array<char, 256> ALPHA = genReverseAlpha();
        char cv = ALPHA[*cur];
        if (cv >= base || cv == -1) {
            if (cur == first) {
                return {cur,
                        std::make_error_code(std::errc::result_out_of_range)};
            } else {
                value = v;
                return {cur, {}};
            }
        }

        v *= base;
        v += cv;
    } while (++cur < last);

    value = is_negative ? -v : v;
    return {cur, {}};
}

constexpr int128_t operator"" _int128(const char* n) {
    return int128_t::_impl::from_str(n);
}
constexpr int256_t operator"" _int256(const char* n) {
    return int256_t::_impl::from_str(n);
}
constexpr int512_t operator"" _int512(const char* n) {
    return int512_t::_impl::from_str(n);
}
constexpr uint128_t operator"" _uint128(const char* n) {
    return uint128_t::_impl::from_str(n);
}
constexpr uint256_t operator"" _uint256(const char* n) {
    return uint256_t::_impl::from_str(n);
}
constexpr uint512_t operator"" _uint512(const char* n) {
    return uint512_t::_impl::from_str(n);
}

template <size_t MachineWords, wide_integer_s Signed>
struct hash<wide_integer<MachineWords, Signed>> {
    std::size_t operator()(const wide_integer<MachineWords, Signed>& lhs) const {
        size_t res = 0;
        for (auto n : lhs.m_arr) {
            res += n;
        }
        return hash<size_t>()(res);
    }
};

#undef CT
} // namespace std
