#pragma once

#include <string>
#include <vector>

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Window.H>

#include "../SecRandom.h"

class AppController;

class StudentManageWindow : public Fl_Window {
public:
    explicit StudentManageWindow(AppController* controller);
    void refreshStudents();
    void applyTheme(bool dark);

private:
    AppController* controller = nullptr;
    std::vector<Candidate> students;
    std::string selected_id;

    Fl_Hold_Browser* student_browser = nullptr;
    Fl_Input* id_input = nullptr;
    Fl_Input* name_input = nullptr;
    Fl_Input* gender_input = nullptr;
    Fl_Input* group_input = nullptr;
    Fl_Button* add_button = nullptr;
    Fl_Button* update_button = nullptr;
    Fl_Button* delete_button = nullptr;
    Fl_Button* import_button = nullptr;
    Fl_Button* save_button = nullptr;
    Fl_Button* export_button = nullptr;
    Fl_Box* title_box = nullptr;
    Fl_Box* left_panel = nullptr;
    Fl_Box* right_panel = nullptr;
    Fl_Box* status_box = nullptr;
    std::vector<Fl_Box*> text_labels;
    std::string status_text;

    static void selectCallback(Fl_Widget*, void* data);
    static void addCallback(Fl_Widget*, void* data);
    static void updateCallback(Fl_Widget*, void* data);
    static void deleteCallback(Fl_Widget*, void* data);
    static void importCallback(Fl_Widget*, void* data);
    static void saveCallback(Fl_Widget*, void* data);
    static void exportCallback(Fl_Widget*, void* data);

    Candidate candidateFromInputs() const;
    void fillInputs(const Candidate& student);
    void setStatus(const std::string& text);
};
