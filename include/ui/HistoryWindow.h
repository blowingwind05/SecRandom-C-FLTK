#pragma once

#include <FL/Fl_Browser.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>

#include <vector>

class AppController;

class HistoryWindow : public Fl_Window {
public:
    explicit HistoryWindow(AppController* controller);
    void refreshHistory();
    void applyTheme(bool dark);

private:
    AppController* controller = nullptr;
    Fl_Browser* attendance_browser = nullptr;
    Fl_Browser* draw_browser = nullptr;
    Fl_Button* refresh_button = nullptr;
    Fl_Button* clear_attendance_button = nullptr;
    Fl_Button* clear_draw_button = nullptr;
    Fl_Box* title_box = nullptr;
    Fl_Box* left_panel = nullptr;
    Fl_Box* right_panel = nullptr;
    std::vector<Fl_Box*> text_labels;

    static void refreshCallback(Fl_Widget*, void* data);
    static void clearAttendanceCallback(Fl_Widget*, void* data);
    static void clearDrawCallback(Fl_Widget*, void* data);
};
