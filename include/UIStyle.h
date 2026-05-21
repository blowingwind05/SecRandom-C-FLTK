#pragma once

#include <FL/Fl_Browser.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Window.H>

namespace UIStyle {
struct Theme {
    Fl_Color window_bg;
    Fl_Color panel_bg;
    Fl_Color card_bg;
    Fl_Color border;
    Fl_Color title;
    Fl_Color text;
    Fl_Color muted;
    Fl_Color primary;
    Fl_Color secondary;
    Fl_Color success;
    Fl_Color warning;
    Fl_Color danger;
    Fl_Color input_bg;
    Fl_Color selection_bg;
};

const Theme& lightTheme();
const Theme& darkTheme();
const Theme& theme(bool dark);

void applyWindow(Fl_Window* window, bool dark);
void applyPanel(Fl_Widget* widget, bool dark);
void applyTitle(Fl_Widget* widget, bool dark, int size = 18);
void applyBodyText(Fl_Widget* widget, bool dark, int size = 14);
void applyMutedText(Fl_Widget* widget, bool dark, int size = 12);
void applyPrimaryButton(Fl_Button* button, bool dark, Fl_Color accent = 0);
void applySecondaryButton(Fl_Button* button, bool dark);
void applyDangerButton(Fl_Button* button, bool dark);
void applyInput(Fl_Input* input, bool dark);
void applyChoice(Fl_Choice* choice, bool dark);
void applyCheck(Fl_Check_Button* check, bool dark);
void applyBrowser(Fl_Browser* browser, bool dark);
void applyCardBox(Fl_Widget* widget, bool dark);
}
