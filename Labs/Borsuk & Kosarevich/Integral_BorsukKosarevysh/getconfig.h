#ifndef INTEGRAL_GETCONFIG_H
#define INTEGRAL_GETCONFIG_H

#include <iostream>
#include <utility>
#include <string>
#include <sstream>
#include <map>
#include <fstream>

#define DELIMITER    '='

class Configs {
private:
    std::map<std::string, std::string> configs;

    void get_configs(const std::string &filename);

    std::string find(const std::string &param);
public:
    Configs(const std::string &filename);

    template<class T>
    T get_value(const std::string &param);

    template<class T>
    void get_array(const std::string &param, T* arr, int length);

    template <class T>
    std::pair<T, T> get_pair(const std::string &param);
};

template<class T>
T Configs::get_value(const std::string &param) {
    std::string result = find(param);
    std::stringstream stream(result);
    T val;
    if (stream >> val ) {
        return val;
    }
    throw "Invalid arguments for: " + param;
}

template<class T>
void Configs::get_array(const std::string &param, T* arr, int length) {
    std::string result = find(param);
    std::stringstream stream(result);
    int i = 0;
    while (stream >> arr[i]) {
        i++;
    }
    if (i != length) {
        throw "Invalid arguments for: " + param;
    }
}

template <class T>
std::pair<T, T> Configs::get_pair(const std::string &param) {
    T arr_pair[2];
    get_array<T>(param, arr_pair, 2);
    return std::make_pair(arr_pair[0], arr_pair[1]);
}

#endif //INTEGRAL_GETCONFIG_H
