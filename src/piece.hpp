#ifndef PIECE_HPP
#define PIECE_HPP

#include <string>

enum class PieceType {
    EMPTY = 0,
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING,
};

enum class PieceColour {
    NONE = 0,
    BLACK,
    WHITE,
};

class Piece {
public:
    PieceType type;
    PieceColour colour;
    bool active;

public:
    Piece()
        : type(PieceType::EMPTY)
        , colour(PieceColour::NONE)
        , active(false) {}

    Piece(PieceType type, PieceColour colour)
        : type(type)
        , colour(colour)
        , active(true) {}
};

#endif