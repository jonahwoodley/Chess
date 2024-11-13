#include <iostream>
#include <string>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "chess.hpp"
#include "movelogic.hpp"
#include "ui.hpp"

#define ENDS_WITH(str, suffix) \
    (str.size() >= sizeof(suffix) - 1 && str.compare(str.size() - sizeof(suffix) + 1, sizeof(suffix) - 1, suffix) == 0)

Chess::Chess(const GameSpecification& spec)
    : m_window(nullptr)
    , m_renderer(nullptr)
    , m_specification(spec)
    , m_moveLogic(std::make_unique<MoveLogic>(this))
    , m_ui(nullptr)
    , m_selectedPiecePosition(-1, -1)
    , m_boardClickEnabled(true)
    , m_gameRunning(true)
    , m_currentTurn(PieceColour::WHITE) {

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL could not be initialized. SDL_Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    m_window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
    if (!m_window) {
        std::cerr << "Window could not be created. SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_renderer) {
        std::cerr << "Renderer could not be created. SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        exit(1);
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        std::cerr << "SDL_mixer could not initialized. Mix_Error: " << Mix_GetError() << std::endl;
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        exit(1);
    }

    // init m_ui now that m_window and m_renderer are not null.
    m_ui = std::make_unique<UI>(this);

    loadPieceTextures();
    loadSounds();
    setupBoard();
}

void Chess::loadPieceTextures() {
    std::filesystem::path texturesPath = "resources/images/";

    for (const auto& entry : std::filesystem::directory_iterator(texturesPath)) {
        if (entry.is_regular_file()) {
            std::string fileName = entry.path().filename().string();

            if (fileName.find("-") != std::string::npos && ENDS_WITH(fileName, ".png")) {
                std::string key = fileName.substr(0, fileName.find(".png"));
                m_textures[key] = loadTexture(entry.path());
            }
        }
    }
}

void Chess::loadSounds() {
    std::filesystem::path soundsPath = "resources/sounds/";

    for (const auto& entry : std::filesystem::directory_iterator(soundsPath)) {
        if (entry.is_regular_file()) {
            std::string fileName = entry.path().filename().string();

            if (ENDS_WITH(fileName, ".mp3") || ENDS_WITH(fileName, ".wav")) {
                std::string key = fileName.substr(0, fileName.find_last_of('.'));
                Mix_Chunk* sound = Mix_LoadWAV(entry.path().string().c_str());

                if (!sound) {
                    std::cerr << "Failed to load sound: " << fileName << ". Mix_Error: " << Mix_GetError() << std::endl;
                    continue;
                }

                m_sounds[key] = sound;
            }
        }
    }
}

void Chess::setupBoard() {
    m_board.clear();
    m_board.resize(m_specification.boardSize, std::vector<Piece>(m_specification.boardSize));

    m_takenWhitePieces = std::array<Piece, 16>{};
    m_takenBlackPieces = std::array<Piece, 16>{};

    m_currentTurn = PieceColour::WHITE;

    m_board[0][0] = Piece(PieceType::ROOK, PieceColour::BLACK);
    m_board[0][1] = Piece(PieceType::KNIGHT, PieceColour::BLACK);
    m_board[0][2] = Piece(PieceType::BISHOP, PieceColour::BLACK);
    m_board[0][3] = Piece(PieceType::QUEEN, PieceColour::BLACK);
    m_board[0][4] = Piece(PieceType::KING, PieceColour::BLACK);
    m_board[0][5] = Piece(PieceType::BISHOP, PieceColour::BLACK);
    m_board[0][6] = Piece(PieceType::KNIGHT, PieceColour::BLACK);
    m_board[0][7] = Piece(PieceType::ROOK, PieceColour::BLACK);

    for (int i = 0; i < 8; ++i) {
        m_board[1][i] = Piece(PieceType::PAWN, PieceColour::BLACK);
        m_board[6][i] = Piece(PieceType::PAWN, PieceColour::WHITE);
    }

    m_board[7][0] = Piece(PieceType::ROOK, PieceColour::WHITE);
    m_board[7][1] = Piece(PieceType::KNIGHT, PieceColour::WHITE);
    m_board[7][2] = Piece(PieceType::BISHOP, PieceColour::WHITE);
    m_board[7][3] = Piece(PieceType::QUEEN, PieceColour::WHITE);
    m_board[7][4] = Piece(PieceType::KING, PieceColour::WHITE);
    m_board[7][5] = Piece(PieceType::BISHOP, PieceColour::WHITE);
    m_board[7][6] = Piece(PieceType::KNIGHT, PieceColour::WHITE);
    m_board[7][7] = Piece(PieceType::ROOK, PieceColour::WHITE);
}

