
#include <Common/Common.hpp>

namespace Common
{
    using namespace boost;

    std::string generateString(const size_t len)
    {
        static const std::string characters(
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "1234567890"
            "!@#$%^&*()"
            "`~-_=+[{]}\\|;:'\",<.>/?");
        std::random_device rng;
        random::uniform_int_distribution<> index_dist(0, characters.size() - 1);

        std::string temp_string;
        size_t char_sz = sizeof(characters);
        temp_string.reserve(len);

        for (size_t i = 0; i < len; i++)
        {
            temp_string += characters[index_dist(rng)];
        }

        return temp_string;
    }
}