#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <iostream>
#include <boost/random/uniform_int_distribution.hpp>
#include <random>

namespace Common
{

    std::string generateString(const size_t len);
    const std::vector<std::string> stringSplit(const std::string& msg, const std::string& delimiter);
}




#endif /* COMMON_H */
