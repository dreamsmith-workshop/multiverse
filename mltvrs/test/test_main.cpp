#include <catch2/catch_session.hpp>
#include <catch2/catch_tostring.hpp>

auto Catch::StringMaker<std::string_view>::convert(std::string_view arg) -> std::string
{
    return std::string{arg};
}

int main(int argc, char** argv)
{
    return Catch::Session().run(argc, argv);
}
