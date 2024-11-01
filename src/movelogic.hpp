#ifndef MOVELOGIC_HPP
#define MOVELOGIC_HPP

#include <vector>

#include "piece.hpp"

class MoveLogic {
public:
    explicit MoveLogic(class Chess* chess);
    std::vector<struct Position> processPieceMoves(const Piece& piece, int row, int col);

private:
    Chess* chess;

    bool isWithinBounds(int row, int col) const;
    bool isSquareEmpty(int row, int col) const;
    bool isOpponentPiece(int row, int col, PieceColour colour) const;
    void tryAddMove(int row, int col, std::vector<struct Position>& moves) const;
    void tryAddCaptureMove(int row, int col, PieceColour colour, std::vector<struct Position>& moves) const;
    std::vector<struct Position> getPawnMoves(const Piece& pawn, int row, int col);
    std::vector<struct Position> getKnightMoves(const Piece& knight, int row, int col);
    std::vector<struct Position> getBishopMoves(const Piece& bishop, int row, int col);
    std::vector<struct Position> getRookMoves(const Piece& rook, int row, int col);
    std::vector<struct Position> getQueenMoves(const Piece& queen, int row, int col);
    std::vector<struct Position> getKingMoves(const Piece& king, int row, int col);
};

#endif
