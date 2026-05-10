#pragma once

#include <rad/Gui/GuiRenderer.h>

class RenderTest : public rad::RefCounted<RenderTest>
{
public:
    RenderTest(rad::Window* window, rad::GuiRenderer* renderer) :
        m_window(window),
        m_renderer(renderer)
    {
    }
    virtual ~RenderTest() = default;

    virtual void OnRender() = 0;

    // Dear ImGui UI updated each frame for this test (widgets handled by ImGui).
    virtual void ShowSettings() {}

protected:
    rad::Window* m_window;
    rad::GuiRenderer* m_renderer;
}; // class RenderTest

class ClearTest : public RenderTest
{
public:
    ClearTest(rad::Window* window, rad::GuiRenderer* renderer) :
        RenderTest(window, renderer)
    {
    }
    ~ClearTest() override = default;

    void ShowSettings() override;
    void OnRender() override;

private:
    float m_clearColor[4] = {0, 0, 0, 1};

}; // class ClearTest
