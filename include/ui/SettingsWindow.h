#pragma once

#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Window.H>

#include <string>
#include <vector>

#include "../Settings.h"

class AppController;

class SettingsWindow : public Fl_Window {
public:
    explicit SettingsWindow(AppController* controller);
    void loadFromController();
    void applyTheme(bool dark);

private:
    AppController* controller = nullptr;
    Fl_Int_Input* lottery_count_input = nullptr;
    Fl_Check_Button* fair_mode_check = nullptr;
    Fl_Check_Button* allow_repeat_check = nullptr;
    Fl_Check_Button* dark_mode_check = nullptr;
    Fl_Choice* weight_choice = nullptr;
    Fl_Choice* group_choice = nullptr;
    Fl_Button* reset_stats_button = nullptr;
    Fl_Button* backup_button = nullptr;
    Fl_Button* restore_button = nullptr;
    Fl_Int_Input* gap_threshold_input = nullptr;
    Fl_Button* save_button = nullptr;
    Fl_Box* title_box = nullptr;
    Fl_Box* panel = nullptr;
    Fl_Box* status_box = nullptr;
    std::vector<Fl_Box*> text_labels;
    std::string status_text;

    static void saveCallback(Fl_Widget*, void* data);
    AppSettings collectSettings() const;
    void setStatus(const char* text);
};
