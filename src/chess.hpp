#ifndef CHESS_HPP
#define CHESS_HPP

#include <array>
#include <filesystem>
#include <memory>
#include <unordered_map>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "piece.hpp"
#include "ui.hpp"

struct Position {
    int row, col;

    Position()
        : row(0)
        , col(0) {}

    Position(int r, int c)
        : row(r)
        , col(c) {}
};

struct GameSpecification {
    SDL_Color chessTileLightColour = {222, 184, 135, 255};
    SDL_Color chessTileDarkColour = {139, 69, 19, 255};
    SDL_Color windowBackgroundColour = {18, 18, 18, 255};
    int tileSize = 90;
    int boardSize = 8;
    int targetFrameRate = 60;
};

class Chess {
public:
    Chess(const GameSpecification& spec = GameSpecification());
    ~Chess();
    void run();

public:
    std::string getTextureKey(const Piece& piece) const;
    SDL_Texture* getTexture(const std::string& textureKey) const;

public:
    const std::vector<std::vector<Piece>>& getBoard() const {
        return m_board;
    }
    int getBoardSize() const {
        return m_specification.boardSize;
    }

    SDL_Window* getWindow() const {
        return m_window;
    }

    SDL_Renderer* getRenderer() const {
        return m_renderer;
    }

    PieceColour getCurrentTurn() const {
        return m_currentTurn;
    }

    const std::array<Piece, 16>& getTakenWhitePieces() const {
        return m_takenWhitePieces;
    }
    const std::array<Piece, 16>& getTakenBlackPieces() const {
        return m_takenBlackPieces;
    }

private:
    void drawBoard();
    void setupBoard();
    void loadPieceTextures();
    void loadSounds();
    void onBoardClick(int mouseX, int mouseY);
    bool isValidBoardPosition(int row, int col) const;
    bool isMoveInPossibleMoves(int row, int col) const;
    void checkPawnPromotion(int targetRow, int targetCol);
    void movePiece(int targetRow, int targetCol);
    void toggleTurn();
    bool isCheckmate(PieceColour colour);
    bool isMoveIllegal(const Position& from, const Position& to, PieceColour colour);
    bool isInCheck(PieceColour colour);
    void drawTile(int col, int row, SDL_Color colour);
    void playSound(const std::string& soundName);
    void drawPiece(int col, int row);
    void drawPossibleMoves();
    Position findKing(PieceColour colour);
    SDL_Color getTileColour(int row, int col);
    SDL_Texture* loadTexture(const std::filesystem::path& filePath);

private:
    int m_whiteCaptureCount = 0;
    int m_blackCaptureCount = 0;
    bool m_boardClickEnabled;
    bool m_gameRunning;

    Position m_selectedPiecePosition;
    PieceColour m_currentTurn;
    GameSpecification m_specification;

    std::vector<Position> m_possibleMoves;
    std::vector<std::vector<Piece>> m_board;
    std::unordered_map<std::string, Mix_Chunk*> m_sounds;
    std::unordered_map<std::string, SDL_Texture*> m_textures;
    std::array<Piece, 16> m_takenWhitePieces;
    std::array<Piece, 16> m_takenBlackPieces;
    std::unique_ptr<class MoveLogic> m_moveLogic;
    std::unique_ptr<class UI> m_ui;

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
};

#endif