void Chess::run() {
    SDL_Event event;

    const int frameDelay = 1000 / m_specification.targetFrameRate;
    Uint32 frameStart;
    int frameTime;

    while (m_gameRunning) {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                m_gameRunning = false;
                break;
            }

            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    onBoardClick(event.button.x, event.button.y);
                }
            }
        }

        SDL_SetRenderDrawColor(m_renderer, m_specification.windowBackgroundColour.r, m_specification.windowBackgroundColour.g,
                               m_specification.windowBackgroundColour.b, m_specification.windowBackgroundColour.a);
        SDL_RenderClear(m_renderer);

        drawBoard();

        if (!m_possibleMoves.empty()) {
            drawPossibleMoves();
        }

        m_ui->renderInterfaces();

        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), m_renderer);
        SDL_RenderPresent(m_renderer);

        // limit FPS
        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void Chess::playSound(const std::string& soundName) {
    auto sound = m_sounds.find(soundName);
    if (sound != m_sounds.end()) {
        Mix_PlayChannel(-1, sound->second, 0);
    }
}

SDL_Texture* Chess::loadTexture(const std::filesystem::path& filePath) {
    SDL_Texture* texture = IMG_LoadTexture(m_renderer, filePath.string().c_str());
    if (!texture) {
        std::cerr << "Failed to load texture! SDL_image Error: " << IMG_GetError() << std::endl;
        exit(1);
    }
    return texture;
}

void Chess::onBoardClick(int mouseX, int mouseY) {

    if (!m_boardClickEnabled) {
        return;
    }

    int col = mouseX / m_specification.tileSize;
    int row = mouseY / m_specification.tileSize;

    if (!isValidBoardPosition(row, col))
        return;

    const Piece piece = m_board[row][col];
    std::cout << "Board Position Click (" << row << ", " << col << ")" << std::endl; // Debug

    if (piece.active && piece.colour == m_currentTurn) {
        m_selectedPiecePosition = {row, col};
        m_possibleMoves.clear();
        m_possibleMoves = m_moveLogic->processPieceMoves(piece, row, col);

    } else if (isMoveInPossibleMoves(row, col)) {
        movePiece(row, col);
    }
}

bool Chess::isValidBoardPosition(int row, int col) const {
    return col >= 0 && col < m_specification.boardSize && row >= 0 && row < m_specification.boardSize;
}

bool Chess::isMoveInPossibleMoves(int row, int col) const {
    for (const auto& move : m_possibleMoves) {
        if (move.row == row && move.col == col)
            return true;
    }
    return false;
}

void Chess::checkPawnPromotion(int targetRow, int targetCol) {
    Piece& piece = m_board[targetRow][targetCol];
    if (piece.type == PieceType::PAWN) {
        if ((piece.colour == PieceColour::WHITE && targetRow == 0) || (piece.colour == PieceColour::BLACK && targetRow == 7)) {
            piece.type = PieceType::QUEEN;
            std::cout << "Promotion\n";
            playSound("promotion");
        }
    }
}

void Chess::movePiece(int targetRow, int targetCol) {
    const Piece pieceToMove = m_board[m_selectedPiecePosition.row][m_selectedPiecePosition.col];

    if (isMoveIllegal(m_selectedPiecePosition, {targetRow, targetCol}, pieceToMove.colour)) {
        playSound("illegal");
        m_possibleMoves.clear();
        return;
    }

    if (m_board[targetRow][targetCol].type != PieceType::EMPTY) {

        Piece capturedPiece = m_board[targetRow][targetCol];
        if (capturedPiece.colour == PieceColour::WHITE) {
            m_takenWhitePieces[m_blackCaptureCount++] = capturedPiece;
        } else if (capturedPiece.colour == PieceColour::BLACK) {
            m_takenBlackPieces[m_whiteCaptureCount++] = capturedPiece;
        }
        playSound("capture");
    } else {
        playSound("move");
    }

    m_board[targetRow][targetCol] = Piece(pieceToMove.type, pieceToMove.colour);

    m_board[m_selectedPiecePosition.row][m_selectedPiecePosition.col] = Piece();

    checkPawnPromotion(targetRow, targetCol);

    m_selectedPiecePosition = {};
    m_possibleMoves.clear();

    PieceColour opponentColour = (pieceToMove.colour == PieceColour::WHITE) ? PieceColour::BLACK : PieceColour::WHITE;

    if (isInCheck(opponentColour)) {
        playSound("check");
    }

    if (isCheckmate(opponentColour)) {
        playSound("check");
        playSound("game-end");
        setupBoard();
    }

    toggleTurn();
}

bool Chess::isCheckmate(PieceColour colour) {
    if (!isInCheck(colour)) {
        return false;
    }

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const Piece& piece = m_board[row][col];
            if (piece.colour != colour || piece.type == PieceType::EMPTY) {
                continue;
            }

            std::vector<Position> piecePossibleMoves = m_moveLogic->processPieceMoves(piece, row, col);

            if (piecePossibleMoves.empty()) {
                continue;
            }

            for (const Position& move : piecePossibleMoves) {
                if (!isMoveIllegal({row, col}, move, colour)) {
                    return false;
                }
            }
        }
    }

    std::cout << (colour == PieceColour::WHITE ? "White" : "Black") << " is in checkmate!" << std::endl;
    return true;
}

