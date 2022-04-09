
#include <Common/Common.hpp>

namespace Common
{
    std::string generateString(const size_t len)
    {
        static const char characters[75] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "!@#$%^&*()_+";
        std::string temp_string;
        size_t char_sz = sizeof(characters);
        temp_string.reserve(len);

        for (size_t i = 0; i < len; i++)
        {
            temp_string += characters[rand() % char_sz - 1];
        }

        return temp_string;
    }
}