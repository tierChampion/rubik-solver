#include "cube/move.h"

namespace rubik
{
    Move::Move() { _type = (MoveType)0; }

    Move::Move(int value)
    {
        _type = (MoveType)value;
    }

    Move::Move(int face, int turns)
    {
        _type = (MoveType)(turns - 1 + face * MOVES_PER_FACE);
    }

    Move::Move(MoveType t)
    {
        _type = t;
    }

    uint8_t Move::code() const
    {
        return _type;
    }

    /**
     * @return the number of turns of the move.
     */
    int Move::getTurns() const
    {
        return _type % MOVES_PER_FACE + 1;
    }

    /**
     * @return the face that is affected by the move.
     */
    int Move::getFace() const
    {
        return _type / MOVES_PER_FACE;
    }

    /**
     * @return the move that negates this move.
     */
    Move Move::inverse() const
    {
        return Move(_type + 2 - 2 * (_type % MOVES_PER_FACE));
    }

    MoveAxis Move::getAxis() const
    {

        return MoveAxis(getFace() / 2);
    }

    /**
     * Get the rotation modification of the model.
     * @return the axis to turn around and the angle to turn by
     */
    glm::vec4 Move::toAxisAngle() const
    {
        glm::vec3 axis(0, 0, 0);

        int face = getFace();
        int turns = getTurns();

        switch (face)
        {
        case 0:
            axis[1] = 1;
            break;
        case 1:
            axis[1] = -1;
            break;
        case 2:
            axis[2] = 1;
            break;
        case 3:
            axis[2] = -1;
            break;
        case 4:
            axis[0] = -1;
            break;
        case 5:
            axis[0] = 1;
            break;
        }

        float angle = (turns - (4 * (turns == 3))) * M_PI / 2.0f;

        return glm::vec4(axis, -angle);
    }

    /**
     * @param normals - normals of the sides the cubie touches
     * @return if the piece is part of the side that turns
     */
    bool Move::affectsPiece(const std::vector<glm::vec3> normals) const
    {
        int face = getFace();

        for (int i = 0; i < normals.size(); i++)
        {
            glm::vec3 normal = normals[i];

            if ((face == 0 && normal[1] > 0) ||
                (face == 1 && normal[1] < 0) ||
                (face == 2 && normal[2] > 0) ||
                (face == 3 && normal[2] < 0) ||
                (face == 4 && normal[0] < 0) ||
                (face == 5 && normal[0] > 0))
                return true;
        }

        return false;
    }

    /**
     * Shows the move in the standard rubik's cube format.
     * @param s - stream to later print
     * @param move - move to convert to standard format
     */
    std::ostream &operator<<(std::ostream &s, const Move &move)
    {
        int face = move.getFace();
        int turns = move.getTurns();

        char faceName = 'U' * (face == 0) +
                        'D' * (face == 1) +
                        'F' * (face == 2) +
                        'B' * (face == 3) +
                        'L' * (face == 4) +
                        'R' * (face == 5);

        char turnName = '\'' * (turns == 3) +
                        '2' * (turns == 2);

        return s << faceName << turnName;
    }
}