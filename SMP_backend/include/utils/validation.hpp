#pragma once

#include <string>

class UserValidator
{
public:
    // Username validation
    static bool isValidUsername(const std::string &uname);
    static bool usernameMinLength(const std::string &uname, int minLen = 3);
    static bool usernameMaxLength(const std::string &uname, int maxLen = 20);
    static bool usernameHasValidChars(const std::string &uname);
    static bool usernameNotEmpty(const std::string &uname);

    // Password validation
    static bool isValidPassword(const std::string &pwd);
    static bool passwordMinLength(const std::string &pwd, int minLen = 6);
    static bool passwordMaxLength(const std::string &pwd, int maxLen = 50);
    static bool passwordHasUppercase(const std::string &pwd);
    static bool passwordHasLowercase(const std::string &pwd);
    static bool passwordHasDigit(const std::string &pwd);
    static bool passwordHasSpecialChar(const std::string &pwd);
    static bool passwordNotEmpty(const std::string &pwd);

    // City validation
    static bool isValidCity(const std::string &city);
    static bool cityMinLength(const std::string &city, int minLen = 2);
    static bool cityMaxLength(const std::string &city, int maxLen = 50);
    static bool cityHasValidChars(const std::string &city);
    static bool cityNotEmpty(const std::string &city);

    // General string utilities
    static bool isAlphanumeric(const std::string &str);
    static bool isAlpha(const std::string &str);
    static bool hasWhitespace(const std::string &str);
    static int stringLength(const std::string &str);

private:
    static bool isUpperCase(char c);
    static bool isLowerCase(char c);
    static bool isDigit(char c);
    static bool isAlphaChar(char c);
    static bool isAlphanumericChar(char c);
    static bool isSpecialChar(char c);
};