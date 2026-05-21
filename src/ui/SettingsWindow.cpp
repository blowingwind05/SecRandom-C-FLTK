#include "../../include/ui/SettingsWindow.h"

#include "../../include/AppController.h"
#include "../../include/UIStyle.h"

#include <FL/Fl_Box.H>
#include <FL/Fl_Choice.H>
#include <cstdlib>

SettingsWindow::SettingsWindow(AppController* controller)
    : Fl_Window(520, 410, "设置"), controller(controller) {
    UIStyle::applyWindow(this, false);

    title_box = new Fl_Box(28, 18, 220, 28, "设置");
    UIStyle::applyTitle(title_box, false, 22);

    panel = new Fl_Box(20, 56, 480, 284, "");
    UIStyle::applyPanel(panel, false);

    text_labels.push_back(new Fl_Box(42, 82, 120, 24, "抽奖人数"));
    lottery_count_input = new Fl_Int_Input(168, 78, 120, 28);

    fair_mode_check = new Fl_Check_Button(42, 122, 180, 26, "启用公平算法");
    allow_repeat_check = new Fl_Check_Button(42, 154, 180, 26, "允许重复抽取");
    dark_mode_check = new Fl_Check_Button(42, 186, 180, 26, "深色主题");

    text_labels.push_back(new Fl_Box(250, 82, 120, 24, "权重等级"));
    weight_choice = new Fl_Choice(340, 78, 120, 28);
    weight_choice->add("轻柔");
    weight_choice->add("标准");
    weight_choice->add("稳健");

    text_labels.push_back(new Fl_Box(250, 124, 120, 24, "差距阈值"));
    gap_threshold_input = new Fl_Int_Input(340, 120, 120, 28);

    reset_stats_button = new Fl_Button(42, 236, 108, 32, "重置统计");
    backup_button = new Fl_Button(156, 236, 108, 32, "备份数据");
    restore_button = new Fl_Button(270, 236, 108, 32, "恢复数据");
    save_button = new Fl_Button(384, 236, 108, 32, "保存设置");
    save_button->callback(saveCallback, this);
    status_box = new Fl_Box(28, 350, 440, 20, "");
    UIStyle::applyMutedText(status_box, false, 12);
    reset_stats_button->callback([](Fl_Widget*, void* data) {
        auto* self = static_cast<SettingsWindow*>(data);
        self->controller->resetAlgorithmStats();
        self->setStatus("抽取统计已重置。");
    }, this);
    backup_button->callback([](Fl_Widget*, void* data) {
        auto* self = static_cast<SettingsWindow*>(data);
        self->setStatus(self->controller->backupData() ? "已生成备份。" : "备份失败。");
    }, this);
    restore_button->callback([](Fl_Widget*, void* data) {
        auto* self = static_cast<SettingsWindow*>(data);
        self->setStatus(self->controller->restoreData() ? "已恢复备份。" : "恢复失败。");
    }, this);

    UIStyle::applyInput(lottery_count_input, false);
    UIStyle::applyInput(gap_threshold_input, false);
    UIStyle::applyCheck(fair_mode_check, false);
    UIStyle::applyCheck(allow_repeat_check, false);
    UIStyle::applyCheck(dark_mode_check, false);
    UIStyle::applyChoice(weight_choice, false);
    for (auto* label : text_labels) UIStyle::applyBodyText(label, false, 14);
    UIStyle::applyDangerButton(reset_stats_button, false);
    UIStyle::applySecondaryButton(backup_button, false);
    UIStyle::applySecondaryButton(restore_button, false);
    UIStyle::applyPrimaryButton(save_button, false, UIStyle::theme(false).primary);
    title_box->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

    end();
    loadFromController();
}

void SettingsWindow::loadFromController() {
    AppSettings settings = controller->getSettings();
    lottery_count_input->value(std::to_string(settings.lottery_count).c_str());
    fair_mode_check->value(settings.fair_mode_enabled ? 1 : 0);
    allow_repeat_check->value(settings.allow_repeat ? 1 : 0);
    gap_threshold_input->value(std::to_string(settings.gap_threshold).c_str());
    dark_mode_check->value(settings.dark_mode ? 1 : 0);
    weight_choice->value(settings.weight_level - 1);
}

void SettingsWindow::saveCallback(Fl_Widget*, void* data) {
    auto* self = static_cast<SettingsWindow*>(data);
    self->setStatus(self->controller->updateSettings(self->collectSettings()) ? "设置已保存。" : "设置保存失败。");
}

AppSettings SettingsWindow::collectSettings() const {
    AppSettings settings;
    settings.lottery_count = std::atoi(lottery_count_input->value());
    settings.fair_mode_enabled = fair_mode_check->value() != 0;
    settings.allow_repeat = allow_repeat_check->value() != 0;
    settings.dark_mode = dark_mode_check->value() != 0;
    settings.weight_level = weight_choice->value() + 1;
    settings.gap_threshold = std::atoi(gap_threshold_input->value());
    settings.reset_stats = false;
    if (settings.lottery_count < 1) settings.lottery_count = 1;
    if (settings.gap_threshold < 0) settings.gap_threshold = 0;
    return settings;
}

void SettingsWindow::setStatus(const char* text) {
    status_text = text ? text : "";
    status_box->label(status_text.c_str());
    status_box->redraw();
}

void SettingsWindow::applyTheme(bool dark) {
    UIStyle::applyWindow(this, dark);
    UIStyle::applyTitle(title_box, dark, 22);
    UIStyle::applyPanel(panel, dark);
    for (auto* label : text_labels) UIStyle::applyBodyText(label, dark, 14);
    UIStyle::applyInput(lottery_count_input, dark);
    UIStyle::applyInput(gap_threshold_input, dark);
    UIStyle::applyCheck(fair_mode_check, dark);
    UIStyle::applyCheck(allow_repeat_check, dark);
    UIStyle::applyCheck(dark_mode_check, dark);
    UIStyle::applyChoice(weight_choice, dark);
    UIStyle::applyDangerButton(reset_stats_button, dark);
    UIStyle::applySecondaryButton(backup_button, dark);
    UIStyle::applySecondaryButton(restore_button, dark);
    UIStyle::applyPrimaryButton(save_button, dark, UIStyle::theme(dark).primary);
    UIStyle::applyMutedText(status_box, dark, 12);
    redraw();
}
