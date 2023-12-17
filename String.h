#pragma once

#include <string>
#include <algorithm>

bool isAlpha(std::string& str);
void toUpper(std::string& str);


bool isAlpha(std::string& str)
{
    return std::all_of(str.begin(), str.end(), [](char& c) {
        return isalpha(c);
        });
}

void toUpper(std::string& str)
{
    std::for_each(str.begin(), str.end(), [](char& c)
        {
            c = toupper(c);
        });
}