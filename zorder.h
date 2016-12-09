//
// Created by Pietro Saccardi on 05/12/2016.
//

#ifndef ZORDER_ZORDER_H
#define ZORDER_ZORDER_H

#include <type_traits>
#include <array>

namespace impl {
    /// @brief Generates 0b1...1 with @tparam n ones
    template <class T, unsigned n>
    using n_ones = std::integral_constant<T, (~static_cast<T>(0) >> (sizeof(T) * 8 - n))>;


    /// @brief Performs `@tparam input | (@tparam input << @tparam width` @tparam repeat times.
    ///@{
    template <class T, T input, unsigned width, unsigned repeat>
    struct lshift_add :
        public lshift_add<T, lshift_add<T, input, width, 1>::value, width, repeat - 1> {
    };
    /// @brief Specialization for 1 repetition, just does the shift-and-add operation.
    template <class T, T input, unsigned width>
    struct lshift_add<T, input, width, 1> : public std::integral_constant<T,
        (input & n_ones<T, width>::value) | (input << (width < sizeof(T) * 8 ? width : 0))> {
    };
    /// @brief Specialization for 0 repetitions (just in case).
    template <class T, T input, unsigned width>
    struct lshift_add<T, input, width, 0> :
        public std::integral_constant<T, static_cast<T>(0)> {
    };
    ///@}


    /// @brief Recursively computes the integral log in base 2 of @tparam arg (as a constexpr).
    ///@{
    template <unsigned arg>
    struct log2 : public std::integral_constant<unsigned, log2<(arg >> 1)>::value + 1> {};
    /// @brief Specialization for `arg=1` to interrupt recursion.
    template <>
    struct log2<1u> : public std::integral_constant<unsigned, 0u> {};
    /// @brief The domain of log(x) is {x>0}
    template <>
    struct log2<0u> {};
    ///@}


    /** @brief Generates the masks necessary for deinterleaving Morton numbers
     * @tparam step 0-based index of the step; this corresponds to a rshift of
     *         `(@tparam dimensions - 1) * 2^@tparam step`.
     * @tparam dimensions how many values are interleaved, default 2.
     *
     * This fills a T type with adjacent patterns of the type 0...01...1 where the number of ones is `2^@tparam step`
     * and the total width is `@tparam dimensions * 2^@tparam step`; i.e. for `@tparam dimensions = 2`, we have
     *   0. 0b...01010101
     *   1. 0b...00110011
     *   2. 0b...00001111
     */
    template <class T, unsigned step, unsigned dimensions = 2u>
    using mask = lshift_add<T, n_ones<T, 1 << step>::value, dimensions * (1 << step), sizeof(T) * 8 / (2 << step)>;


    /** @brief Recursively defines a function that deinterleaves @tparam dimensions-dimensional Morton numbers.
     * @note This extracts only the first bit. Rshift the argument once to get the next bit and so on.
     * @tparam step Step of the deinterleave algorithm. Each of it corresponds to a rshift and a bitwise and with one of
     *         the masks defined in @see masks.
     * @tparam dimensions Number of coordinates packed into the Morton number.
     */
    ///@{
    template <class T, unsigned step, unsigned dimensions>
    struct deinterleave {
        static T work(T input) {
            input = deinterleave<T, step - 1, dimensions>::work(input);
            return (input | (input >> ((dimensions - 1) * (1 << (step - 1))))) & mask<T, step, dimensions>::value;
        }
    };
    /// @brief Specialization for step 0, where there is just a bitwise and
    template <class T, unsigned dimensions>
    struct deinterleave<T, 0u, dimensions> {
        static T work(T input) {
            return input & mask<T, 0u, dimensions>::value;
        }
    };
    ///@}


    /// @brief Helper constexpr which returns the number of steps needed to fully interleave a type @tparam T.
    template <class T, unsigned dimensions>
    using num_steps = std::integral_constant<unsigned, log2<sizeof(T) * 8 / dimensions>::value + 1>;


    /// @brief Helper function which combines @see deinterleave and @see num_steps into a single call.
    template <class T, unsigned dimensions>
    T deinterleave_first(T n) {
        return deinterleave<T, num_steps<T, dimensions>::value - 1, dimensions>::work(n);
    }

    /// @brief Helper class which recursively packs an array by de-interleaving one coordinate at a time
    ///@{
    template <class T, unsigned dimensions, unsigned idx>
    struct deinterleave_array {
        static void work(T &n, std::array<T, dimensions> &tuple) {
            std::get<idx>(tuple) = deinterleave_first<T, dimensions>(n);
            deinterleave_array<T, dimensions, idx - 1>::work(n >>= 1, tuple);
        }
    };
    /// @brief Specialization to stop the recursion.
    template <class T, unsigned dimensions>
    struct deinterleave_array<T, dimensions, 0> {
        static void work(T &n, std::array<T, dimensions> &tuple) {
            std::get<0>(tuple) = deinterleave_first<T, dimensions>(n);
        }
    };
    ///@}
}


/// @brief Extract the first coordinate from a Morton number
template <class T, unsigned dimensions = 2>
T deinterleave_one(T n) {
    return impl::deinterleave_first<T, dimensions>(n);
}

/// @brief Extract @tparam dimensions coordinates from a Morton number
template <class T, unsigned dimensions = 2>
std::array<T, dimensions> deinterleave_all(T n) {
    std::array<T, dimensions> retval;
    impl::deinterleave_array<T, dimensions, dimensions - 1>::work(n, retval);
    return retval;
}

#endif //ZORDER_ZORDER_H
