#pragma once

#include <type_traits>
#include <utility>

namespace mltvrs {

    /**
     * @brief String literal type.
     *
     * @tparam CharT The string character type.
     * @tparam N     The number of characters in the string.
     */
    template<typename CharT, std::size_t N>
    struct basic_string_literal
    {
        private:
            using storage_type = CharT[N + 1];

        public:
            using value_type      = CharT;             //!< Character type.
            using size_type       = std::size_t;       //!< String length type.
            using const_reference = const value_type&; //!< Immutable character reference.
            using reference       = const_reference;   //!< Mutable character reference.
            using const_pointer   = const value_type*; //!< Immutable character pointer.
            using pointer         = const_pointer;     //!< Mutable character pointer.
            using iterator        = pointer;           //!< Mutable character iterator.
            using const_iterator  = const_pointer;     //!< Immutable character iterator.

            /**
             * @brief Default-construct zero-length string literal.
             */
            consteval basic_string_literal() noexcept
                requires(N == 0)
            = default;

            /**
             * @brief Construct string literal from constant character array.
             *
             * @param arr A constant character array to initialize from.
             */
            consteval basic_string_literal(const CharT (&arr)[N + 1]) noexcept;

            /**
             * @name Literal Access
             *
             * @brief Access as a string literal.
             *
             * @return Returns the represented string literal.
             *
             * @{
             */
            [[nodiscard]] constexpr operator const storage_type&() const noexcept { return value; }
            [[nodiscard]] constexpr auto data() const noexcept -> const_pointer { return value; }
            [[nodiscard]] constexpr auto c_str() const noexcept -> const_pointer { return value; }
            //! @}

            /**
             * @brief Obtain the string literal length, including the null terminator.
             *
             * @return Returns the string literal length.
             */
            [[nodiscard]] constexpr auto size() const noexcept -> size_type { return N; }

            /**
             * @name Iterators
             *
             * @brief Obtain iterators into the string literal.
             *
             * @return Returns the requested iterator.
             *
             * @{
             */
            [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator { return data(); }
            [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator;
            [[nodiscard]] constexpr auto end() const noexcept -> const_iterator;
            [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator;
            //! @}

            const storage_type value;

        private:
            template<std::size_t... I>
            consteval basic_string_literal(
                const CharT (&arr)[N + 1],
                std::index_sequence<I...> /*unused*/) noexcept;
            template<typename... C>
            consteval basic_string_literal(C... c) noexcept;
    };

    /**
     * @brief Deduction from string literal.
     *
     * @tparam CharT The string character type.
     * @tparam N     The string literal length.
     */
    template<typename CharT, std::size_t N>
    basic_string_literal(const CharT (&)[N])
        -> basic_string_literal<std::remove_cv_t<CharT>, N - 1>;

    /**
     * @brief Convenience aliases for common character types.
     *
     * @tparam N The number of characters in the literal.
     *
     * @{
     */
    template<std::size_t N>
    using string_literal = basic_string_literal<char, N>;
    template<std::size_t N>
    using wstring_literal = basic_string_literal<wchar_t, N>;
    template<std::size_t N>
    using u8string_literal = basic_string_literal<char8_t, N>;
    template<std::size_t N>
    using u16string_literal = basic_string_literal<char16_t, N>;
    template<std::size_t N>
    using u32string_literal = basic_string_literal<char32_t, N>;
    //! @}

} // namespace mltvrs

#include <mltvrs/ipp/string_literal.ipp>
