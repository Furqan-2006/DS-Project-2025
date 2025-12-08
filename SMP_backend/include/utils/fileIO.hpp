#pragma once

#include <fstream>
#include <string>
#include <iomanip>
#include "nlohmann/json.hpp"

class FileIO
{
    static void writeJSON(const std::string &path, const nlohmann::json &data);

public:
    static void appendJSON(const std::string &path, const nlohmann::json &data);
    static nlohmann::json readJSON(const std::string &path);
};