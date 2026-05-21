#include "../include/UIStyle.h"

#include <FL/Fl_Box.H>

namespace UIStyle {
namespace {
const Theme kLight{
    fl_rgb_color(246, 251, 250),
    fl_rgb_color(238, 248, 246),
    fl_rgb_color(255, 255, 255),
    fl_rgb_color(214, 232, 229),
    fl_rgb_color(31, 55, 61),
    fl_rgb_color(50, 71, 76),
    fl_rgb_color(116, 135, 139),
    fl_rgb_color(69, 166, 150),
    fl_rgb_color(121, 143, 232),
    fl_rgb_color(73, 181, 126),
    fl_rgb_color(240, 169, 75),
    fl_rgb_color(232, 91, 91),
    fl_rgb_color(250, 253, 252),
    fl_rgb_color(213, 242, 236)
};

const Theme kDark{
    fl_rgb_color(24, 34, 38),
    fl_rgb_color(32, 46, 51),
    fl_rgb_color(39, 56, 62),
    fl_rgb_color(68, 88, 94),
    fl_rgb_color(238, 248, 246),
    fl_rgb_color(222, 235, 232),
    fl_rgb_color(163, 181, 184),
    fl_rgb_color(80, 190, 170),
    fl_rgb_color(137, 151, 238),
    fl_rgb_color(84, 202, 141),
    fl_rgb_color(245, 178, 88),
    fl_rgb_color(242, 102, 102),
    fl_rgb_color(30, 43, 48),
    fl_rgb_color(46, 118, 108)
};
}

const Theme& lightTheme() { return kLight; }
const Theme& darkTheme() { return kDark; }
const Theme& theme(bool dark) { return dark ? kDark : kLight; }

void applyWindow(Fl_Window* window, bool dark) {
    if (!window) return;
    window->color(theme(dark).window_bg);
}

void applyPanel(Fl_Widget* widget, bool dark) {
    if (!widget) return;
    const Theme& t = theme(dark);
    widget->box(FL_ROUNDED_BOX);
    widget->color(t.panel_bg);
    widget->labelcolor(t.text);
}

void applyTitle(Fl_Widget* widget, bool dark, int size) {
    if (!widget) return;
    widget->labelsize(size);
    widget->labelfont(FL_HELVETICA_BOLD);
    widget->labelcolor(theme(dark).title);
}

void applyBodyText(Fl_Widget* widget, bool dark, int size) {
    if (!widget) return;
    widget->labelsize(size);
    widget->labelcolor(theme(dark).text);
}

void applyMutedText(Fl_Widget* widget, bool dark, int size) {
    if (!widget) return;
    widget->labelsize(size);
    widget->labelcolor(theme(dark).muted);
}

void applyPrimaryButton(Fl_Button* button, bool dark, Fl_Color accent) {
    if (!button) return;
    const Theme& t = theme(dark);
    Fl_Color base = accent ? accent : t.primary;
    button->box(FL_ROUND_UP_BOX);
    button->color(base);
    button->down_color(fl_darker(base));
    button->labelcolor(FL_WHITE);
    button->labelsize(15);
    button->labelfont(FL_HELVETICA_BOLD);
}

void applySecondaryButton(Fl_Button* button, bool dark) {
    if (!button) return;
    const Theme& t = theme(dark);
    button->box(FL_ROUND_UP_BOX);
    button->color(t.card_bg);
    button->down_color(t.panel_bg);
    button->labelcolor(t.text);
    button->labelsize(14);
}

void applyDangerButton(Fl_Button* button, bool dark) {
    if (!button) return;
    const Theme& t = theme(dark);
    button->box(FL_ROUND_UP_BOX);
    button->color(t.danger);
    button->down_color(fl_darker(t.danger));
    button->labelcolor(FL_WHITE);
    button->labelsize(14);
}

void applyInput(Fl_Input* input, bool dark) {
    if (!input) return;
    const Theme& t = theme(dark);
    input->box(FL_ROUNDED_BOX);
    input->color(t.input_bg);
    input->textcolor(t.text);
    input->selection_color(t.selection_bg);
    input->labelcolor(t.text);
    input->textsize(14);
}

void applyChoice(Fl_Choice* choice, bool dark) {
    if (!choice) return;
    const Theme& t = theme(dark);
    choice->box(FL_ROUNDED_BOX);
    choice->color(t.input_bg);
    choice->textcolor(t.text);
    choice->selection_color(t.selection_bg);
    choice->labelcolor(t.text);
    choice->textsize(14);
}

void applyCheck(Fl_Check_Button* check, bool dark) {
    if (!check) return;
    const Theme& t = theme(dark);
    check->labelcolor(t.text);
    check->down_color(t.primary);
    check->selection_color(t.primary);
    check->labelsize(14);
}

void applyBrowser(Fl_Browser* browser, bool dark) {
    if (!browser) return;
    const Theme& t = theme(dark);
    browser->box(FL_ROUNDED_BOX);
    browser->color(t.card_bg);
    browser->textcolor(t.text);
    browser->selection_color(t.selection_bg);
    browser->scrollbar_size(12);
    browser->textsize(14);
}

void applyCardBox(Fl_Widget* widget, bool dark) {
    if (!widget) return;
    const Theme& t = theme(dark);
    widget->box(FL_ROUNDED_BOX);
    widget->color(t.card_bg);
    widget->labelcolor(t.text);
}
}
