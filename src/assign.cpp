#include "../include/assign.h"

std::string assign(std::string str, int n) {
    int blank_count = n - str.length();

    if (blank_count > 0) {
        for (int i = 0; i < blank_count; i++) str += ' ';
    }

    return str;
}