//
// Created by Paker on 17/08/20.
//

#ifndef INSPECTOR_EXPLORER_H
#define INSPECTOR_EXPLORER_H

#include "../../src/Inspector.h"

class Explorer {
private:
    std::string read_file(const std::string &path);

public:
    Explorer(Inspector &inspector, const std::string &root = ".");
};

#endif //INSPECTOR_EXPLORER_H
