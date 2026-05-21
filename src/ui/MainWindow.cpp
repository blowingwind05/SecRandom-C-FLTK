#include "../../include/ui/MainWindow.h"
#include "../../include/UIStyle.h"

MainWindow::MainWindow(int w, int h, const char* title) : Fl_Window(w, h, title) {
    UIStyle::applyWindow(this, false);

    Fl_Box* header_card = new Fl_Box(28, 18, w - 56, 72, "");
    Fl_Box* result_card = new Fl_Box(28, 104, w - 56, 190, "");
    card_boxes.push_back(header_card);
    card_boxes.push_back(result_card);

    Fl_Box* title_box = new Fl_Box(46, 28, 250, 28, "SecRandom");
    UIStyle::applyTitle(title_box, false, 24);
    title_box->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    title_widgets.push_back(title_box);

    result_box = new Fl_Box(46, 122, w - 92, 150, "等待开始...");
    UIStyle::applyCardBox(result_box, false);
    UIStyle::applyTitle(result_box, false, 24);
    result_box->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

    status_box = new Fl_Box(48, 58, w - 96, 24, "已加载系统");
    UIStyle::applyMutedText(status_box, false, 13);
    status_box->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

    roll_call_button = new Fl_Button(40, 312, 120, 42, "随机点名");
    lottery_button = new Fl_Button(175, 312, 120, 42, "随机抽奖");
    UIStyle::applyPrimaryButton(roll_call_button, false, UIStyle::theme(false).primary);
    UIStyle::applyPrimaryButton(lottery_button, false, UIStyle::theme(false).secondary);
    roll_call_button->callback(rollCallCallback, this);
    lottery_button->callback(lotteryCallback, this);

    group_choice = new Fl_Choice(312, 316, 116, 34);
    group_button = new Fl_Button(438, 316, 112, 34, "分组抽取");
    UIStyle::applyChoice(group_choice, false);
    UIStyle::applyPrimaryButton(group_button, false, UIStyle::theme(false).primary);
    group_button->callback(groupCallback, this);

    present_button = new Fl_Button(40, 365, 70, 34, "出勤");
    leave_button = new Fl_Button(118, 365, 70, 34, "请假");
    absent_button = new Fl_Button(196, 365, 70, 34, "缺勤");
    UIStyle::applySecondaryButton(present_button, false);
    UIStyle::applySecondaryButton(leave_button, false);
    UIStyle::applySecondaryButton(absent_button, false);
    present_button->labelcolor(UIStyle::theme(false).success);
    leave_button->labelcolor(UIStyle::theme(false).warning);
    absent_button->labelcolor(UIStyle::theme(false).danger);
    present_button->callback(presentCallback, this);
    leave_button->callback(leaveCallback, this);
    absent_button->callback(absentCallback, this);
    enableAttendanceButtons(false);

    students_button = new Fl_Button(312, 365, 100, 34, "名单管理");
    history_button = new Fl_Button(422, 365, 90, 34, "历史记录");
    settings_button = new Fl_Button(522, 365, 60, 34, "设置");
    UIStyle::applySecondaryButton(students_button, false);
    UIStyle::applySecondaryButton(history_button, false);
    UIStyle::applySecondaryButton(settings_button, false);
    students_button->callback(studentsCallback, this);
    history_button->callback(historyCallback, this);
    settings_button->callback(settingsCallback, this);

    resizable(this);
    end();
}

void MainWindow::setResultText(const std::string& text) {
    result_text = text;
    result_box->label(result_text.c_str());
    result_box->redraw();
}

void MainWindow::setResultColor(Fl_Color color) {
    result_box->labelcolor(color);
    result_box->redraw();
}

void MainWindow::setStatusText(const std::string& text) {
    status_text = text;
    status_box->label(status_text.c_str());
    status_box->redraw();
}

void MainWindow::setGroupChoices(const std::vector<std::string>& groups) {
    if (!group_choice) return;
    group_choice->clear();
    for (const auto& group : groups) group_choice->add(group.c_str());
    if (!groups.empty()) group_choice->value(0);
    group_choice->redraw();
}

void MainWindow::applyTheme(bool dark) {
    dark_theme = dark;
    const UIStyle::Theme& t = UIStyle::theme(dark);
    UIStyle::applyWindow(this, dark);
    for (auto* box : card_boxes) UIStyle::applyPanel(box, dark);
    for (auto* widget : title_widgets) UIStyle::applyTitle(widget, dark, 24);
    UIStyle::applyCardBox(result_box, dark);
    UIStyle::applyTitle(result_box, dark, 24);
    UIStyle::applyMutedText(status_box, dark, 13);
    UIStyle::applyPrimaryButton(roll_call_button, dark, t.primary);
    UIStyle::applyPrimaryButton(lottery_button, dark, t.secondary);
    UIStyle::applyPrimaryButton(group_button, dark, t.primary);
    UIStyle::applyChoice(group_choice, dark);
    UIStyle::applySecondaryButton(students_button, dark);
    UIStyle::applySecondaryButton(history_button, dark);
    UIStyle::applySecondaryButton(settings_button, dark);
    resetAttendanceButtons();
    redraw();
}

