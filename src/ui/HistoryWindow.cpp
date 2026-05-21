#include "../../include/ui/HistoryWindow.h"

#include "../../include/AppController.h"
#include "../../include/UIStyle.h"

#include <FL/Fl_Box.H>

HistoryWindow::HistoryWindow(AppController* controller)
    : Fl_Window(780, 500, "历史记录"), controller(controller) {
    UIStyle::applyWindow(this, false);

    title_box = new Fl_Box(24, 18, 200, 28, "历史记录");
    UIStyle::applyTitle(title_box, false, 22);

    left_panel = new Fl_Box(20, 56, 360, 360, "");
    right_panel = new Fl_Box(400, 56, 360, 360, "");
    UIStyle::applyPanel(left_panel, false);
    UIStyle::applyPanel(right_panel, false);

    text_labels.push_back(new Fl_Box(36, 74, 160, 24, "点名 / 出勤历史"));
    attendance_browser = new Fl_Browser(32, 104, 304, 290);

    text_labels.push_back(new Fl_Box(416, 74, 160, 24, "抽奖历史"));
    draw_browser = new Fl_Browser(412, 104, 304, 290);

    refresh_button = new Fl_Button(20, 430, 96, 32, "刷新");
    clear_attendance_button = new Fl_Button(132, 430, 132, 32, "清空点名");
    clear_draw_button = new Fl_Button(280, 430, 132, 32, "清空抽奖");
    refresh_button->callback(refreshCallback, this);
    clear_attendance_button->callback(clearAttendanceCallback, this);
    clear_draw_button->callback(clearDrawCallback, this);

    UIStyle::applyBrowser(attendance_browser, false);
    UIStyle::applyBrowser(draw_browser, false);
    for (auto* label : text_labels) UIStyle::applyBodyText(label, false, 14);
    UIStyle::applySecondaryButton(refresh_button, false);
    UIStyle::applyDangerButton(clear_attendance_button, false);
    UIStyle::applyDangerButton(clear_draw_button, false);

    end();
    refreshHistory();
}

void HistoryWindow::refreshHistory() {
    attendance_browser->clear();
    draw_browser->clear();
    for (const auto& row : controller->getAttendanceHistory()) {
        attendance_browser->add(row.c_str());
    }
    for (const auto& row : controller->getDrawHistory()) {
        draw_browser->add(row.c_str());
    }
}

void HistoryWindow::refreshCallback(Fl_Widget*, void* data) {
    static_cast<HistoryWindow*>(data)->refreshHistory();
}

void HistoryWindow::clearAttendanceCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<HistoryWindow*>(data);
    self->controller->clearAttendanceHistory();
    self->refreshHistory();
}

void HistoryWindow::clearDrawCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<HistoryWindow*>(data);
    self->controller->clearDrawHistory();
    self->refreshHistory();
}

void HistoryWindow::applyTheme(bool dark) {
    UIStyle::applyWindow(this, dark);
    UIStyle::applyTitle(title_box, dark, 22);
    UIStyle::applyPanel(left_panel, dark);
    UIStyle::applyPanel(right_panel, dark);
    for (auto* label : text_labels) UIStyle::applyBodyText(label, dark, 14);
    UIStyle::applyBrowser(attendance_browser, dark);
    UIStyle::applyBrowser(draw_browser, dark);
    UIStyle::applySecondaryButton(refresh_button, dark);
    UIStyle::applyDangerButton(clear_attendance_button, dark);
    UIStyle::applyDangerButton(clear_draw_button, dark);
    redraw();
}
