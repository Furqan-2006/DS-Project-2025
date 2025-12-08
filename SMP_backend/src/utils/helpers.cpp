#include "utils/helpers.hpp"

std::string toLower(const std::string &str)
{
    std::string result = str;
    for (char &c : result)
    {
        if (c >= 'A' && c <= 'Z')
        {
            c = c | 0x20;
        }
    }
    return result;
}
