#pragma once
#ifndef UTILITY_H
#define UTILITY_H

#include <iomanip>
#include <sstream>

std::string floatToString (float f)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << f;
    return ss.str();
}

#endif // UTILITY_H