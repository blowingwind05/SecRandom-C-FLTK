#pragma once

#include <functional>
#include <string>
#include <vector>

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Window.H>

#include "../StudentRecords.h"

class MainWindow : public Fl_Window {
public:
    std::function<void()> onRollCallRequested;
    std::function<void()> onLotteryRequested;
    std::function<void(const std::string&)> onGroupLotteryRequested;
    std::function<void(AttendanceStatus)> onAttendanceSelected;
    std::function<void()> onOpenStudentManager;
    std::function<void()> onOpenHistory;
    std::function<void()> onOpenSettings;

    MainWindow(int w, int h, const char* title);

    void setResultText(const std::string& text);
    void setResultColor(Fl_Color color);
    void setStatusText(const std::string& text);
    void setGroupChoices(const std::vector<std::string>& groups);
    void applyTheme(bool dark);
    void enableAttendanceButtons(bool enabled);
    void resetAttendanceButtons();
    void markAttendanceButton(AttendanceStatus status);

    void resize(int x, int y, int w, int h) override;

private:
    Fl_Box* result_box = nullptr;
    Fl_Box* status_box = nullptr;
    Fl_Button* roll_call_button = nullptr;
    Fl_Button* lottery_button = nullptr;
    Fl_Button* group_button = nullptr;
    Fl_Choice* group_choice = nullptr;
    Fl_Button* present_button = nullptr;
    Fl_Button* leave_button = nullptr;
    Fl_Button* absent_button = nullptr;
    Fl_Button* students_button = nullptr;
    Fl_Button* history_button = nullptr;
    Fl_Button* settings_button = nullptr;
    std::vector<Fl_Box*> card_boxes;
    std::vector<Fl_Widget*> title_widgets;

    std::string result_text;
    std::string status_text;
    bool dark_theme = false;

    static void rollCallCallback(Fl_Widget*, void* data);
    static void lotteryCallback(Fl_Widget*, void* data);
    static void groupCallback(Fl_Widget*, void* data);
    static void presentCallback(Fl_Widget*, void* data);
    static void leaveCallback(Fl_Widget*, void* data);
    static void absentCallback(Fl_Widget*, void* data);
    static void studentsCallback(Fl_Widget*, void* data);
    static void historyCallback(Fl_Widget*, void* data);
    static void settingsCallback(Fl_Widget*, void* data);
};
