#pragma once

#include <vector>

#include <cube/move.h>

namespace parsing
{
    std::vector<rubik::Move> parseAlgorithm(std::string filePath);
}