void MainWindow::enableAttendanceButtons(bool enabled) {
    if (enabled) {
        present_button->activate();
        leave_button->activate();
        absent_button->activate();
    } else {
        present_button->deactivate();
        leave_button->deactivate();
        absent_button->deactivate();
    }
}

void MainWindow::resetAttendanceButtons() {
    UIStyle::applySecondaryButton(present_button, dark_theme);
    UIStyle::applySecondaryButton(leave_button, dark_theme);
    UIStyle::applySecondaryButton(absent_button, dark_theme);
    present_button->labelcolor(UIStyle::theme(dark_theme).success);
    leave_button->labelcolor(UIStyle::theme(dark_theme).warning);
    absent_button->labelcolor(UIStyle::theme(dark_theme).danger);
    present_button->redraw();
    leave_button->redraw();
    absent_button->redraw();
}

void MainWindow::markAttendanceButton(AttendanceStatus status) {
    resetAttendanceButtons();
    Fl_Button* target = present_button;
    if (status == AttendanceStatus::Leave) target = leave_button;
    if (status == AttendanceStatus::Absent) target = absent_button;
    target->color(target->labelcolor());
    target->labelcolor(FL_WHITE);
    target->redraw();
}

void MainWindow::resize(int x, int y, int w, int h) {
    Fl_Window::resize(x, y, w, h);
    if (card_boxes.size() > 0 && card_boxes[0]) card_boxes[0]->resize(28, 18, w - 56, 72);
    if (card_boxes.size() > 1 && card_boxes[1]) card_boxes[1]->resize(28, 104, w - 56, h - 238);
    if (!title_widgets.empty() && title_widgets[0]) title_widgets[0]->resize(46, 28, 250, 28);
    if (status_box) status_box->resize(48, 58, w - 96, 24);
    if (result_box) result_box->resize(46, 122, w - 92, h - 274);

    int action_y = h - 118;
    if (roll_call_button) roll_call_button->resize(40, action_y, 120, 42);
    if (lottery_button) lottery_button->resize(175, action_y, 120, 42);
    if (group_choice) group_choice->resize(312, action_y + 4, 116, 34);
    if (group_button) group_button->resize(438, action_y + 4, 112, 34);

    int nav_y = h - 62;
    if (present_button) present_button->resize(40, nav_y, 70, 34);
    if (leave_button) leave_button->resize(118, nav_y, 70, 34);
    if (absent_button) absent_button->resize(196, nav_y, 70, 34);
    if (students_button) students_button->resize(w - 308, nav_y, 100, 34);
    if (history_button) history_button->resize(w - 198, nav_y, 90, 34);
    if (settings_button) settings_button->resize(w - 98, nav_y, 60, 34);
}

void MainWindow::rollCallCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<MainWindow*>(data);
    if (self->onRollCallRequested) self->onRollCallRequested();
}

void MainWindow::lotteryCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<MainWindow*>(data);
    if (self->onLotteryRequested) self->onLotteryRequested();
}

void MainWindow::groupCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<MainWindow*>(data);
    if (!self->onGroupLotteryRequested || !self->group_choice) return;
    const char* selected = self->group_choice->text();
    self->onGroupLotteryRequested(selected ? selected : "");
}

void MainWindow::presentCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<MainWindow*>(data);
    if (self->onAttendanceSelected) self->onAttendanceSelected(AttendanceStatus::Present);
}

void MainWindow::leaveCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<MainWindow*>(data);
    if (self->onAttendanceSelected) self->onAttendanceSelected(AttendanceStatus::Leave);
}

void MainWindow::absentCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<MainWindow*>(data);
    if (self->onAttendanceSelected) self->onAttendanceSelected(AttendanceStatus::Absent);
}

void MainWindow::studentsCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<MainWindow*>(data);
    if (self->onOpenStudentManager) self->onOpenStudentManager();
}

void MainWindow::historyCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<MainWindow*>(data);
    if (self->onOpenHistory) self->onOpenHistory();
}

void MainWindow::settingsCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<MainWindow*>(data);
    if (self->onOpenSettings) self->onOpenSettings();
}
