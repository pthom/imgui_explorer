// imgui_kit: small, standalone additions to Dear ImGui
// https://github.com/pthom/imgui_kit (MIT license)
//
// One header, imgui_kit.h. Its sources, beside it: imgui_kit.cpp, imgui_kit_layout.cpp, imgui_kit_widgets.cpp,
// and imgui_kit_demo.cpp (the demo, optional). They need only Dear ImGui (its docking or master branch, 1.93 or later).
// Define IMGUI_KIT_USE_STL to get overloads that take std::string and std::vector.

/*

Index of this file:
// [SECTION] Header mess
// [SECTION] Demo
// [SECTION] Em units
// [SECTION] Layout
// [SECTION] Widgets

*/

#pragma once

//-----------------------------------------------------------------------------
// [SECTION] Header mess
//-----------------------------------------------------------------------------

#define IMGUI_KIT_VERSION       "0.1.0"
#define IMGUI_KIT_VERSION_NUM   100     // major * 10000 + minor * 100 + patch

#include "imgui.h"

#ifdef IMGUI_KIT_USE_STL
#include <string>
#include <vector>
#endif

typedef int ImGuiKitIcon;   // -> enum ImGuiKitIcon_

namespace ImGuiKit
{

//-----------------------------------------------------------------------------
// [SECTION] Demo
//-----------------------------------------------------------------------------

// The kit's demo window, as ImGui::ShowDemoWindow(). Removed by IMGUI_KIT_DISABLE_DEMO.
IMGUI_API void ShowDemoWindow(bool* p_open = nullptr);
// The demo's content, in the current window (a host that places the window itself)
IMGUI_API void ShowDemoContents();

//-----------------------------------------------------------------------------
// [SECTION] Em units
//-----------------------------------------------------------------------------
// Sizes in multiples of the current font size, as the em unit of CSS: they follow the font,
// so they stay right on high-DPI screens and when the user zooms the text.

IMGUI_API float  EmSize();                          // The current font size, in pixels
IMGUI_API float  EmSize(float nb_lines);            // The height of nb_lines lines of text, in pixels
IMGUI_API ImVec2 EmToVec2(float x, float y);        // A size or a position in em, in pixels
IMGUI_API ImVec2 EmToVec2(ImVec2 v);
IMGUI_API ImVec2 PixelsToEm(ImVec2 pixels);         // Pixels, in em
IMGUI_API float  PixelSizeToEm(float pixel_size);

//-----------------------------------------------------------------------------
// [SECTION] Layout
//-----------------------------------------------------------------------------
// Dear ImGui sizes an item when it is called, before the items that follow it exist. The helpers below measure
// what they need on the previous frame (kept in the window's state storage), so a row can know the width of its
// end, or a group its own width, before drawing them. The first frame draws them hidden while they are measured.

// Fill row: the next item takes the line's width, minus the items after FillRowTail().
//
//     | [====== slider, it fills the line ======] [>] (i) |
//       ^ the fill item                           ^ the tail, on the right
//
// Every widget that takes its width from SetNextItemWidth() fills the line this way: sliders, drags, InputText,
// Combo, ColorEdit... A widget that takes a size argument instead (a plot, a child window) gets the width that
// BeginFillRow() returns. Without a fill row, the room for the tail must be computed by hand, and an error of a
// few pixels gives the window a horizontal scrollbar.
//
//     ImGuiKit::BeginFillRow("slice");
//     ImGui::SliderInt("##slice", &z, 0, 59);        // fills the line
//     ImGuiKit::FillRowTail();                        // the items after this call stay on the right
//     ImGui::ArrowButton("##next", ImGuiDir_Right);
//     ImGuiKit::EndFillRow();
IMGUI_API float  BeginFillRow(const char* str_id);   // Sets the next item's width, and returns it
IMGUI_API void   FillRowTail();
IMGUI_API void   EndFillRow();

// Aligned group: a group placed on the line from its width on the previous frame.
// align_x: 0.0 = left, 0.5 = centered, 1.0 = right. It aligns the group's box, not each line inside it.
// Inside a column (below), it aligns within the column's width instead of the line's.
//
//     |                         [Cancel] [Apply] |    <- BeginAlignedGroup("buttons", 1.0f)
IMGUI_API void   BeginAlignedGroup(const char* str_id, float align_x);
IMGUI_API void   EndAlignedGroup();

// Column: a group that knows its width (from the previous frame), so that aligned groups inside it align within it.
// Items under a knob, centered under it:
//
//     ImGuiKit::BeginColumn("clahe");
//     ImGuiKnobs::Knob("CLAHE", &clip, 1.0f, 6.0f);
//     ImGuiKit::BeginAlignedGroup("help", 0.5f);   // centered under the knob
//     ImGuiKit::HelpMarker("Contrast-limited adaptive histogram equalization...");
//     ImGuiKit::EndAlignedGroup();
//     ImGuiKit::EndColumn();
//     ImGui::SameLine();                             // the next column
IMGUI_API void   BeginColumn(const char* str_id);
IMGUI_API void   EndColumn();

// Label column: labels on the left, and the widgets after them aligned, at the widest label (of the previous frame).
// Label() starts a row; the rest of the row is a normal line.
// labels_align_x: 0.0 = left-aligned labels, 1.0 = right-aligned (as the x of the style's ButtonTextAlign).
//
//     Volume   [=======o========]
//     Output   [x] Muted  [Test sound]
//
//     ImGuiKit::BeginLabelColumn("audio");
//     ImGuiKit::Label("Volume");
//     ImGui::SliderFloat("##volume", &volume, 0.0f, 1.0f);
//     ImGuiKit::Label("Output");
//     ImGui::Checkbox("Muted", &muted);
//     ImGui::SameLine();
//     ImGui::Button("Test sound");
//     ImGuiKit::EndLabelColumn();
IMGUI_API void   BeginLabelColumn(const char* str_id, float labels_align_x = 0.0f);
IMGUI_API void   Label(const char* text);
IMGUI_API void   EndLabelColumn();

// Flow: items on the same line while they fit, then on the next one, as words in a paragraph.
// Call FlowNext() before each item (the first one too). An item wider than the line takes a line of its own.
//
//     | [Coral] [Lavender] [Mint]   |
//     | [Sunflower yellow] [Sky]    |
//
//     ImGuiKit::BeginFlow("presets");
//     for (int i = 0; i < count; i++)
//     {
//         ImGuiKit::FlowNext();
//         if (ImGui::Button(labels[i])) ...
//     }
//     ImGuiKit::EndFlow();
IMGUI_API void   BeginFlow(const char* str_id);
IMGUI_API void   FlowNext();
IMGUI_API void   EndFlow();

//-----------------------------------------------------------------------------
// [SECTION] Widgets
//-----------------------------------------------------------------------------

// Help marker: a small drawn (i). It shows its text:
//   - with a mouse: in a tooltip while hovered; a click pins it in a popup, until a click elsewhere or Escape;
//   - on a touch screen (which has no hover): in a popup, opened by a tap.
// width_em: the window's width, in em (a parameter named _em is in em; the kit's other sizes are in pixels, as
// Dear ImGui's); 0 = the text's width on one line, up to 35 em (Dear ImGui's tooltips do not wrap; its demo wraps
// its help at 35 em). Never wider than the screen. The height: at most 60% of the screen; a
// longer text scrolls in the popup, and the tooltip says to click to read it all.
// The text: '\n' starts a line with the default renderer; with a Markdown renderer, Markdown's rules apply (a
// single '\n' joins the lines of a paragraph, an empty line separates paragraphs).
// It goes to the next line when the current one has no room for it. Its ID is its text: two markers with the
// same text in the same ID scope share their popup (use PushID() or BeginHelpMarker() to tell them apart).
IMGUI_API void   HelpMarker(const char* text, float width_em = 0.0f);
// The same, with any content: draw it between the two calls when BeginHelpMarker() returns true.
// width_em: 0 = the content sizes the window (its text wraps at 35 em); give a width for a content that wraps to
// its window (Markdown).
//     if (ImGuiKit::BeginHelpMarker("##level_help")) { ImGui::Text("..."); ImGuiKit::EndHelpMarker(); }
IMGUI_API bool   BeginHelpMarker(const char* str_id, float width_em = 0.0f);
IMGUI_API void   EndHelpMarker();
// The width a help marker takes at the end of a line, with the spacing before it
IMGUI_API float  HelpMarkerWidth();
// How HelpMarker() draws its text. By default: wrapped plain text. An app can install a richer renderer
// (Dear ImGui Bundle installs one that renders Markdown). nullptr restores the default.
typedef void (*HelpMarkerTextRenderer)(const char* text);
IMGUI_API void   SetHelpMarkerTextRenderer(HelpMarkerTextRenderer renderer);

// Icon button: a button with a symbol drawn with the draw list, centered (a font's glyph often sits a little
// high in a button), in the text's color. size: (0, 0) = a square of the frame's height.
IMGUI_API bool   IconButton(const char* str_id, ImGuiKitIcon icon, const ImVec2& size = ImVec2(0, 0));

// Segmented switch: exclusive choices in one frame, the current one filled. Its corners follow the style's
// FrameRounding, as buttons and sliders do.
//
//     ( Demo | Code )        <- current = 1: "Code" is filled
//
// It reacts at the press, not the release, which suits touch screens. Returns true when *current changed.
IMGUI_API bool   SegmentedSwitch(const char* str_id, int* current, const char* const labels[], int labels_count);
#ifdef IMGUI_KIT_USE_STL
IMGUI_API bool   SegmentedSwitch(const char* str_id, int* current, const std::vector<std::string>& labels);
#endif

} // namespace ImGuiKit

// The symbols of IconButton()
enum ImGuiKitIcon_
{
    ImGuiKitIcon_Minus,
    ImGuiKitIcon_Plus,
    ImGuiKitIcon_ZoomIn,
    ImGuiKitIcon_ZoomOut,
    ImGuiKitIcon_ZoomOneToOne,
    ImGuiKitIcon_ZoomFit,
};
