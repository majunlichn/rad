#include <rad/Common/MemoryDebug.h>

#include "Window.test.h"

#include "imgui.h"

#include <stdexcept>
#include <string>

MainWindow::MainWindow()
{
}

MainWindow::~MainWindow()
{
    Destroy();
}

bool MainWindow::Init(int width, int height, int maxFrames)
{
    m_maxFrames = maxFrames;
    m_frameCount = 0;
    if (!Create("MainWindow", width, height))
    {
        RAD_LOG_GUI(err, "Failed to create MainWindow");
        return false;
    }
    m_renderer = RAD_NEW rad::GuiRenderer(this);
    return true;
}

bool MainWindow::OnEvent(const SDL_Event& event)
{
    if (m_renderer)
    {
        m_renderer->ProcessEvent(event);
    }
    return rad::Window::OnEvent(event);
}

void MainWindow::OnIdle()
{
    if (m_renderer)
    {
        m_renderer->BeginFrame();
        if (m_showDemoWindow)
        {
            ImGui::ShowDemoWindow(&m_showDemoWindow);
        }
        DrawDebugOverlay();
        m_renderer->EndFrame();
    }

    rad::Window::OnIdle();

    if (m_maxFrames >= 0)
    {
        ++m_frameCount;
        if (m_frameCount >= m_maxFrames)
        {
            Destroy();
        }
    }
}

void MainWindow::DrawDebugOverlay()
{
    ImGui::SetNextWindowPos(ImVec2(12.f, 12.f), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.55f);
    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                                       ImGuiWindowFlags_AlwaysAutoResize |
                                       ImGuiWindowFlags_NoSavedSettings |
                                       ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    ImGui::Begin("DebugOverlay", nullptr, flags);
    ImGui::Checkbox("Log Mouse Motion", &m_logMouseMotion);
    {
        float mouseX = 0.f;
        float mouseY = 0.f;
        SDL_GetMouseState(&mouseX, &mouseY);
        ImGui::Text("Mouse Position: %8.2f, %8.2f", mouseX, mouseY);
    }
    ImGui::End();
}

void MainWindow::OnWindowEvent(const SDL_WindowEvent& event)
{
    rad::Window::OnWindowEvent(event);
}

void MainWindow::OnShown()
{
    RAD_LOG_GUI(info, "MainWindow::OnShown");
}

void MainWindow::OnHidden()
{
    RAD_LOG_GUI(info, "MainWindow::OnHidden");
}

void MainWindow::OnExposed()
{
    RAD_LOG_GUI(info, "MainWindow::OnExposed");
}

void MainWindow::OnMoved(int x, int y)
{
    RAD_LOG_GUI(info, "MainWindow::OnMoved: {:4}, {:4}", x, y);
}

void MainWindow::OnResized(int width, int height)
{
    RAD_LOG_GUI(info, "MainWindow::OnResized: {:4}, {:4}", width, height);
}

void MainWindow::OnPixelSizeChanged(int width, int height)
{
    RAD_LOG_GUI(info, "MainWindow::OnPixelSizeChanged: {:4}, {:4}", width, height);
}

void MainWindow::OnMinimized()
{
    RAD_LOG_GUI(info, "MainWindow::OnMinimized");
}

void MainWindow::OnMaximized()
{
    RAD_LOG_GUI(info, "MainWindow::OnMaximized");
}

void MainWindow::OnRestored()
{
    RAD_LOG_GUI(info, "MainWindow::OnRestored");
}

void MainWindow::OnMouseEnter()
{
    RAD_LOG_GUI(info, "MainWindow::OnMouseEnter");
}

void MainWindow::OnMouseLeave()
{
    RAD_LOG_GUI(info, "MainWindow::OnMouseLeave");
}

void MainWindow::OnFocusGained()
{
    RAD_LOG_GUI(info, "MainWindow::OnFocusGained");
}

void MainWindow::OnFocusLost()
{
    RAD_LOG_GUI(info, "MainWindow::OnFocusLost");
}

void MainWindow::OnCloseRequested()
{
    RAD_LOG_GUI(info, "MainWindow::OnCloseRequested");
    Destroy();
}

void MainWindow::Destroy()
{
    RAD_LOG_GUI(info, "MainWindow::Destroy");
    m_renderer.reset();
    rad::Window::Destroy();
}

void MainWindow::OnHitTest()
{
    RAD_LOG_GUI(info, "MainWindow::OnHitTest");
}

void MainWindow::OnIccProfileChanged()
{
    RAD_LOG_GUI(info, "MainWindow::OnIccProfileChanged");
}

void MainWindow::OnDisplayChanged()
{
    RAD_LOG_GUI(info, "MainWindow::OnDisplayChanged");
}

void MainWindow::OnDisplayScaleChanged()
{
    RAD_LOG_GUI(info, "MainWindow::OnDisplayScaleChanged");
}

