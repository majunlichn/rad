#include "GuiRenderer.test.h"

#include "imgui.h"

void ClearTest::ShowSettings()
{
    ImGui::ColorPicker4("Clear Color", m_clearColor);
}

void ClearTest::OnRender()
{
    m_renderer->SetRenderDrawColor(m_clearColor[0], m_clearColor[1], m_clearColor[2],
                                   m_clearColor[3]);
    m_renderer->Clear();
}
