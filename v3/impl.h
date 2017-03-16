#pragma once

#include "int.h"

namespace std {

// type traits
template<size_t Bytes, wide_int_s Signed, size_t Bytes2, wide_int_s Signed2>
struct common_type<wide_int<Bytes, Signed>, wide_int<Bytes2, Signed2>> {
    using type = std::conditional_t<
            Bytes == Bytes2,
            wide_int<Bytes,
                (Signed == wide_int_s::Signed && Signed2 == wide_int_s::Signed)
                    ? wide_int_s::Signed
                    : wide_int_s::Unsigned
            >,
            std::conditional_t<
                Bytes2 < Bytes,
                wide_int<Bytes, Signed>,
                wide_int<Bytes2, Signed2
            >
        >
    >;
};

template<size_t Bytes, wide_int_s Signed, typename Arithmetic>
struct common_type<wide_int<Bytes, Signed>, Arithmetic> {
    static_assert(std::is_arithmetic<Arithmetic>::value, "");

    using type = std::conditional_t<
        std::is_floating_point<Arithmetic>::value,
        Arithmetic,
        std::conditional_t<
            sizeof(Arithmetic) < Bytes,
            wide_int<Bytes, Signed>,
            std::conditional_t<
                Bytes < sizeof(Arithmetic),
                Arithmetic,
                std::conditional_t<
                    Bytes == sizeof(Arithmetic) && ( Signed == wide_int_s::Signed || std::is_signed<Arithmetic>::value ),
                    Arithmetic,
                    wide_int<Bytes, Signed>
                >
            >
        >
    >;
};




template<size_t Bytes, wide_int_s Signed>
struct wide_int<Bytes, Signed>::_impl {

    // utils
    constexpr static int arr_size() noexcept {
//        return Bytes/base_bits(); // for template<size_t Bits, wide_int_s Signed>
        return Bytes; // for template<size_t Bytes, wide_int_s Signed>
    }
    constexpr static int base_bits() noexcept {
        return sizeof(base_type)*CHAR_BIT;
    }

    template<size_t B, wide_int_s S>
    constexpr static bool is_negative(const wide_int<B,S>& n) noexcept {
        return S == wide_int_s::Signed && static_cast<signed_base_type>(n.m_arr[0]) < 0;
    }

    template<typename T, class = typename std::enable_if<std::is_signed<T>::value, T>::type>
    constexpr static int64_t to_Integral(T f) noexcept {
        return static_cast<int64_t>(f);
    }
    template<typename T, class = typename std::enable_if<!std::is_signed<T>::value, T>::type>
    constexpr static uint64_t to_Integral(T f) noexcept {
        return static_cast<uint64_t>(f);
    }

    template<typename Integral>
    constexpr static void wide_int_from_Integral(wide_int<Bytes, Signed>& self, Integral other) noexcept {
        int r_idx = 0;

        for ( ; static_cast<size_t>(r_idx) < sizeof(Integral) && r_idx < arr_size(); ++r_idx) {
            base_type& curr = self.m_arr[arr_size() - 1 - r_idx];
            base_type curr_other = (other >> (r_idx*CHAR_BIT)) & std::numeric_limits<base_type>::max();
            curr = curr_other;
        }

        for ( ; r_idx < arr_size(); ++r_idx) {
            base_type& curr = self.m_arr[arr_size() - 1 - r_idx];
            curr = other < 0 ? std::numeric_limits<base_type>::max() : 0;
        }
    }
    template<size_t Bytes2, wide_int_s Signed2>
    constexpr static void wide_int_from_wide_int(wide_int<Bytes, Signed>& self, const wide_int<Bytes2,Signed2>& other) noexcept {
//        int bytes_to_copy = std::min(arr_size(), other.arr_size());
        auto other_arr_size = wide_int<Bytes2,Signed2>::_impl::arr_size();
        int bytes_to_copy = _impl::arr_size() < other_arr_size ? _impl::arr_size()
                                                               : other_arr_size;
        for (int i = 0; i < bytes_to_copy; ++i) {
            self.m_arr[_impl::arr_size() - 1 - i] = other.m_arr[other_arr_size - 1 - i];
        }
        for (int i = 0; i < arr_size() - bytes_to_copy; ++i) {
            self.m_arr[i] = is_negative(other) ? std::numeric_limits<base_type>::max() : 0;
        }
    }

