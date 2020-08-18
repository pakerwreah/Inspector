//
// Created by Paker on 17/08/20.
//

#include <sstream>
#include <fstream>
#include <filesystem>

#include "../../src/Inspector.h"

#ifndef INSPECTOR_EXPLORER_H
#define INSPECTOR_EXPLORER_H

class Explorer {
private:
    string root;

    string read_file(const string &path) {
        ifstream file(path);
        ostringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    string relative_path(const string &path) {
        return filesystem::path(root + (path.empty() ? "." : path));
    }

public:
    Explorer(Inspector &inspector, const string &root) {
        this->root = root;

        inspector.addLivePlugin("explorer", "Explorer", [this] {
            return read_file("../ext/explorer/explorer.html");
        });

        inspector.addPluginAPI("GET", "filesystem/list", [this](const Params &params) {
            const auto entries = filesystem::directory_iterator(relative_path(params.at("path")));

            json out = json::array();

            for (const auto &entry : entries) {
                string name = entry.path().filename();
                string type = entry.is_regular_file() ? "file" : "folder";
                out.push_back({
                                      {"type", type},
                                      {"name", name},
                                      {"size", entry.is_regular_file() ? entry.file_size() : 0}
                              });
            }

            return out.dump();
        });

        inspector.addPluginAPI("GET", "filesystem/open", [this](const Params &params) {
            auto path = relative_path(params.at("path"));
            return read_file(path);
        });
    }
};

#endif //INSPECTOR_EXPLORER_H
