#pragma once

#include <rad/Gui/Window.h>
#include <rad/Gui/GuiRenderer.h>

class MainWindow : public rad::Window
{
public:
    MainWindow();
    ~MainWindow();

    void Destroy() override;

    // m_maxFrames < 0: no frame-count shutdown. Otherwise Destroy() after that many OnIdle ticks (same path as OnCloseRequested).
    bool Init(int width, int height, int maxFrames = -1);

    bool OnEvent(const SDL_Event& event) override;
    void OnIdle() override;

    // Window events:
    void OnWindowEvent(const SDL_WindowEvent& event) override;
    void OnShown() override;
    void OnHidden() override;
    void OnExposed() override;
    void OnMoved(int x, int y) override;
    void OnResized(int width, int height) override;
    void OnPixelSizeChanged(int width, int height) override;
    void OnMinimized() override;
    void OnMaximized() override;
    void OnRestored() override;
    void OnMouseEnter() override;
    void OnMouseLeave() override;
    // Window has gained keyboard focus.
    void OnFocusGained() override;
    // Window has lost keyboard focus.
    void OnFocusLost() override;
    void OnCloseRequested() override;
    void OnHitTest() override;
    void OnIccProfileChanged() override;
    void OnDisplayChanged() override;
    void OnDisplayScaleChanged() override;
    void OnOccluded() override;
    void OnEnterFullscreen() override;
    void OnLeaveFullscreen() override;
    void OnDestroyed() override;

    // Keyboard events:
    void OnKeyDown(const SDL_KeyboardEvent& keyDown) override;
    void OnKeyUp(const SDL_KeyboardEvent& keyUp) override;
    void OnTextEditing(const SDL_TextEditingEvent& textEditing) override;
    void OnTextInput(const SDL_TextInputEvent& textInput) override;

    // Mouse events:
    void OnMouseMove(const SDL_MouseMotionEvent& mouseMotion) override;
    void OnMouseButtonDown(const SDL_MouseButtonEvent& mouseButton) override;
    void OnMouseButtonUp(const SDL_MouseButtonEvent& mouseButton) override;
    void OnMouseWheel(const SDL_MouseWheelEvent& mouseWheel) override;

    // User events
    void OnUserEvent(const SDL_UserEvent& user) override;

private:
    void DrawDebugOverlay();

    rad::Ref<rad::GuiRenderer> m_renderer;
    bool m_showDemoWindow = true;
    bool m_logMouseMotion = false;
    float m_mouseMotionX = 0.f;
    float m_mouseMotionY = 0.f;
    float m_mouseMotionXrel = 0.f;
    float m_mouseMotionYrel = 0.f;

    int m_maxFrames = -1;
    int m_frameCount = 0;
}; // class MainWindow
