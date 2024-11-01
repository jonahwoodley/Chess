#include "ui.hpp"
#include "chess.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "piece.hpp"
#include <array>
#include <iostream>

UI::UI(Chess* chess)
    : m_chess(chess) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO io = ImGui::GetIO();

    io.Fonts->AddFontDefault();
    ImFont* m_fontLargeLibreBaskerville = io.Fonts->AddFontFromFileTTF("resources/fonts/LibreBaskerville-Regular.ttf", 20.0f);
    IM_ASSERT(m_fontLargeLibreBaskerville != NULL);

    ImGui_ImplSDL2_InitForSDLRenderer(m_chess->getWindow(), m_chess->getRenderer());
    ImGui_ImplSDLRenderer2_Init(m_chess->getRenderer());

    if (m_fontLargeLibreBaskerville == nullptr) {
        std::cerr << "Error: Could not load font from resources/fonts/LibreBaskerville-Regular.ttf" << std::endl;
    } else {
        std::cout << "Font loaded successfully!" << std::endl;
    }

    io.Fonts->Build();
}

void UI::renderInterfaces() {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    static float f = 0.0f;

    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Once);

    ImGui::Begin("Chess V0.1");

    renderCurrentPlayerIndicator();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    renderCapturePieces();

    ImGui::End();

    ImGui::Render();
}

void UI::renderCurrentPlayerIndicator() {
    PieceColour currentColour = m_chess->getCurrentTurn();

    ImU32 turnColour = (currentColour == PieceColour::WHITE) ? ImColor(255, 255, 255) : ImColor(0, 0, 0);

    ImGui::Text("Current Turn: ");

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, turnColour);
    ImGui::PushStyleColor(ImGuiCol_Border, ImColor(255, 255, 255).Value);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.5f);
    ImGui::Button("##CurrentTurnIndicator", ImVec2(15, 15));
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void UI::renderCapturePieces() {
    const std::array<Piece, 16>& takenWhitePieces = m_chess->getTakenWhitePieces();
    const std::array<Piece, 16>& takenBlackPieces = m_chess->getTakenBlackPieces();

    ImGui::Text("Captured White Pieces:");
    for (const Piece& piece : takenWhitePieces) {
        if (piece.type != PieceType::EMPTY) {
            std::string textureKey = m_chess->getTextureKey(piece);
            SDL_Texture* pieceTexture = m_chess->getTexture(textureKey);
            if (pieceTexture) {
                ImGui::Image(reinterpret_cast<ImTextureID>(pieceTexture), ImVec2(30, 30));
                ImGui::SameLine();
            }
        }
    }
    ImGui::NewLine();
    ImGui::Spacing();

    ImGui::Text("Captured Black Pieces:");
    for (const Piece& piece : takenBlackPieces) {
        if (piece.type != PieceType::EMPTY) {
            std::string textureKey = m_chess->getTextureKey(piece);
            SDL_Texture* pieceTexture = m_chess->getTexture(textureKey);
            if (pieceTexture) {
                ImGui::Image(reinterpret_cast<ImTextureID>(pieceTexture), ImVec2(30, 30));
                ImGui::SameLine();
            }
        }
    }
}