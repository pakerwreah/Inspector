#ifndef INSPECTOR_READ_FILE_H
#define INSPECTOR_READ_FILE_H

#include <sstream>
#include <fstream>

inline std::string read_file(const std::string &path) {
    std::ifstream file(path);
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

#endif //INSPECTOR_READ_FILE_H
