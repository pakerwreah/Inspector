//
// Created by Paker on 08/11/19.
//

#ifndef INSPECTOR_BASE64_H
#define INSPECTOR_BASE64_H

#include <string>

namespace base64 {
    std::string encode(const std::string &in);
    std::string decode(const std::string &in);
}

#endif //INSPECTOR_BASE64_H
