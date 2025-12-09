#include "utils/validation.hpp"

// ============================================
// Character-level helper functions
// ============================================

bool UserValidator::isUpperCase(char c)
{
    return c >= 'A' && c <= 'Z';
}

bool UserValidator::isLowerCase(char c)
{
    return c >= 'a' && c <= 'z';
}

bool UserValidator::isDigit(char c)
{
    return c >= '0' && c <= '9';
}

bool UserValidator::isAlphaChar(char c)
{
    return isUpperCase(c) || isLowerCase(c);
}

bool UserValidator::isAlphanumericChar(char c)
{
    return isAlphaChar(c) || isDigit(c);
}

bool UserValidator::isSpecialChar(char c)
{
    const char specialChars[] = "!@#$%^&*()_+-=[]{}|;:',.<>?/~`";
    for (int i = 0; specialChars[i] != '\0'; ++i)
    {
        if (c == specialChars[i])
            return true;
    }
    return false;
}

// ============================================
// Username validation functions
// ============================================

bool UserValidator::usernameNotEmpty(const std::string &uname)
{
    return !uname.empty();
}

bool UserValidator::usernameMinLength(const std::string &uname, int minLen)
{
    int len = 0;
    for (char c : uname)
    {
        ++len;
    }
    return len >= minLen;
}

bool UserValidator::usernameMaxLength(const std::string &uname, int maxLen)
{
    int len = 0;
    for (char c : uname)
    {
        ++len;
    }
    return len <= maxLen;
}

bool UserValidator::usernameHasValidChars(const std::string &uname)
{
    for (char c : uname)
    {
        if (!isAlphanumericChar(c) && c != '_' && c != '.')
        {
            return false;
        }
    }
    return true;
}

bool UserValidator::isValidUsername(const std::string &uname)
{
    return usernameNotEmpty(uname) &&
           usernameMinLength(uname, 3) &&
           usernameMaxLength(uname, 20) &&
           usernameHasValidChars(uname);
}

// ============================================
// Password validation functions
// ============================================

bool UserValidator::passwordNotEmpty(const std::string &pwd)
{
    return !pwd.empty();
}

bool UserValidator::passwordMinLength(const std::string &pwd, int minLen)
{
    int len = 0;
    for (char c : pwd)
    {
        ++len;
    }
    return len >= minLen;
}

bool UserValidator::passwordMaxLength(const std::string &pwd, int maxLen)
{
    int len = 0;
    for (char c : pwd)
    {
        ++len;
    }
    return len <= maxLen;
}

bool UserValidator::passwordHasUppercase(const std::string &pwd)
{
    for (char c : pwd)
    {
        if (isUpperCase(c))
            return true;
    }
    return false;
}

bool UserValidator::passwordHasLowercase(const std::string &pwd)
{
    for (char c : pwd)
    {
        if (isLowerCase(c))
            return true;
    }
    return false;
}

bool UserValidator::passwordHasDigit(const std::string &pwd)
{
    for (char c : pwd)
    {
        if (isDigit(c))
            return true;
    }
    return false;
}

bool UserValidator::passwordHasSpecialChar(const std::string &pwd)
{
    for (char c : pwd)
    {
        if (isSpecialChar(c))
            return true;
    }
    return false;
}

bool UserValidator::isValidPassword(const std::string &pwd)
{
    return passwordNotEmpty(pwd) &&
           passwordMinLength(pwd, 6) &&
           passwordMaxLength(pwd, 50) &&
           passwordHasUppercase(pwd) &&
           passwordHasLowercase(pwd) &&
           passwordHasDigit(pwd);
}

// ============================================
// City validation functions
// ============================================

bool UserValidator::cityNotEmpty(const std::string &city)
{
    return !city.empty();
}

bool UserValidator::cityMinLength(const std::string &city, int minLen)
{
    int len = 0;
    for (char c : city)
    {
        ++len;
    }
    return len >= minLen;
}

bool UserValidator::cityMaxLength(const std::string &city, int maxLen)
{
    int len = 0;
    for (char c : city)
    {
        ++len;
    }
    return len <= maxLen;
}

bool UserValidator::cityHasValidChars(const std::string &city)
{
    for (char c : city)
    {
        if (!isAlphaChar(c) && c != ' ' && c != '-' && c != '\'')
        {
            return false;
        }
    }
    return true;
}

bool UserValidator::isValidCity(const std::string &city)
{
    return cityNotEmpty(city) &&
           cityMinLength(city, 2) &&
           cityMaxLength(city, 50) &&
           cityHasValidChars(city);
}

// ============================================
// General string utilities
// ============================================

bool UserValidator::isAlphanumeric(const std::string &str)
{
    if (str.empty())
        return false;

    for (char c : str)
    {
        if (!isAlphanumericChar(c))
            return false;
    }
    return true;
}

bool UserValidator::isAlpha(const std::string &str)
{
    if (str.empty())
        return false;

    for (char c : str)
    {
        if (!isAlphaChar(c))
            return false;
    }
    return true;
}

bool UserValidator::hasWhitespace(const std::string &str)
{
    for (char c : str)
    {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
            return true;
    }
    return false;
}

int UserValidator::stringLength(const std::string &str)
{
    int len = 0;
    for (char c : str)
    {
        ++len;
    }
    return len;
}