void MainWindow::OnOccluded()
{
    RAD_LOG_GUI(info, "MainWindow::OnOccluded");
}

void MainWindow::OnEnterFullscreen()
{
    RAD_LOG_GUI(info, "MainWindow::OnEnterFullscreen");
}

void MainWindow::OnLeaveFullscreen()
{
    RAD_LOG_GUI(info, "MainWindow::OnLeaveFullscreen");
}

void MainWindow::OnDestroyed()
{
    RAD_LOG_GUI(info, "MainWindow::OnDestroyed");
}

static std::string GetKeyModString(Uint16 modBits)
{
    const SDL_Keymod m = static_cast<SDL_Keymod>(modBits);
    if (m == SDL_KMOD_NONE)
    {
        return "None";
    }
    std::string s;
    if (m & SDL_KMOD_CTRL)
    {
        s += "Ctrl+";
    }
    if (m & SDL_KMOD_SHIFT)
    {
        s += "Shift+";
    }
    if (m & SDL_KMOD_ALT)
    {
        s += "Alt+";
    }
    if (m & SDL_KMOD_GUI)
    {
        s += "Gui+";
    }
    if (m & SDL_KMOD_CAPS)
    {
        s += "Caps+";
    }
    if (m & SDL_KMOD_NUM)
    {
        s += "Num+";
    }
    if (!s.empty() && s.back() == '+')
    {
        s.pop_back();
    }
    return s;
}

void MainWindow::OnKeyDown(const SDL_KeyboardEvent& keyDown)
{
    RAD_LOG_GUI(info, "MainWindow::OnKeyDown: {} (mods={}; repeat={})", SDL_GetKeyName(keyDown.key),
                GetKeyModString(keyDown.mod), keyDown.repeat);
    if (keyDown.key == SDLK_F1 && !keyDown.repeat)
    {
        m_showDemoWindow = !m_showDemoWindow;
    }
    else if (keyDown.key == SDLK_ESCAPE && !keyDown.repeat)
    {
        Destroy();
    }
}

void MainWindow::OnKeyUp(const SDL_KeyboardEvent& keyUp)
{
    RAD_LOG_GUI(info, "MainWindow::OnKeyUp: {} (mods={}; repeat={})", SDL_GetKeyName(keyUp.key),
                GetKeyModString(keyUp.mod), keyUp.repeat);
}

void MainWindow::OnTextEditing(const SDL_TextEditingEvent& textEditing)
{
    RAD_LOG_GUI(info, "MainWindow::OnTextEditing: {}", textEditing.text);
}

void MainWindow::OnTextInput(const SDL_TextInputEvent& textInput)
{
    RAD_LOG_GUI(info, "MainWindow::OnTextInput: {}", textInput.text);
}

void MainWindow::OnMouseMove(const SDL_MouseMotionEvent& mouseMotion)
{
    m_mouseMotionX = mouseMotion.x;
    m_mouseMotionY = mouseMotion.y;
    m_mouseMotionXrel = mouseMotion.xrel;
    m_mouseMotionYrel = mouseMotion.yrel;
    if (m_logMouseMotion)
    {
        RAD_LOG_GUI(info, "MainWindow::OnMouseMove: {:8.2f}, {:8.2f} ({:+8.2f}, {:+8.2f})",
                    m_mouseMotionX, m_mouseMotionY, m_mouseMotionXrel, m_mouseMotionYrel);
    }
}

static const char* GetMouseButtonName(Uint8 button)
{
    switch (button)
    {
    case SDL_BUTTON_LEFT:
        return "Left";
    case SDL_BUTTON_MIDDLE:
        return "Middle";
    case SDL_BUTTON_RIGHT:
        return "Right";
    case SDL_BUTTON_X1:
        return "X1";
    case SDL_BUTTON_X2:
        return "X2";
    }
    return "Unknown";
}

void MainWindow::OnMouseButtonDown(const SDL_MouseButtonEvent& mouseButton)
{
    RAD_LOG_GUI(info, "MainWindow::OnMouseButtonDown: {} (clicks={})",
                GetMouseButtonName(mouseButton.button), mouseButton.clicks);
}

void MainWindow::OnMouseButtonUp(const SDL_MouseButtonEvent& mouseButton)
{
    RAD_LOG_GUI(info, "MainWindow::OnMouseButtonUp: {} (clicks={})",
                GetMouseButtonName(mouseButton.button), mouseButton.clicks);
}

void MainWindow::OnMouseWheel(const SDL_MouseWheelEvent& mouseWheel)
{
    RAD_LOG_GUI(info, "MainWindow::OnMouseWheel: {:+4.2f}, {:+4.2f}", mouseWheel.x, mouseWheel.y);
}

void MainWindow::OnUserEvent(const SDL_UserEvent& user)
{
    RAD_LOG_GUI(info, "MainWindow::OnUserEvent");
}
