#include "../../include/ui/StudentManageWindow.h"

#include "../../include/AppController.h"
#include "../../include/UIStyle.h"

#include <FL/Fl_Box.H>
#include <FL/Fl_File_Chooser.H>

StudentManageWindow::StudentManageWindow(AppController* controller)
    : Fl_Window(760, 480, "名单管理"), controller(controller) {
    UIStyle::applyWindow(this, false);

    title_box = new Fl_Box(24, 18, 200, 28, "名单管理");
    UIStyle::applyTitle(title_box, false, 22);

    left_panel = new Fl_Box(20, 56, 340, 360, "");
    right_panel = new Fl_Box(380, 56, 360, 360, "");
    UIStyle::applyPanel(left_panel, false);
    UIStyle::applyPanel(right_panel, false);

    text_labels.push_back(new Fl_Box(36, 74, 140, 24, "学生名单"));
    student_browser = new Fl_Hold_Browser(32, 104, 296, 290);
    UIStyle::applyBrowser(student_browser, false);
    student_browser->callback(selectCallback, this);

    text_labels.push_back(new Fl_Box(404, 76, 60, 24, "学号"));
    id_input = new Fl_Input(466, 72, 234, 30);
    text_labels.push_back(new Fl_Box(404, 118, 60, 24, "姓名"));
    name_input = new Fl_Input(466, 114, 234, 30);
    text_labels.push_back(new Fl_Box(404, 160, 60, 24, "性别"));
    gender_input = new Fl_Input(466, 156, 234, 30);
    text_labels.push_back(new Fl_Box(404, 202, 60, 24, "分组"));
    group_input = new Fl_Input(466, 198, 234, 30);

    add_button = new Fl_Button(404, 250, 72, 32, "新增");
    update_button = new Fl_Button(486, 250, 72, 32, "修改");
    delete_button = new Fl_Button(568, 250, 72, 32, "删除");
    import_button = new Fl_Button(404, 296, 96, 32, "导入");
    save_button = new Fl_Button(510, 296, 96, 32, "保存");
    export_button = new Fl_Button(616, 296, 96, 32, "导出");
    add_button->callback(addCallback, this);
    update_button->callback(updateCallback, this);
    delete_button->callback(deleteCallback, this);
    import_button->callback(importCallback, this);
    save_button->callback(saveCallback, this);
    export_button->callback(exportCallback, this);

    status_text = "修改后请点击保存名单。";
    status_box = new Fl_Box(34, 412, 688, 24, status_text.c_str());
    UIStyle::applyMutedText(status_box, false, 12);

    UIStyle::applyInput(id_input, false);
    UIStyle::applyInput(name_input, false);
    UIStyle::applyInput(gender_input, false);
    UIStyle::applyInput(group_input, false);
    UIStyle::applySecondaryButton(add_button, false);
    UIStyle::applySecondaryButton(update_button, false);
    UIStyle::applyDangerButton(delete_button, false);
    UIStyle::applySecondaryButton(import_button, false);
    UIStyle::applyPrimaryButton(save_button, false, UIStyle::theme(false).primary);
    UIStyle::applySecondaryButton(export_button, false);
    for (auto* label : text_labels) UIStyle::applyBodyText(label, false, 14);

    end();
    refreshStudents();
}

void StudentManageWindow::refreshStudents() {
    students = controller->getStudentsCopy();
    std::string keep_id = selected_id;
    student_browser->clear();
    int selected_index = 0;
    for (const auto& s : students) {
        std::string row = s.id + "\t" + s.name + "\t" + s.gender + "\t" + s.group_id;
        student_browser->add(row.c_str());
        if (!keep_id.empty() && s.id == keep_id) {
            selected_index = student_browser->size();
        }
    }
    if (selected_index > 0) {
        student_browser->select(selected_index);
        student_browser->value(selected_index);
    }
}

void StudentManageWindow::selectCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<StudentManageWindow*>(data);
    int index = self->student_browser->value();
    if (index <= 0 || index > (int)self->students.size()) return;
    const Candidate& student = self->students[index - 1];
    self->selected_id = student.id;
    self->fillInputs(student);
    self->setStatus("已选择：" + student.name + "，修改输入框后点击修改。");
}