bool Chess::isMoveIllegal(const Position& from, const Position& to, PieceColour colour) {
    Piece movingPiece = m_board[from.row][from.col];

    Piece originalPiece = m_board[to.row][to.col];

    m_board[to.row][to.col] = movingPiece;
    m_board[from.row][from.col] = Piece(PieceType::EMPTY, PieceColour::WHITE);

    bool isIllegal = isInCheck(colour);

    m_board[from.row][from.col] = movingPiece;
    m_board[to.row][to.col] = originalPiece;

    return isIllegal;
}

bool Chess::isInCheck(PieceColour colour) {
    Position kingPosition = findKing(colour);

    if (kingPosition.row == -1) {
        return false;
    }

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const Piece& piece = m_board[row][col];

            if (piece.type == PieceType::EMPTY || piece.colour == colour) {
                continue;
            }
            m_possibleMoves.clear();
            m_possibleMoves = m_moveLogic->processPieceMoves(piece, row, col);
            for (const Position& move : m_possibleMoves) {
                if (move.row == kingPosition.row && move.col == kingPosition.col) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Chess::toggleTurn() {
    m_possibleMoves.clear();
    m_currentTurn = (m_currentTurn == PieceColour::WHITE) ? PieceColour::BLACK : PieceColour::WHITE;
}

Position Chess::findKing(PieceColour colour) {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const Piece& piece = m_board[row][col];
            if (piece.type == PieceType::KING && piece.colour == colour) {
                return {row, col};
            }
        }
    }
    return {-1, -1};
}

void Chess::drawPiece(int col, int row) {
    const Piece& piece = m_board[row][col];

    if (piece.active) {
        std::string textureKey = getTextureKey(piece);
        SDL_Texture* texture = m_textures[textureKey];

        if (texture) {
            int x = col * m_specification.tileSize;
            int y = row * m_specification.tileSize;
            SDL_Rect destRect = {x, y, m_specification.tileSize, m_specification.tileSize};

            SDL_RenderCopy(m_renderer, texture, nullptr, &destRect);
        }
    }
}

void Chess::drawPossibleMoves() {
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

    SDL_Color transparentGreen = {0, 255, 0, 128};
    SDL_Rect rect;

    for (const auto& move : m_possibleMoves) {
        rect.x = move.col * m_specification.tileSize;
        rect.y = move.row * m_specification.tileSize;
        rect.w = m_specification.tileSize;
        rect.h = m_specification.tileSize;

        SDL_SetRenderDrawColor(m_renderer, transparentGreen.r, transparentGreen.g, transparentGreen.b, transparentGreen.a);

        SDL_RenderFillRect(m_renderer, &rect);
    }
}

void Chess::drawBoard() {
    for (int row = 0; row < m_specification.boardSize; ++row) {
        for (int col = 0; col < m_specification.boardSize; ++col) {
            SDL_Color tileColour = getTileColour(row, col);
            drawTile(col, row, tileColour);
            drawPiece(col, row);
        }
    }
}

void Chess::drawTile(int col, int row, SDL_Color colour) {
    SDL_SetRenderDrawColor(m_renderer, colour.r, colour.g, colour.b, colour.a);
    int x = col * m_specification.tileSize;
    int y = row * m_specification.tileSize;
    SDL_Rect tile = {x, y, m_specification.tileSize, m_specification.tileSize};
    SDL_RenderFillRect(m_renderer, &tile);
}

std::string Chess::getTextureKey(const Piece& piece) const {
    std::string colourPrefix = (piece.colour == PieceColour::WHITE) ? "white-" : "black-";
    std::string pieceName;

    switch (piece.type) {
    case PieceType::PAWN:
        pieceName = "pawn";
        break;
    case PieceType::ROOK:
        pieceName = "rook";
        break;
    case PieceType::KNIGHT:
        pieceName = "knight";
        break;
    case PieceType::BISHOP:
        pieceName = "bishop";
        break;
    case PieceType::QUEEN:
        pieceName = "queen";
        break;
    case PieceType::KING:
        pieceName = "king";
        break;
    default:
        pieceName = "unknown";
        break;
    }

    return colourPrefix + pieceName;
}

SDL_Texture* Chess::getTexture(const std::string& textureKey) const {
    auto it = m_textures.find(textureKey);
    if (it != m_textures.end()) {
        return it->second;
    }
    return nullptr;
}

SDL_Color Chess::getTileColour(int row, int col) {
    return (row % 2 == col % 2) ? m_specification.chessTileLightColour : m_specification.chessTileDarkColour;
}

Chess::~Chess() {
    for (auto& texture : m_textures) {
        SDL_DestroyTexture(texture.second);
    }

    for (auto& sound : m_sounds) {
        Mix_FreeChunk(sound.second);
    }

    m_sounds.clear();
    m_textures.clear();

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}
