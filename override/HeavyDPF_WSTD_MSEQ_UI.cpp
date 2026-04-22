/**
 * Copyright (c) Wasted Audio 2023 - GPL-3.0-or-later
 */

#include "DistrhoUI.hpp"
#ifdef DISTRHO_OS_WASM
#include "DistrhoStandaloneUtils.hpp"
#endif
#include "ResizeHandle.hpp"
#include "veramobd.hpp"
#include "wstdcolors.hpp"


START_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------------------------------
enum HeavyParams {
    M_HIGH,
    M_LOW,
    M_MID,
    M_MID_FREQ,
    S_HIGH,
    S_LOW,
    S_MID,
    S_MID_FREQ,
};


class ImGuiPluginUI : public UI
{
    float fm_high = 0.0f;
    float fm_low = 0.0f;
    float fm_mid = 0.0f;
    float fm_mid_freq = 1337.0f;
    float fs_high = 0.0f;
    float fs_low = 0.0f;
    float fs_mid = 0.0f;
    float fs_mid_freq = 1337.0f;

    // ----------------------------------------------------------------------------------------------------------------

public:
   /**
      UI class constructor.
      The UI should be initialized to a default state that matches the plugin side.
    */
    ImGuiPluginUI()
        : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT)
    {
        ImGuiIO& io(ImGui::GetIO());

        ImFontConfig fc;
        fc.FontDataOwnedByAtlas = true;
        fc.OversampleH = 1;
        fc.OversampleV = 1;
        fc.PixelSnapH = true;

        io.Fonts->AddFontFromMemoryCompressedTTF((void*)veramobd_compressed_data, veramobd_compressed_size, 16.0f * getScaleFactor(), &fc);
        io.Fonts->AddFontFromMemoryCompressedTTF((void*)veramobd_compressed_data, veramobd_compressed_size, 21.0f * getScaleFactor(), &fc);
        io.Fonts->AddFontFromMemoryCompressedTTF((void*)veramobd_compressed_data, veramobd_compressed_size, 11.0f * getScaleFactor(), &fc);
        io.Fonts->AddFontFromMemoryCompressedTTF((void*)veramobd_compressed_data, veramobd_compressed_size, 12.5f * getScaleFactor(), &fc);
        io.Fonts->Build();
        io.FontDefault = io.Fonts->Fonts[1];
    }

