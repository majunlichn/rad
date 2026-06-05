#include <rad/Common/MemoryDebug.h>

#include "Window.test.h"

#include "imgui.h"

#include <algorithm>
#include <stdexcept>
#include <string>

WindowTest::WindowTest()
{
}

WindowTest::~WindowTest()
{
    Destroy();
}

bool WindowTest::Init(int width, int height, int maxFrames)
{
    m_maxFrames = maxFrames;
    m_frameCount = 0;
    if (!Create("WindowTest", width, height))
    {
        RAD_LOG_GUI(err, "Failed to create WindowTest");
        return false;
    }
    m_renderer = RAD_NEW rad::GuiRenderer(this);
    m_renderTestNames = {"None", "Clear"};
    m_renderTestIndex = 0;
    return true;
}

bool WindowTest::OnEvent(const SDL_Event& event)
{
    if (m_renderer)
    {
        m_renderer->ProcessEvent(event);
    }
    return rad::Window::OnEvent(event);
}

void WindowTest::OnIdle()
{
    if (m_renderer)
    {
        m_renderer->BeginFrame();
        ShowDebugOverlay();
        if (m_showDemoWindow)
        {
            ImGui::ShowDemoWindow(&m_showDemoWindow);
        }
        if (m_showRenderTestPanel)
        {
            ShowRenderTestPanel();
        }
        if (m_renderTest)
        {
            m_renderTest->OnRender();
        }
        else
        {
            m_renderer->SetRenderDrawColor(0, 0, 0, SDL_ALPHA_OPAQUE_FLOAT);
            m_renderer->Clear();
        }
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

void WindowTest::ShowRenderTestPanel()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowSizeConstraints(ImVec2(280.f, 140.f), viewport->WorkSize);
    if (ImGui::Begin("RenderTest", &m_showRenderTestPanel, ImGuiWindowFlags_AlwaysAutoResize))
    {
        int comboIndex = m_renderTestIndex;
        const int testCount = static_cast<int>(m_renderTestNames.size());
        const int lastIndex = std::max(0, testCount - 1);
        if (ImGui::Combo("##RenderTestCombo", &comboIndex, m_renderTestNames.data(), testCount))
        {
            comboIndex = std::clamp(comboIndex, 0, lastIndex);
        }
        if (m_renderTestIndex != comboIndex)
        {
            m_renderTestIndex = comboIndex;
            m_renderTest = CreateRenderTest(m_renderTestNames[m_renderTestIndex]);
        }

        ImGui::Separator();

        if (m_renderTest)
        {
            m_renderTest->ShowSettings();
        }
        else
        {
            ImGui::TextDisabled("No test selected.");
        }
        ImGui::End();
    }
}

rad::Ref<RenderTest> WindowTest::CreateRenderTest(rad::cstring_view name)
{
    if (name == "Clear")
    {
        return RAD_NEW ClearTest(this, m_renderer.get());
    }
    return nullptr;
}

void WindowTest::ShowDebugOverlay()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const float pad = 12.f;
    ImGui::SetNextWindowPos(ImVec2(pad, pad), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.55f);
    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                                       ImGuiWindowFlags_AlwaysAutoResize |
                                       ImGuiWindowFlags_NoSavedSettings |
                                       ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    ImGui::Begin("DebugOverlay", nullptr, flags);
    {
        float mouseX = 0.f;
        float mouseY = 0.f;
        SDL_GetMouseState(&mouseX, &mouseY);
        ImGui::Text("Mouse Position: %8.2f, %8.2f", mouseX, mouseY);
    }
    ImGui::Checkbox("Log Mouse Motion", &m_logMouseMotion);
    ImGui::End();
}

void WindowTest::OnWindowEvent(const SDL_WindowEvent& event)
{
    rad::Window::OnWindowEvent(event);
}

void WindowTest::OnShown()
{
    RAD_LOG_GUI(info, "WindowTest::OnShown");
}

void WindowTest::OnHidden()
{
    RAD_LOG_GUI(info, "WindowTest::OnHidden");
}

void WindowTest::OnExposed()
{
    RAD_LOG_GUI(info, "WindowTest::OnExposed");
}

void WindowTest::OnMoved(int x, int y)
{
    RAD_LOG_GUI(info, "WindowTest::OnMoved: {:4}, {:4}", x, y);
}

void WindowTest::OnResized(int width, int height)
{
    RAD_LOG_GUI(info, "WindowTest::OnResized: {:4}, {:4}", width, height);
}

void WindowTest::OnPixelSizeChanged(int width, int height)
{
    RAD_LOG_GUI(info, "WindowTest::OnPixelSizeChanged: {:4}, {:4}", width, height);
}

void WindowTest::OnMinimized()
{
    RAD_LOG_GUI(info, "WindowTest::OnMinimized");
}

void WindowTest::OnMaximized()
{
    RAD_LOG_GUI(info, "WindowTest::OnMaximized");
}

void WindowTest::OnRestored()
{
    RAD_LOG_GUI(info, "WindowTest::OnRestored");
}

