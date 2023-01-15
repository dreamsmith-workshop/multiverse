
template<typename CharT, std::size_t N>
constexpr mltvrs::basic_string_literal<CharT, N>::basic_string_literal(
    const CharT (&arr)[N + 1]) noexcept
    : basic_string_literal{arr, std::make_index_sequence<N>{}}
{
}

template<typename CharT, std::size_t N>
template<std::size_t... I>
constexpr mltvrs::basic_string_literal<CharT, N>::basic_string_literal(
    const CharT (&arr)[N + 1],
    std::index_sequence<I...> /*unused*/) noexcept
    : basic_string_literal{arr[I]...}
{
}

template<typename CharT, std::size_t N>
template<typename... C>
constexpr mltvrs::basic_string_literal<CharT, N>::basic_string_literal(C... c) noexcept
    : value{c..., CharT{}}
{
}

template<typename CharT, std::size_t N>
[[nodiscard]] constexpr auto mltvrs::basic_string_literal<CharT, N>::cbegin() const noexcept
    -> const_iterator
{
    return data();
}

template<typename CharT, std::size_t N>
[[nodiscard]] constexpr auto mltvrs::basic_string_literal<CharT, N>::end() const noexcept
    -> const_iterator
{
    return data() + N;
}

template<typename CharT, std::size_t N>
[[nodiscard]] constexpr auto mltvrs::basic_string_literal<CharT, N>::cend() const noexcept
    -> const_iterator
{
    return data() + N;
}