protected:
    // ----------------------------------------------------------------------------------------------------------------
    // DSP/Plugin Callbacks

   /**
      A parameter has changed on the plugin side.@n
      This is called by the host to inform the UI about parameter changes.
    */
    void parameterChanged(uint32_t index, float value) override
    {
        switch (index) {
            case M_HIGH:
                fm_high = value;
                break;
            case M_LOW:
                fm_low = value;
                break;
            case M_MID:
                fm_mid = value;
                break;
            case M_MID_FREQ:
                fm_mid_freq = value;
                break;
            case S_HIGH:
                fs_high = value;
                break;
            case S_LOW:
                fs_low = value;
                break;
            case S_MID:
                fs_mid = value;
                break;
            case S_MID_FREQ:
                fs_mid_freq = value;
                break;

            default: return;
        }

        repaint();
    }

    // ----------------------------------------------------------------------------------------------------------------
    // Widget Callbacks

   /**
      ImGui specific onDisplay function.
    */
    void onImGuiDisplay() override
    {

        const float width = getWidth();
        const float height = getHeight();
        const float margin = 0.0f;
        auto scaleFactor   = getScaleFactor();
        #ifdef DISTRHO_OS_WASM
        static bool inputActive = false;
        #endif

        ImGui::SetNextWindowPos(ImVec2(margin, margin));
        ImGui::SetNextWindowSize(ImVec2(width - 2 * margin, height - 2 * margin));

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

        style.Colors[ImGuiCol_TitleBgActive] = (ImVec4)WstdTitleBgActive;
        style.Colors[ImGuiCol_WindowBg] = (ImVec4)WstdWindowBg;

        ImGuiIO& io(ImGui::GetIO());
        ImFont* defaultFont = ImGui::GetFont();
        ImFont* titleBarFont = io.Fonts->Fonts[2];
        ImFont* smallFont    = io.Fonts->Fonts[3];
        ImFont* mediumFont   = io.Fonts->Fonts[4];

        auto m_HighColorActive     = ColorBright(Blue, fm_high);
        auto m_HighColorHovered    = ColorBright(BlueBr, fm_high);
        auto m_MidColorActive      = ColorMid(Blue, Green, Red, fm_mid, fm_mid_freq);
        auto m_MidColorHovered     = ColorMid(BlueBr, GreenBr, RedBr, fm_mid, fm_mid_freq);
        auto m_MidFreqColorActive  = ColorMid(BlueBr, GreenDr, RedBr, fm_mid, fm_mid_freq);
        auto m_MidFreqColorHovered = ColorMid(Blue, Green, Red, fm_mid, fm_mid_freq);
        auto m_LowColorActive      = ColorBright(Red, fm_low);
        auto m_LowColorHovered     = ColorBright(RedBr, fm_low);

        auto s_HighColorActive     = ColorBright(Blue, fs_high);
        auto s_HighColorHovered    = ColorBright(BlueBr, fs_high);
        auto s_MidColorActive      = ColorMid(Blue, Green, Red, fs_mid, fs_mid_freq);
        auto s_MidColorHovered     = ColorMid(BlueBr, GreenBr, RedBr, fs_mid, fs_mid_freq);
        auto s_MidFreqColorActive  = ColorMid(BlueBr, GreenDr, RedBr, fs_mid, fs_mid_freq);
        auto s_MidFreqColorHovered = ColorMid(Blue, Green, Red, fs_mid, fs_mid_freq);
        auto s_LowColorActive      = ColorBright(Red, fs_low);
        auto s_LowColorHovered     = ColorBright(RedBr, fs_low);

        const float hundred = 100 * scaleFactor;
        const float seventy = 70 * scaleFactor;

        const float knobWidth = 85 * scaleFactor;

        auto dbstep = 0.1f;
        auto hzstep = 20.0f;

        if (io.KeyShift)
        {
            dbstep = 0.01f;
            hzstep = 1.0f;
        }

        ImGui::PushFont(titleBarFont);
        if (ImGui::Begin("WSTD MSEQ", nullptr, ImGuiWindowFlags_NoResize + ImGuiWindowFlags_NoCollapse))
        {
            ImGui::PushFont(defaultFont);

            #ifdef DISTRHO_OS_WASM
            if (!inputActive)
            {
                ImGui::OpenPopup("Activate");
            }

            if (ImGui::BeginPopupModal("Activate", nullptr, ImGuiWindowFlags_NoResize + ImGuiWindowFlags_NoMove))
            {
                if (ImGui::Button("OK", ImVec2(80, 0)))
                {
                    requestAudioInput();
                    inputActive = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            #endif

            auto ImGuiKnob_Flags = ImGuiKnobFlags_DoubleClickReset + ImGuiKnobFlags_ValueTooltip + ImGuiKnobFlags_NoInput + ImGuiKnobFlags_ValueTooltipHideOnClick;
            auto ImGuiKnob_FlagsDB = ImGuiKnob_Flags + ImGuiKnobFlags_dB;
            auto ImGuiKnob_FlagsLog = ImGuiKnob_Flags + ImGuiKnobFlags_Logarithmic;

            ImDrawList* drawList = ImGui::GetWindowDrawList();
            // M background
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ColorBright(WstdWindowBg, 5.0f, false));
            drawList->AddRectFilled(ImVec2(0, 0), ImVec2(ImGui::GetWindowWidth()/2, ImGui::GetWindowHeight()), ImGui::GetColorU32(ImGuiCol_FrameBg));
            ImGui::PopStyleColor();
            // M/S background
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ColorBright(WstdTitleBgActive, -15.0f, false));
            drawList->AddRectFilled(ImVec2(0, 0), ImVec2(ImGui::GetWindowWidth(), 62*scaleFactor), ImGui::GetColorU32(ImGuiCol_FrameBg));
            ImGui::PopStyleColor();

            ImGui::BeginGroup();
            {
                ImGui::PushStyleColor(ImGuiCol_Text, TextClr);
                ImGui::PushFont(titleBarFont);
                CenterTextX("M", knobWidth);
                ImGui::PopFont();
                ImGui::PopStyleColor();

                ImGui::Dummy(ImVec2(0.0f, 8.0f * scaleFactor));

                ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)m_HighColorActive);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)m_HighColorHovered);
                ImGui::PushID(M_HIGH);
                if (ImGuiKnobs::Knob("High", &fm_high, -15.0f, 15.0, dbstep, "%.2fdB", ImGuiKnobVariant_SteppedTick, hundred, ImGuiKnob_FlagsDB, 7))
                {
                    if (ImGui::IsItemActivated())
                    {
                        editParameter(M_HIGH, true);
                        if (ImGui::IsMouseDoubleClicked(0))
                            fm_high = 0.0f;
                    }
                    setParameterValue(M_HIGH, fm_high);
                }
                ImGui::PopID();
                ImGui::PopStyleColor(2);

                ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)m_MidColorActive);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)m_MidColorHovered);
                ImGui::PushID(M_MID);
                if (ImGuiKnobs::Knob("Mid", &fm_mid, -15.0f, 15.0, dbstep, "%.2fdB", ImGuiKnobVariant_SteppedTick, hundred, ImGuiKnob_FlagsDB, 7))
                {
                    if (ImGui::IsItemActivated())
                    {
                        editParameter(M_MID, true);
                        if (ImGui::IsMouseDoubleClicked(0))
                            fm_mid = 0.0f;
                    }
                    setParameterValue(M_MID, fm_mid);
                }
                ImGui::PopID();
                ImGui::PopStyleColor(2);

                ImGui::Dummy(ImVec2(7.5f * scaleFactor, 0.0f)); ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)m_MidFreqColorActive);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)m_MidFreqColorHovered);
                ImGui::PushFont(smallFont);
                ImGui::PushID(M_MID_FREQ);
                if (ImGuiKnobs::Knob("Mid Freq", &fm_mid_freq, 313.3f, 5705.6f, hzstep, "%.1fHz", ImGuiKnobVariant_SteppedTick, seventy, ImGuiKnob_FlagsLog, 11))
                {
                    if (ImGui::IsItemActivated())
                    {
                        editParameter(M_MID_FREQ, true);
                        if (ImGui::IsMouseDoubleClicked(0))
                            fm_mid_freq = 1337.0f;
                    }
                    setParameterValue(M_MID_FREQ, fm_mid_freq);
                }
                ImGui::PopID();
                ImGui::PopFont();
                ImGui::PopStyleColor(2);

                ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)m_LowColorActive);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)m_LowColorHovered);
                ImGui::PushID(M_LOW);
                if (ImGuiKnobs::Knob("Low", &fm_low, -15.0f, 15.0, dbstep, "%.2fdB", ImGuiKnobVariant_SteppedTick, hundred, ImGuiKnob_FlagsDB, 7))
                {
                    if (ImGui::IsItemActivated())
                    {
                        editParameter(M_LOW, true);
                        if (ImGui::IsMouseDoubleClicked(0))
                            fm_low = 0.0f;
                    }
                    setParameterValue(M_LOW, fm_low);
                }
                ImGui::PopID();
                ImGui::PopStyleColor(2);
            }
            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::BeginGroup();
            {
                ImGui::PushStyleColor(ImGuiCol_Text, TextClr);
                ImGui::PushFont(titleBarFont);
                CenterTextX("S", knobWidth);
                ImGui::PopFont();
                ImGui::PopStyleColor();

                ImGui::Dummy(ImVec2(0.0f, 8.0f * scaleFactor));

                ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)s_HighColorActive);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)s_HighColorHovered);
                ImGui::PushID(S_HIGH);
                if (ImGuiKnobs::Knob("High", &fs_high, -15.0f, 15.0, dbstep, "%.2fdB", ImGuiKnobVariant_SteppedTick, hundred, ImGuiKnob_FlagsDB, 7))
                {
                    if (ImGui::IsItemActivated())
                    {
                        editParameter(S_HIGH, true);
                        if (ImGui::IsMouseDoubleClicked(0))
                            fs_high = 0.0f;
                    }
                    setParameterValue(S_HIGH, fs_high);
                }
                ImGui::PopID();
                ImGui::PopStyleColor(2);

                ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)s_MidColorActive);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)s_MidColorHovered);
                ImGui::PushID(S_MID);
                if (ImGuiKnobs::Knob("Mid", &fs_mid, -15.0f, 15.0, dbstep, "%.2fdB", ImGuiKnobVariant_SteppedTick, hundred, ImGuiKnob_FlagsDB, 7))
                {
                    if (ImGui::IsItemActivated())
                    {
                        editParameter(S_MID, true);
                        if (ImGui::IsMouseDoubleClicked(0))
                            fs_mid = 0.0f;
                    }
                    setParameterValue(S_MID, fs_mid);
                }
                ImGui::PopID();
                ImGui::PopStyleColor(2);

                ImGui::Dummy(ImVec2(7.5f * scaleFactor, 0.0f)); ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)s_MidFreqColorActive);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)s_MidFreqColorHovered);
                ImGui::PushFont(smallFont);
                ImGui::PushID(S_MID_FREQ);
                if (ImGuiKnobs::Knob("Mid Freq", &fs_mid_freq, 313.3f, 5705.6f, hzstep, "%.1fHz", ImGuiKnobVariant_SteppedTick, seventy, ImGuiKnob_FlagsLog, 11))
                {
                    if (ImGui::IsItemActivated())
                    {
                        editParameter(S_MID_FREQ, true);
                        if (ImGui::IsMouseDoubleClicked(0))
                            fs_mid_freq = 1337.0f;
                    }
                    setParameterValue(S_MID_FREQ, fs_mid_freq);
                }
                ImGui::PopID();
                ImGui::PopFont();
                ImGui::PopStyleColor(2);

                ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)s_LowColorActive);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)s_LowColorHovered);
                ImGui::PushID(S_LOW);
                if (ImGuiKnobs::Knob("Low", &fs_low, -15.0f, 15.0, dbstep, "%.2fdB", ImGuiKnobVariant_SteppedTick, hundred, ImGuiKnob_FlagsDB, 7))
                {
                    if (ImGui::IsItemActivated())
                    {
                        editParameter(S_LOW, true);
                        if (ImGui::IsMouseDoubleClicked(0))
                            fs_low = 0.0f;
                    }
                    setParameterValue(S_LOW, fs_low);
                }
                ImGui::PopStyleColor(2);
            }
            ImGui::PopID();
            ImGui::EndGroup();

            if (ImGui::IsItemDeactivated())
            {
                editParameter(M_HIGH, false);
                editParameter(M_LOW, false);
                editParameter(M_MID, false);
                editParameter(M_MID_FREQ, false);
                editParameter(S_HIGH, false);
                editParameter(S_LOW, false);
                editParameter(S_MID, false);
                editParameter(S_MID_FREQ, false);
            }

            ImGui::PopFont();
        }
        ImGui::PopFont();
        ImGui::End();
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImGuiPluginUI)
};

// --------------------------------------------------------------------------------------------------------------------

UI* createUI()
{
    return new ImGuiPluginUI();
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
