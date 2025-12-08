#include "utils/fileIO.hpp"

void FileIO::writeJSON(const std::string &path, const nlohmann::json &data)
{
    std::ofstream out(path);
    if (!out.is_open())
    {
        throw std::runtime_error("Unable to open file for writing: " + path);
    }
    out << std::setw(4) << data;
}
void FileIO::appendJSON(const std::string &path, const nlohmann::json &d)
{
    nlohmann::json data;

    std::ifstream in(path);
    if (in.is_open())
        in >> data;

    if (!data.is_array())
    {
        data = nlohmann::json::array();
    }

    data.push_back(d);

    writeJSON(path, data);
}

nlohmann::json FileIO::readJSON(const std::string &path)
{
    std::ifstream in(path);
    if (!in.is_open())
    {
        throw std::runtime_error("Unable to open file for reading: " + path);
    }
    nlohmann::json data;
    in >> data;

    return data;
}