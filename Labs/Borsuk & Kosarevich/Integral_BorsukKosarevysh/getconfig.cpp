#include "getconfig.h"

void Configs::get_configs(const std::string &filename) {
    std::string line, key, value;
    std::ifstream file(filename);
    if (file.is_open()) {
        while (std::getline(file, line)) {
            auto begin = line.find_first_not_of(" \t");
            if (begin == std::string::npos) continue;

            if (line[begin] == '#') continue;

            auto end = line.find(DELIMITER, begin);
            key = line.substr(begin, end - begin);
            key.erase(key.find_last_not_of(" \t") + 1);
            if (key.empty()) continue;

            begin = line.find_first_not_of(" \t", end + 1);
            end   = line.find_last_not_of(" \t") + 1;
            value = line.substr(begin, end - begin);

            configs.insert(std::pair<std::string, std::string>(key, value));
        }
        file.close();
    }
    else {
        throw "Invalid file: " + filename;
    }
}

Configs::Configs(const std::string &filename) {
    get_configs(filename);
}

std::string Configs::find(const std::string &param) {
    auto found_val = configs.find(param);
    if (found_val == configs.end()) {
        throw "No such key: " + param;
    }
    return found_val->second;
}