void WindowTest::OnMouseEnter()
{
    RAD_LOG_GUI(info, "WindowTest::OnMouseEnter");
}

void WindowTest::OnMouseLeave()
{
    RAD_LOG_GUI(info, "WindowTest::OnMouseLeave");
}

void WindowTest::OnFocusGained()
{
    RAD_LOG_GUI(info, "WindowTest::OnFocusGained");
}

void WindowTest::OnFocusLost()
{
    RAD_LOG_GUI(info, "WindowTest::OnFocusLost");
}

void WindowTest::OnCloseRequested()
{
    RAD_LOG_GUI(info, "WindowTest::OnCloseRequested");
    Destroy();
}

void WindowTest::Destroy()
{
    RAD_LOG_GUI(info, "WindowTest::Destroy");
    m_renderer.reset();
    rad::Window::Destroy();
}

void WindowTest::OnHitTest()
{
    RAD_LOG_GUI(info, "WindowTest::OnHitTest");
}

void WindowTest::OnIccProfileChanged()
{
    RAD_LOG_GUI(info, "WindowTest::OnIccProfileChanged");
}

void WindowTest::OnDisplayChanged()
{
    RAD_LOG_GUI(info, "WindowTest::OnDisplayChanged");
}

void WindowTest::OnDisplayScaleChanged()
{
    RAD_LOG_GUI(info, "WindowTest::OnDisplayScaleChanged");
}

void WindowTest::OnOccluded()
{
    RAD_LOG_GUI(info, "WindowTest::OnOccluded");
}

void WindowTest::OnEnterFullscreen()
{
    RAD_LOG_GUI(info, "WindowTest::OnEnterFullscreen");
}

void WindowTest::OnLeaveFullscreen()
{
    RAD_LOG_GUI(info, "WindowTest::OnLeaveFullscreen");
}

void WindowTest::OnDestroyed()
{
    RAD_LOG_GUI(info, "WindowTest::OnDestroyed");
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

void WindowTest::OnKeyDown(const SDL_KeyboardEvent& keyDown)
{
    RAD_LOG_GUI(info, "WindowTest::OnKeyDown: {} (mods={}; repeat={})", SDL_GetKeyName(keyDown.key),
                GetKeyModString(keyDown.mod), keyDown.repeat);
    if (keyDown.key == SDLK_F1 && !keyDown.repeat)
    {
        m_showDemoWindow = !m_showDemoWindow;
    }
    else if (keyDown.key == SDLK_F2 && !keyDown.repeat)
    {
        m_showRenderTestPanel = !m_showRenderTestPanel;
    }
    else if (keyDown.key == SDLK_ESCAPE && !keyDown.repeat)
    {
        Destroy();
    }
}

void WindowTest::OnKeyUp(const SDL_KeyboardEvent& keyUp)
{
    RAD_LOG_GUI(info, "WindowTest::OnKeyUp: {} (mods={}; repeat={})", SDL_GetKeyName(keyUp.key),
                GetKeyModString(keyUp.mod), keyUp.repeat);
}

void WindowTest::OnTextEditing(const SDL_TextEditingEvent& textEditing)
{
    RAD_LOG_GUI(info, "WindowTest::OnTextEditing: {}", textEditing.text);
}

void WindowTest::OnTextInput(const SDL_TextInputEvent& textInput)
{
    RAD_LOG_GUI(info, "WindowTest::OnTextInput: {}", textInput.text);
}

void WindowTest::OnMouseMove(const SDL_MouseMotionEvent& mouseMotion)
{
    m_mouseMotionX = mouseMotion.x;
    m_mouseMotionY = mouseMotion.y;
    m_mouseMotionXrel = mouseMotion.xrel;
    m_mouseMotionYrel = mouseMotion.yrel;
    if (m_logMouseMotion)
    {
        RAD_LOG_GUI(info, "WindowTest::OnMouseMove: {:8.2f}, {:8.2f} ({:+8.2f}, {:+8.2f})",
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

void WindowTest::OnMouseButtonDown(const SDL_MouseButtonEvent& mouseButton)
{
    RAD_LOG_GUI(info, "WindowTest::OnMouseButtonDown: {} (clicks={})",
                GetMouseButtonName(mouseButton.button), mouseButton.clicks);
}

void WindowTest::OnMouseButtonUp(const SDL_MouseButtonEvent& mouseButton)
{
    RAD_LOG_GUI(info, "WindowTest::OnMouseButtonUp: {} (clicks={})",
                GetMouseButtonName(mouseButton.button), mouseButton.clicks);
}

void WindowTest::OnMouseWheel(const SDL_MouseWheelEvent& mouseWheel)
{
    RAD_LOG_GUI(info, "WindowTest::OnMouseWheel: {:+4.2f}, {:+4.2f}", mouseWheel.x, mouseWheel.y);
}

void WindowTest::OnUserEvent(const SDL_UserEvent& user)
{
    RAD_LOG_GUI(info, "WindowTest::OnUserEvent");
}