    template<typename T>
    using __keep_size = typename std::enable_if<sizeof(T) <= Bytes, wide_int<Bytes,Signed>>::type;
    template<size_t Bytes2, wide_int_s Signed2>
    using __need_increase_size = typename std::enable_if<Bytes < Bytes2, wide_int<Bytes2,Signed>>::type;

    constexpr static wide_int<Bytes,wide_int_s::Unsigned> shift_left(const wide_int<Bytes,wide_int_s::Unsigned>& other, int n) {
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

    constexpr static wide_int<Bytes,wide_int_s::Signed> shift_left(const wide_int<Bytes,wide_int_s::Signed>& other, int n) {
//        static_assert(is_negative(other), "shift left for negative numbers is underfined!");
        if (is_negative(other)) {
            throw std::runtime_error("shift left for negative numbers is underfined!");
        }
        return wide_int<Bytes,wide_int_s::Signed>(shift_left(wide_int<Bytes,wide_int_s::Unsigned>(other), n));
    }

    constexpr static wide_int<Bytes,wide_int_s::Unsigned> shift_right(const wide_int<Bytes,wide_int_s::Unsigned>& other, int n) noexcept {
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

    constexpr static wide_int<Bytes,wide_int_s::Signed> shift_right(const wide_int<Bytes,wide_int_s::Signed>& other, int n) noexcept {
        if (static_cast<size_t>(n) >= base_bits()*arr_size()) return 0;
        if (n <= 0) return other;

        bool is_neg = is_negative(other);
        if (!is_neg) {
            return shift_right(wide_int<Bytes,wide_int_s::Unsigned>(other), n);
        }

        wide_int<Bytes,Signed> num = other;
        int cur_shift = n % base_bits();
        if (cur_shift) {
            num = shift_right(wide_int<Bytes,wide_int_s::Unsigned>(num), cur_shift);
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
    constexpr static wide_int<Bytes,Signed> operator_plus_T(const wide_int<Bytes,Signed>& num, T other) noexcept(Signed == wide_int_s::Unsigned) {
        if (other < 0) {
            return _operator_minus_T(num, -other);
        } else {
            return _operator_plus_T(num, other);
        }
    }

private:
    template<typename T>
    constexpr static wide_int<Bytes,Signed> _operator_minus_T(const wide_int<Bytes,Signed>& num, T other) noexcept(Signed == wide_int_s::Unsigned) {
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
    constexpr static wide_int<Bytes,Signed> _operator_plus_T(const wide_int<Bytes,Signed>& num, T other) noexcept(Signed == wide_int_s::Unsigned) {
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

    constexpr static wide_int<Bytes,Signed> operator_unary_minus(const wide_int<Bytes,Signed>& num) noexcept(Signed == wide_int_s::Unsigned) {
        return operator_plus_T(operator_unary_tilda(num), 1);
    }

    template<typename T>
    using __keep_size = typename std::enable_if<sizeof(T) <= Bytes, wide_int<Bytes,Signed>>::type;
    template<size_t Bytes2, wide_int_s Signed2>
    using __need_increase_size = typename std::enable_if<Bytes < Bytes2, wide_int<Bytes2,Signed>>::type;

    template<typename T, class = __keep_size<T>>
    constexpr static wide_int<Bytes,Signed> operator_plus(const wide_int<Bytes,Signed>& num,
                                                          const T& other) noexcept(Signed == wide_int_s::Unsigned) {
        wide_int<Bytes,Signed> t = other;
        if (is_negative(t)) {
            return _operator_minus_wide_int(num, operator_unary_minus(t));
        } else {
            return _operator_plus_wide_int(num, t);
        }
    }

    template<size_t Bytes2, wide_int_s Signed2, class = __need_increase_size<Bytes2, Signed2>>
    constexpr static wide_int<Bytes2, Signed> operator_plus(const wide_int<Bytes,Signed>& num,
                                                            const wide_int<Bytes2,Signed2>& other) noexcept(Signed == wide_int_s::Unsigned) {
        return wide_int<Bytes2,Signed>::_impl::operator_plus(wide_int<Bytes2,Signed>(num), other);
    }

    template<typename T, class = __keep_size<T>>
    constexpr static wide_int<Bytes,Signed> operator_minus(const wide_int<Bytes,Signed>& num,
                                                           const T& other) noexcept(Signed == wide_int_s::Unsigned) {
        wide_int<Bytes,Signed> t = other;
        if (is_negative(t)) {
            return _operator_plus_wide_int(num, operator_unary_minus(t));
        } else {
            return _operator_minus_wide_int(num, t);
        }
    }

    template<size_t Bytes2, wide_int_s Signed2, class = __need_increase_size<Bytes2, Signed2>>
    constexpr static wide_int<Bytes2, Signed> operator_minus(const wide_int<Bytes,Signed>& num,
                                                             const wide_int<Bytes2,Signed2>& other) noexcept(Signed == wide_int_s::Unsigned) {
        return wide_int<Bytes2,Signed>::_impl::operator_minus(wide_int<Bytes2,Signed>(num), other);
    }
private:
    constexpr static wide_int<Bytes,Signed> _operator_minus_wide_int(const wide_int<Bytes,Signed>& num,
                                                                     const wide_int<Bytes,Signed>& other) noexcept(Signed == wide_int_s::Unsigned) {
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
                                                                    const wide_int<Bytes,Signed>& other) noexcept(Signed == wide_int_s::Unsigned) {
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
                                                          const T& other) {

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

    template<size_t Bytes2, wide_int_s Signed2, class = __need_increase_size<Bytes2, Signed2>>
    constexpr static wide_int<Bytes2, Signed2> operator_star(const wide_int<Bytes,Signed>& num,
                                                             const wide_int<Bytes2,Signed2>& other) {
        return wide_int<Bytes2,Signed2>::_impl::operator_star(wide_int<Bytes2,Signed2>(num), other);
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
        return wide_int<Bytes2,Signed>::_impl::operator_more(wide_int<Bytes2,Signed>(num), other);
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
        return wide_int<Bytes2,Signed>::_impl::operator_less(wide_int<Bytes2,Signed>(num), other);
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
        return wide_int<Bytes2,Signed>::_impl::operator_eq(wide_int<Bytes2,Signed>(num), other);
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
        return wide_int<Bytes2,Signed>::_impl::operator_pipe(wide_int<Bytes2,Signed>(num), other);
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
        return wide_int<Bytes2,Signed>::_impl::operator_amp(wide_int<Bytes2,Signed>(num), other);
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
                                                           const T& other) {

        wide_int<Bytes,Signed> quotient{}, remainder{};
        divide(num, wide_int<Bytes,Signed>(other), quotient, remainder);
        return quotient;
    }

    template<size_t Bytes2, wide_int_s Signed2, class = __need_increase_size<Bytes2, Signed2>>
    constexpr static wide_int<Bytes2, Signed2> operator_slash(const wide_int<Bytes,Signed>& num,
                                                              const wide_int<Bytes2,Signed2>& other) {
        return wide_int<Bytes2,Signed2>::operator_slash(wide_int<Bytes2,Signed2>(num), other);
    }

    template<typename T, class = __keep_size<T>>
    constexpr static wide_int<Bytes,Signed> operator_percent(const wide_int<Bytes,Signed>& num,
                                                             const T& other) {

        wide_int<Bytes,Signed> quotient{}, remainder{};
        divide(num, wide_int<Bytes,Signed>(other), quotient, remainder);
        return remainder;
    }

    template<size_t Bytes2, wide_int_s Signed2, class = __need_increase_size<Bytes2, Signed2>>
    constexpr static wide_int<Bytes2, Signed2> operator_percent(const wide_int<Bytes,Signed>& num,
                                                                const wide_int<Bytes2,Signed2>& other) {
        return wide_int<Bytes2,Signed2>::operator_percent(wide_int<Bytes2,Signed2>(num), other);
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

    template<size_t Bytes2, wide_int_s Signed2, class = __need_increase_size<Bytes2, Signed2>>
    constexpr static wide_int<Bytes2, Signed2> operator_circumflex(const wide_int<Bytes,Signed>& num,
                                                                   const wide_int<Bytes2,Signed2>& other) noexcept {
        return wide_int<Bytes2,Signed2>::operator_circumflex(wide_int<Bytes2,Signed2>(num), other);
    }


    constexpr static wide_int<Bytes, Signed> from_str(const char* c) {
        wide_int<Bytes,Signed> res = 0;

        bool is_neg = Signed == wide_int_s::Signed && *c == '-';
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

    constexpr static wide_int<Bytes, Signed> from_str(const wchar_t* c) {
        wide_int<Bytes,Signed> res = 0;

        bool is_neg = Signed == wide_int_s::Signed && *c == L'-';
        if (is_neg) {
            ++c;
        }

        if (*c == L'0' && *(c+1) == L'x') { // hex
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

template<size_t Bytes, wide_int_s Signed>
template<typename T>
constexpr wide_int<Bytes,Signed>::wide_int(T other) noexcept
    : m_arr {}
{
    _impl::wide_int_from_Integral(*this, _impl::to_Integral(other));
}


template<size_t Bytes, wide_int_s Signed>
template<size_t Bytes2, wide_int_s Signed2>
constexpr wide_int<Bytes,Signed>::wide_int(const wide_int<Bytes2,Signed2>& other) noexcept
    : m_arr {}
{
    _impl::wide_int_from_wide_int(*this, other);
}


template<size_t Bytes, wide_int_s Signed>
template<size_t Bytes2, wide_int_s Signed2>
constexpr wide_int<Bytes,Signed>& wide_int<Bytes,Signed>::operator=(const wide_int<Bytes2,Signed2>& other) noexcept {
    _impl::wide_int_from_wide_int(*this, other);
    return *this;
}

template<size_t Bytes, wide_int_s Signed>
template<typename T>
constexpr wide_int<Bytes,Signed>& wide_int<Bytes,Signed>::operator=(T other) noexcept {
    _impl::wide_int_from_Integral(*this, other);
    return *this;
}

template<size_t Bytes, wide_int_s Signed>
template<typename T>
constexpr wide_int<Bytes,Signed>& wide_int<Bytes,Signed>::operator*=(const T& other) {
    *this = _impl::operator_star(*this, wide_int<Bytes,Signed>(other));
    return *this;
}

template<size_t Bytes, wide_int_s Signed>
template<typename T>
constexpr wide_int<Bytes,Signed>& wide_int<Bytes,Signed>::operator/=(const T& other) {
    *this = _impl::operator_slash(*this, wide_int<Bytes,Signed>(other));
    return *this;
}

template<size_t Bytes, wide_int_s Signed>
template<typename T>
constexpr wide_int<Bytes,Signed>& wide_int<Bytes,Signed>::operator+=(const T& other) noexcept(Signed == wide_int_s::Unsigned) {
    *this = _impl::operator_plus(*this, wide_int<Bytes,Signed>(other));
    return *this;
}

template<size_t Bytes, wide_int_s Signed>
template<typename T>
constexpr wide_int<Bytes,Signed>& wide_int<Bytes,Signed>::operator-=(const T& other) noexcept(Signed == wide_int_s::Unsigned) {
    *this = _impl::operator_minus(*this, wide_int<Bytes,Signed>(other));
    return *this;
}

template<size_t Bytes, wide_int_s Signed>
template<typename T>
constexpr wide_int<Bytes,Signed>& wide_int<Bytes,Signed>::operator%=(const T& other) {
    *this = _impl::operator_percent(*this, wide_int<Bytes,Signed>(other));
    return *this;
}

template<size_t Bytes, wide_int_s Signed>
template<typename T>
constexpr wide_int<Bytes,Signed>& wide_int<Bytes,Signed>::operator&=(const T& other) noexcept {
    *this = _impl::operator_amp(*this, wide_int<Bytes,Signed>(other));
    return *this;
}

template<size_t Bytes, wide_int_s Signed>
template<typename T>
constexpr wide_int<Bytes,Signed>& wide_int<Bytes,Signed>::operator|=(const T& other) noexcept {
    *this = _impl::operator_pipe(*this, wide_int<Bytes,Signed>(other));
    return *this;
}

template<size_t Bytes, wide_int_s Signed>
template<typename T>
constexpr wide_int<Bytes,Signed>& wide_int<Bytes,Signed>::operator^=(const T& other) noexcept {
    *this = _impl::operator_circumflex(*this, wide_int<Bytes,Signed>(other));
    return *this;
}

template<size_t Bytes, wide_int_s Signed>
constexpr wide_int<Bytes,Signed>& wide_int<Bytes,Signed>::operator<<=(int n) {
    *this = _impl::shift_left(*this, n);
    return *this;
}

template<size_t Bytes, wide_int_s Signed>
constexpr wide_int<Bytes,Signed>& wide_int<Bytes,Signed>::operator>>=(int n) noexcept {
    *this = _impl::shift_right(*this, n);
    return *this;
}


template<size_t Bytes, wide_int_s Signed>
constexpr wide_int<Bytes,Signed>::operator bool () const noexcept {
    return !_impl::operator_eq(*this, 0);
}

template<size_t Bytes, wide_int_s Signed>
template <class T, class>
constexpr wide_int<Bytes,Signed>::operator T () const noexcept {
    static_assert(std::is_integral<T>::value, "");
    T res = 0;
    for (size_t r_idx = 0; r_idx < _impl::arr_size() && r_idx < sizeof(T); ++r_idx) {
        res |= m_arr[_impl::arr_size() - 1 - r_idx] << (_impl::base_bits() * r_idx);
    }
    return res;
}


template<size_t Bytes, wide_int_s Signed>
constexpr wide_int<Bytes,Signed>::operator long double () const noexcept {
    if (_impl::operator_eq(*this, 0)) {
        return 0;
    }

    long double res = 0;
    for (size_t idx = 0; idx < _impl::arr_size(); ++idx) {
        long double t = res;
        res *= std::numeric_limits<base_type>::max();
        res += t;
        res += m_arr[idx];
    }
    return res;
}

template<size_t Bytes, wide_int_s Signed>
constexpr wide_int<Bytes,Signed>::operator double () const noexcept {
    return static_cast<long double>(*this);
}

template<size_t Bytes, wide_int_s Signed>
constexpr wide_int<Bytes,Signed>::operator float () const noexcept {
    return static_cast<long double>(*this);
}



// Unary operators
template<size_t Bytes, wide_int_s Signed>
constexpr wide_int<Bytes,Signed> operator~(const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::_impl::operator_unary_tilda(num);
}

template<size_t Bytes, wide_int_s Signed>
constexpr wide_int<Bytes,Signed> operator-(const wide_int<Bytes,Signed>& num) noexcept(Signed == wide_int_s::Unsigned) {
    return wide_int<Bytes,Signed>::_impl::operator_unary_minus(num);
}

template<size_t Bytes, wide_int_s Signed>
constexpr wide_int<Bytes,Signed> operator+(const wide_int<Bytes,Signed>& num) noexcept(Signed == wide_int_s::Unsigned) {
    return num;
}


template <class T> struct __is_wide_int: std::false_type{};
template <size_t Bytes, wide_int_s Signed> struct __is_wide_int<wide_int<Bytes, Signed> >: std::true_type{};
template <class T>
using __arithm_not_wide_int = typename std::enable_if< std::is_arithmetic<T>::value && !__is_wide_int<T>::value, T&>::type;

// Binary operators
template<size_t Bytes, wide_int_s Signed, typename T>
std::common_type_t<wide_int<Bytes,Signed>, T>
constexpr operator*(const wide_int<Bytes,Signed>& num, const T& other) {
    return wide_int<Bytes,Signed>::_impl::operator_star(num, other);
}
template<size_t Bytes, wide_int_s Signed, typename Arithmetic, class = __arithm_not_wide_int<Arithmetic>>
std::common_type_t<wide_int<Bytes,Signed>, Arithmetic>
constexpr operator*(const Arithmetic& other, const wide_int<Bytes,Signed>& num) {
    return wide_int<Bytes,Signed>::_impl::operator_star(num, other);
}

template<size_t Bytes, wide_int_s Signed, typename T>
std::common_type_t<wide_int<Bytes,Signed>, T>
constexpr operator/(const wide_int<Bytes,Signed>& num, const T& other) {
    return wide_int<Bytes,Signed>::_impl::operator_slash(num, other);
}
template<size_t Bytes, wide_int_s Signed, typename Arithmetic, class = __arithm_not_wide_int<Arithmetic>>
std::common_type_t<wide_int<Bytes,Signed>, Arithmetic>
constexpr operator/(const Arithmetic& other, const wide_int<Bytes,Signed>& num) {
    return wide_int<Bytes,Signed>::_impl::operator_slash(wide_int<Bytes,Signed>(other), num);
}

template<size_t Bytes, wide_int_s Signed, typename T>
std::common_type_t<wide_int<Bytes,Signed>, T>
constexpr operator+(const wide_int<Bytes,Signed>& num, const T& other) noexcept(Signed == wide_int_s::Unsigned) {
    return wide_int<Bytes,Signed>::_impl::operator_plus(num, other);
}
template<size_t Bytes, wide_int_s Signed, typename Arithmetic, class = __arithm_not_wide_int<Arithmetic>>
std::common_type_t<wide_int<Bytes,Signed>, Arithmetic>
constexpr operator+(const Arithmetic& other, const wide_int<Bytes,Signed>& num) noexcept(Signed == wide_int_s::Unsigned) {
    return wide_int<Bytes,Signed>::_impl::operator_plus(wide_int<Bytes,Signed>(other), num);
}

template<size_t Bytes, wide_int_s Signed, typename T>
std::common_type_t<wide_int<Bytes,Signed>, T>
constexpr operator-(const wide_int<Bytes,Signed>& num, const T& other) noexcept(Signed == wide_int_s::Unsigned) {
    return wide_int<Bytes,Signed>::_impl::operator_minus(num, other);
}
template<size_t Bytes, wide_int_s Signed, typename Arithmetic, class = __arithm_not_wide_int<Arithmetic>>
std::common_type_t<wide_int<Bytes,Signed>, Arithmetic>
constexpr operator-(const Arithmetic& other, const wide_int<Bytes,Signed>& num) noexcept(Signed == wide_int_s::Unsigned) {
    return wide_int<Bytes,Signed>::_impl::operator_minus(wide_int<Bytes,Signed>(other), num);
}

template<size_t Bytes, wide_int_s Signed, typename T>
std::common_type_t<wide_int<Bytes,Signed>, T>
constexpr operator%(const wide_int<Bytes,Signed>& num, const T& other) {
    static_assert(__is_wide_int<T>::value || std::is_integral<T>::value, "");
    return wide_int<Bytes,Signed>::_impl::operator_percent(num, other);
}
template<size_t Bytes, wide_int_s Signed, typename Integral, class = __arithm_not_wide_int<Integral>>
std::common_type_t<wide_int<Bytes,Signed>, Integral>
constexpr operator%(const Integral& other, const wide_int<Bytes,Signed>& num) {
    static_assert(std::is_integral<Integral>::value, "");
    return wide_int<Bytes,Signed>::_impl::operator_percent(wide_int<Bytes,Signed>(other), num);
}

template<size_t Bytes, wide_int_s Signed, typename T>
std::common_type_t<wide_int<Bytes,Signed>, T>
constexpr operator&(const wide_int<Bytes,Signed>& num, const T& other) noexcept {
    static_assert(__is_wide_int<T>::value || std::is_integral<T>::value, "");
    return wide_int<Bytes,Signed>::_impl::operator_amp(num, other);
}
template<size_t Bytes, wide_int_s Signed, typename Integral, class = __arithm_not_wide_int<Integral>>
std::common_type_t<wide_int<Bytes,Signed>, Integral>
constexpr operator&(const Integral& other, const wide_int<Bytes,Signed>& num) noexcept {
    static_assert(std::is_integral<Integral>::value, "");
    return wide_int<Bytes,Signed>::_impl::operator_amp(wide_int<Bytes,Signed>(other), num);
}

template<size_t Bytes, wide_int_s Signed, typename T>
std::common_type_t<wide_int<Bytes,Signed>, T>
constexpr operator|(const wide_int<Bytes,Signed>& num, const T& other) noexcept {
    static_assert(__is_wide_int<T>::value || std::is_integral<T>::value, "");
    return wide_int<Bytes,Signed>::_impl::operator_pipe(num, other);
}
template<size_t Bytes, wide_int_s Signed, typename Integral, class = __arithm_not_wide_int<Integral>>
std::common_type_t<wide_int<Bytes,Signed>, Integral>
constexpr operator|(const Integral& other, const wide_int<Bytes,Signed>& num) noexcept {
    static_assert(std::is_integral<Integral>::value, "");
    return wide_int<Bytes,Signed>::_impl::operator_pipe(wide_int<Bytes,Signed>(other), num);
}

template<size_t Bytes, wide_int_s Signed, typename T>
std::common_type_t<wide_int<Bytes,Signed>, T>
constexpr operator^(const wide_int<Bytes,Signed>& num, const T& other) noexcept {
    static_assert(__is_wide_int<T>::value || std::is_integral<T>::value, "");
    return wide_int<Bytes,Signed>::_impl::operator_circumflex(num, other);
}
template<size_t Bytes, wide_int_s Signed, typename Integral, class = __arithm_not_wide_int<Integral>>
std::common_type_t<wide_int<Bytes,Signed>, Integral>
constexpr operator^(const Integral& other, const wide_int<Bytes,Signed>& num) noexcept {
    static_assert(std::is_integral<Integral>::value, "");
    return wide_int<Bytes,Signed>::_impl::operator_circumflex(wide_int<Bytes,Signed>(other), num);
}


template<size_t Bytes, wide_int_s Signed>
constexpr wide_int<Bytes,Signed> operator<<(const wide_int<Bytes,Signed>& num, int n) noexcept {
    return wide_int<Bytes,Signed>::_impl::shift_left(num, n);
}
template<size_t Bytes, wide_int_s Signed>
constexpr wide_int<Bytes,Signed> operator>>(const wide_int<Bytes,Signed>& num, int n) noexcept {
    return wide_int<Bytes,Signed>::_impl::shift_right(num, n);
}

template<size_t Bytes, wide_int_s Signed, typename T>
constexpr bool operator<(const wide_int<Bytes,Signed>& num,
                         const T& other) noexcept {
    return wide_int<Bytes,Signed>::_impl::operator_less(num, other);
}
template<size_t Bytes, wide_int_s Signed, typename Arithmetic, class = __arithm_not_wide_int<Arithmetic>>
constexpr bool operator<(const Arithmetic& other,
                         const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::_impl::operator_less(wide_int<Bytes,Signed>(other), num);
}

template<size_t Bytes, wide_int_s Signed, typename T>
constexpr bool operator>(const wide_int<Bytes,Signed>& num,
                         const T& other) noexcept {
    return wide_int<Bytes,Signed>::_impl::operator_more(num, other);
}
template<size_t Bytes, wide_int_s Signed, typename Arithmetic, class = __arithm_not_wide_int<Arithmetic>>
constexpr bool operator>(const Arithmetic& other,
                         const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::_impl::operator_more(wide_int<Bytes,Signed>(other), num);
}

template<size_t Bytes, wide_int_s Signed, typename T>
constexpr bool operator<=(const wide_int<Bytes,Signed>& num,
                          const T& other) noexcept {
    return wide_int<Bytes,Signed>::_impl::operator_less(num, other) ||
           wide_int<Bytes,Signed>::_impl::operator_eq(num, other);
}
template<size_t Bytes, wide_int_s Signed, typename Arithmetic, class = __arithm_not_wide_int<Arithmetic>>
constexpr bool operator<=(const Arithmetic& other,
                          const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::_impl::operator_less(wide_int<Bytes,Signed>(other), num) ||
           wide_int<Bytes,Signed>::_impl::operator_eq(num, other);
}

template<size_t Bytes, wide_int_s Signed, typename T>
constexpr bool operator>=(const wide_int<Bytes,Signed>& num,
                          const T& other) noexcept {
    return wide_int<Bytes,Signed>::_impl::operator_more(num, other) ||
           wide_int<Bytes,Signed>::_impl::operator_eq(num, other);
}
template<size_t Bytes, wide_int_s Signed, typename Arithmetic, class = __arithm_not_wide_int<Arithmetic>>
constexpr bool operator>=(const Arithmetic& other,
                          const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::_impl::operator_more(wide_int<Bytes,Signed>(other), num) ||
           wide_int<Bytes,Signed>::_impl::operator_eq(num, other);
}

template<size_t Bytes, wide_int_s Signed, typename T>
constexpr bool operator==(const wide_int<Bytes,Signed>& num,
                          const T& other) noexcept {
    return wide_int<Bytes,Signed>::_impl::operator_eq(num, other);
}
template<size_t Bytes, wide_int_s Signed, typename Arithmetic, class = __arithm_not_wide_int<Arithmetic>>
constexpr bool operator==(const Arithmetic& other,
                          const wide_int<Bytes,Signed>& num) noexcept {
    return wide_int<Bytes,Signed>::_impl::operator_eq(num, other);
}

template<size_t Bytes, wide_int_s Signed, typename T>
constexpr bool operator!=(const wide_int<Bytes,Signed>& num,
                          const T& other) noexcept {
    return !wide_int<Bytes,Signed>::_impl::operator_eq(num, other);
}
template<size_t Bytes, wide_int_s Signed, typename Arithmetic, class = __arithm_not_wide_int<Arithmetic>>
constexpr bool operator!=(const Arithmetic& other,
                          const wide_int<Bytes,Signed>& num) noexcept {
    return !wide_int<Bytes,Signed>::_impl::operator_eq(num, other);
}



template<size_t Bytes, wide_int_s Signed>
std::string to_string(const wide_int<Bytes,Signed>& n) {
    std::string res;
    if (wide_int<Bytes, Signed>::_impl::operator_eq(n, 0U)) {
        return "0";
    }

    wide_int<Bytes, wide_int_s::Unsigned> t;
    bool is_neg = wide_int<Bytes, Signed>::_impl::is_negative(n);
    if (is_neg) {
        t = wide_int<Bytes, Signed>::_impl::operator_unary_minus(n);
    } else {
        t = n;
    }

    while (!wide_int<Bytes, wide_int_s::Unsigned>::_impl::operator_eq(t, 0U)) {
        res.insert(res.begin(), '0' + char(wide_int<Bytes, wide_int_s::Unsigned>::_impl::operator_percent(t, 10U)));
        t = wide_int<Bytes, wide_int_s::Unsigned>::_impl::operator_slash(t, 10U);
    }

    if (is_neg) {
        res.insert(res.begin(), '-');
    }

    return res;
}

template<size_t Bytes, wide_int_s Signed>
std::wstring to_wstring(const wide_int<Bytes,Signed>& n) {
    std::wstring res;
    if (wide_int<Bytes, Signed>::_impl::operator_eq(n, 0U)) {
        return L"0";
    }

    wide_int<Bytes, wide_int_s::Unsigned> t;
    bool is_neg = wide_int<Bytes, Signed>::_impl::is_negative(n);
    if (is_neg) {
        t = wide_int<Bytes, Signed>::_impl::operator_unary_minus(n);
    } else {
        t = n;
    }

    while (!wide_int<Bytes, wide_int_s::Unsigned>::_impl::operator_eq(t, 0U)) {
        res.insert(res.begin(), '0' + wchar_t(wide_int<Bytes, wide_int_s::Unsigned>::_impl::operator_percent(t, 10U)));
        t = wide_int<Bytes, wide_int_s::Unsigned>::_impl::operator_slash(t, 10U);
    }

    if (is_neg) {
        res.insert(res.begin(), '-');
    }

    return res;
}

template<size_t Bytes, wide_int_s Signed>
std::ostream& operator<<(std::ostream& out, const wide_int<Bytes,Signed>& n) {
    out << to_string(n);
    return out;
}

template<size_t Bytes, wide_int_s Signed>
std::wostream& operator<<(std::wostream& out, const wide_int<Bytes,Signed>& n) {
    out << to_wstring(n);
    return out;
}

template<size_t Bytes, wide_int_s Signed>
std::istream& operator>>(std::istream& in, wide_int<Bytes,Signed>& n) {
    std::string s;
    in >> s;
    n = wide_int<Bytes,Signed>::_impl::from_str(s.c_str());
    return in;
}

template<size_t Bytes, wide_int_s Signed>
std::wistream& operator>>(std::wistream& in, wide_int<Bytes,Signed>& n) {
    std::wstring s;
    in >> s;
    n = wide_int<Bytes,Signed>::_impl::from_str(s.c_str());
    return in;
}


constexpr int128_t operator "" _int128(const char* n) { return int128_t::_impl::from_str(n); }
constexpr int256_t operator "" _int256(const char* n) { return int256_t::_impl::from_str(n); }
constexpr int512_t operator "" _int512(const char* n) { return int512_t::_impl::from_str(n); }
constexpr uint128_t operator "" _uint128(const char* n) { return uint128_t::_impl::from_str(n); }
constexpr uint256_t operator "" _uint256(const char* n) { return uint256_t::_impl::from_str(n); }
constexpr uint512_t operator "" _uint512(const char* n) { return uint512_t::_impl::from_str(n); }

// numeric limits
template<size_t Bytes, wide_int_s Signed>
class numeric_limits<wide_int<Bytes,Signed>> {
public:
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = Signed == wide_int_s::Signed;
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
    static constexpr int digits = CHAR_BIT*Bytes - (Signed == wide_int_s::Signed ? 1 : 0);
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
        if (Signed == wide_int_s::Signed) {
            wide_int<Bytes,wide_int_s::Signed> res {};
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
        res.m_arr[0] = Signed == wide_int_s::Signed
                        ? std::numeric_limits<typename wide_int<Bytes,Signed>::signed_base_type>::max()
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


template<size_t Bytes, wide_int_s Signed>
struct hash<wide_int<Bytes, Signed>> {
    std::size_t operator()(const wide_int<Bytes, Signed>& num) const {
        size_t res = 0;
        for (auto n : num.m_arr) {
            res += n;
        }
        return hash<size_t>()(res);
    }
};

} // namespace std
