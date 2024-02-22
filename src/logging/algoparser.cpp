#include "logging/algoparser.h"

#include <fstream>
#include <sstream>

#include "logging/utils.h"

namespace parsing
{
    std::vector<rubik::Move> parseAlgorithm(std::string filePath)
    {
        std::vector<rubik::Move> algo;

        std::ifstream file(filePath);
        if (!file.is_open())
        {
            std::cerr << "ERROR: The given ALGO file (" << filePath << ") cannot be found." << std::endl;
            return algo;
        }

        std::string token;
        bool errorDetected = false;

        int turn = 0;
        int face = 0;

        while ((file >> token) && !errorDetected)
        {
            // token = trim(token);
            if (token.length() == 0 || token.length() >= 3)
            {
                continue;
            }

            switch (std::tolower(token[0]))
            {
            case 'u':
                face = 0;
                break;
            case 'd':
                face = 1;
                break;
            case 'f':
                face = 2;
                break;
            case 'b':
                face = 3;
                break;
            case 'l':
                face = 4;
                break;
            case 'r':
                face = 5;
                break;

            default:
                std::cerr << "ERROR: Move " << token << " parsed from " << filePath << " is not valid." << std::endl;
                errorDetected = true;
                break;
            }

            if (token.length() == 1)
            {
                turn = 1;
            }
            else if (token[1] == '2')
            {
                turn = 2;
            }
            else if (token[1] == '\'')
            {
                turn = 3;
            }
            else
            {
                std::cerr << "ERROR: Move " << token << " parsed from " << filePath << " is not valid." << std::endl;
                errorDetected = true;
                break;
            }

            algo.push_back(rubik::Move(face, turn));
        }

        file.close();
        return algo;
    }

}