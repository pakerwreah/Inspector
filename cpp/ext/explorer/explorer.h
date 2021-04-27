//
// Created by Paker on 17/08/20.
//

#pragma once

#include "Inspector.h"

struct Explorer {
    explicit Explorer(Inspector &inspector, const std::string &root = ".");
};
