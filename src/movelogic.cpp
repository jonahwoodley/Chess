#include "movelogic.hpp"
#include "chess.hpp"

MoveLogic::MoveLogic(Chess* chess)
    : chess(chess) {}

bool MoveLogic::isWithinBounds(int row, int col) const {
    int boardSize = chess->getBoardSize();
    return row >= 0 && row < boardSize && col >= 0 && col < boardSize;
}

bool MoveLogic::isSquareEmpty(int row, int col) const {
    const auto& board = chess->getBoard();
    return board[row][col].type == PieceType::EMPTY;
}

bool MoveLogic::isOpponentPiece(int row, int col, PieceColour colour) const {
    const auto& board = chess->getBoard();
    return board[row][col].colour != colour && board[row][col].type != PieceType::EMPTY;
}

void MoveLogic::tryAddMove(int row, int col, std::vector<Position>& moves) const {
    if (isWithinBounds(row, col) && isSquareEmpty(row, col)) {
        moves.push_back({row, col});
    }
}

void MoveLogic::tryAddCaptureMove(int row, int col, PieceColour colour, std::vector<Position>& moves) const {
    if (isWithinBounds(row, col) && isOpponentPiece(row, col, colour)) {
        moves.push_back({row, col});
    }
}

std::vector<Position> MoveLogic::processPieceMoves(const Piece& piece, int row, int col) {
    std::vector<Position> moves;

    switch (piece.type) {
    case PieceType::PAWN:
        moves = getPawnMoves(piece, row, col);
        break;
    case PieceType::KNIGHT:
        moves = getKnightMoves(piece, row, col);
        break;
    case PieceType::BISHOP:
        moves = getBishopMoves(piece, row, col);
        break;
    case PieceType::ROOK:
        moves = getRookMoves(piece, row, col);
        break;
    case PieceType::QUEEN:
        moves = getQueenMoves(piece, row, col);
        break;
    case PieceType::KING:
        moves = getKingMoves(piece, row, col);
        break;
    }

    return moves;
}

std::vector<Position> MoveLogic::getPawnMoves(const Piece& pawn, int row, int col) {
    std::vector<Position> moves;

    int direction = (pawn.colour == PieceColour::WHITE) ? -1 : 1;
    int startRow = (pawn.colour == PieceColour::WHITE) ? 6  // white pawn start row
                                                       : 1; // black pawn start row

    tryAddMove(row + direction, col, moves);

    if (row == startRow) {
        int doubleRow = row + 2 * direction;
        if (isSquareEmpty(row + direction, col)) {
            tryAddMove(doubleRow, col, moves);
        }
    }

    tryAddCaptureMove(row + direction, col - 1, pawn.colour, moves);
    tryAddCaptureMove(row + direction, col + 1, pawn.colour, moves);

    return moves;
}

std::vector<Position> MoveLogic::getKnightMoves(const Piece& knight, int row, int col) {
    std::vector<Position> moves;

    const std::vector<std::pair<int, int>> knightMoveOffsets = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1},
                                                                {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};

    for (const auto& offset : knightMoveOffsets) {
        int newRow = row + offset.first;
        int newCol = col + offset.second;

        if (isWithinBounds(newRow, newCol)) {
            if (isSquareEmpty(newRow, newCol) || isOpponentPiece(newRow, newCol, knight.colour)) {
                moves.push_back({newRow, newCol});
            }
        }
    }

    return moves;
}

std::vector<Position> MoveLogic::getBishopMoves(const Piece& bishop, int row, int col) {
    std::vector<Position> moves;

    const std::vector<std::pair<int, int>> bishopDirections = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

    for (const auto& direction : bishopDirections) {
        int newRow = row;
        int newCol = col;

        while (true) {
            newRow += direction.first;
            newCol += direction.second;

            if (!isWithinBounds(newRow, newCol)) {
                break;
            }

            if (isSquareEmpty(newRow, newCol)) {
                moves.push_back({newRow, newCol});
            } else if (isOpponentPiece(newRow, newCol, bishop.colour)) {
                moves.push_back({newRow, newCol});
                break;
            } else {
                break;
            }
        }
    }

    return moves;
}

std::vector<Position> MoveLogic::getRookMoves(const Piece& rook, int row, int col) {
    std::vector<Position> moves;

    const std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (const auto& direction : directions) {
        int newRow = row;
        int newCol = col;

        while (true) {
            newRow += direction.first;
            newCol += direction.second;

            if (!isWithinBounds(newRow, newCol)) {
                break;
            }

            if (isSquareEmpty(newRow, newCol)) {
                moves.push_back({newRow, newCol});
            } else if (isOpponentPiece(newRow, newCol, rook.colour)) {
                moves.push_back({newRow, newCol});
                break;
            } else {
                break;
            }
        }
    }

    return moves;
}

std::vector<Position> MoveLogic::getQueenMoves(const Piece& queen, int row, int col) {
    std::vector<Position> moves;

    const std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

    for (const auto& direction : directions) {
        int newRow = row;
        int newCol = col;

        while (true) {
            newRow += direction.first;
            newCol += direction.second;

            if (!isWithinBounds(newRow, newCol)) {
                break;
            }

            if (isSquareEmpty(newRow, newCol)) {
                moves.push_back({newRow, newCol});
            } else if (isOpponentPiece(newRow, newCol, queen.colour)) {
                moves.push_back({newRow, newCol});
                break;
            } else {
                break;
            }
        }
    }

    return moves;
}

std::vector<Position> MoveLogic::getKingMoves(const Piece& king, int row, int col) {
    std::vector<Position> moves;

    for (int r = row - 1; r <= row + 1; ++r) {
        for (int c = col - 1; c <= col + 1; ++c) {
            if ((r == row && c == col) || !isWithinBounds(r, c)) {
                continue;
            }
            if (isSquareEmpty(r, c) || isOpponentPiece(r, c, king.colour)) {
                moves.push_back({r, c});
            }
        }
    }

    return moves;
}