void StudentManageWindow::addCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<StudentManageWindow*>(data);
    if (self->controller->addStudent(self->candidateFromInputs())) {
        self->setStatus("新增成功，记得保存名单。");
        self->refreshStudents();
    } else {
        self->setStatus("新增失败：学号或姓名为空，或学号已存在。");
    }
}

void StudentManageWindow::updateCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<StudentManageWindow*>(data);
    if (self->selected_id.empty()) {
        std::string id = self->id_input->value();
        for (const auto& student : self->students) {
            if (student.id == id) {
                self->selected_id = id;
                break;
            }
        }
        if (self->selected_id.empty()) {
            self->setStatus("请先点击左侧列表中的学生，或输入已有学号。");
            return;
        }
    }
    if (self->controller->updateStudent(self->selected_id, self->candidateFromInputs())) {
        self->selected_id = self->id_input->value();
        self->setStatus("修改成功，记得保存名单。");
        self->refreshStudents();
    } else {
        self->setStatus("修改失败：请检查学号是否重复。");
    }
}

void StudentManageWindow::deleteCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<StudentManageWindow*>(data);
    if (self->selected_id.empty()) {
        self->setStatus("请先选择要删除的学生。");
        return;
    }
    if (self->controller->deleteStudent(self->selected_id)) {
        self->selected_id.clear();
        self->id_input->value("");
        self->name_input->value("");
        self->gender_input->value("");
        self->group_input->value("");
        self->setStatus("删除成功，记得保存名单。");
        self->refreshStudents();
    }
}

void StudentManageWindow::importCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<StudentManageWindow*>(data);
    const char* path = fl_file_chooser("选择学生 CSV 文件", "*.csv", "../data/students.csv");
    if (!path) {
        self->setStatus("已取消导入。");
        return;
    }
    if (self->controller->importStudents(path)) {
        self->selected_id.clear();
        self->refreshStudents();
        self->setStatus("导入成功，已覆盖当前名单。");
    } else {
        self->setStatus("导入失败，请检查 CSV 格式。");
    }
}

void StudentManageWindow::saveCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<StudentManageWindow*>(data);
    self->setStatus(self->controller->saveStudents() ? "名单已保存。" : "名单保存失败。");
}

void StudentManageWindow::exportCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<StudentManageWindow*>(data);
    self->setStatus(self->controller->exportStudents() ? "已导出到 data/students_export.csv。" : "导出失败。");
}

Candidate StudentManageWindow::candidateFromInputs() const {
    Candidate student;
    student.id = id_input->value();
    student.name = name_input->value();
    student.gender = gender_input->value();
    student.group_id = group_input->value();
    return student;
}

void StudentManageWindow::fillInputs(const Candidate& student) {
    id_input->value(student.id.c_str());
    name_input->value(student.name.c_str());
    gender_input->value(student.gender.c_str());
    group_input->value(student.group_id.c_str());
}

void StudentManageWindow::setStatus(const std::string& text) {
    status_text = text;
    status_box->label(status_text.c_str());
    status_box->redraw();
}

void StudentManageWindow::applyTheme(bool dark) {
    UIStyle::applyWindow(this, dark);
    UIStyle::applyTitle(title_box, dark, 22);
    UIStyle::applyPanel(left_panel, dark);
    UIStyle::applyPanel(right_panel, dark);
    for (auto* label : text_labels) UIStyle::applyBodyText(label, dark, 14);
    UIStyle::applyBrowser(student_browser, dark);
    UIStyle::applyInput(id_input, dark);
    UIStyle::applyInput(name_input, dark);
    UIStyle::applyInput(gender_input, dark);
    UIStyle::applyInput(group_input, dark);
    UIStyle::applySecondaryButton(add_button, dark);
    UIStyle::applySecondaryButton(update_button, dark);
    UIStyle::applyDangerButton(delete_button, dark);
    UIStyle::applySecondaryButton(import_button, dark);
    UIStyle::applyPrimaryButton(save_button, dark, UIStyle::theme(dark).primary);
    UIStyle::applySecondaryButton(export_button, dark);
    UIStyle::applyMutedText(status_box, dark, 12);
    redraw();
}
