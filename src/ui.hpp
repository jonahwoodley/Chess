#ifndef UI_HPP
#define UI_HPP

#include <SDL.h>
#include <imgui.h>

class UI {

public:
    explicit UI(class Chess* chess);
    void renderInterfaces();

private:
    void renderCurrentPlayerIndicator();
    void renderCapturePieces();

private:
    Chess* m_chess;
    ImFont* m_fontLargeLibreBaskerville;
};

#endif
