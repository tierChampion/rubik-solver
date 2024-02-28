#pragma once

#include <vector>
#include <queue>

#include <cube/move.h>

namespace parsing
{
    std::vector<rubik::Move> parseAlgorithm(std::string filePath);
    void saveProblem(std::string filePath, std::queue<rubik::Move> solution);
}