#include "../include/AppController.h"

#include <FL/Fl.H>
#include <FL/fl_draw.H>

#include <chrono>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sstream>

#include "../include/ui/HistoryWindow.h"
#include "../include/ui/MainWindow.h"
#include "../include/ui/SettingsWindow.h"
#include "../include/ui/StudentManageWindow.h"

using namespace std;
using namespace std::chrono;

namespace {
bool directoryExists(const string& path) {
    struct stat info;
    return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
}

string dataPath(const string& filename) {
    if (directoryExists("data")) return "data/" + filename;
    return "../data/" + filename;
}

string backupPath() {
    if (directoryExists("data")) return "data/backup";
    return "../data/backup";
}

const string kStudentsPath = dataPath("students.csv");
const string kStatsPath = dataPath("students_stats.csv");
const string kAttendancePath = dataPath("attendance.csv");
const string kDrawHistoryPath = dataPath("draw_history.csv");
const string kSettingsPath = dataPath("settings.csv");
const string kExportPath = dataPath("students_export.csv");
const string kBackupPath = backupPath();

bool ensureDirectory(const string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) == 0) return (info.st_mode & S_IFDIR) != 0;
    return mkdir(path.c_str(), 0755) == 0;
}

bool fileExists(const string& path) {
    ifstream file(path.c_str(), ios::binary);
    return file.good();
}

bool copyFile(const string& src, const string& dst) {
    ifstream in(src.c_str(), ios::binary);
    if (!in.is_open()) return false;
    ofstream out(dst.c_str(), ios::binary);
    if (!out.is_open()) return false;
    out << in.rdbuf();
    return in.good() && out.good();
}
}

AppController::AppController() {}

AppController::~AppController() = default;

bool AppController::initialize() {
    cout << "正在初始化点名库数据..." << endl;
    bool loaded = reloadStudents();
    SettingsStore store;
    store.load(kSettingsPath, settings);
    applySettingsToPicker();

    main_window.reset(new MainWindow(620, 430, "SecRandom 课堂抽签系统"));
    main_window->onRollCallRequested = [this]() { startRollCall(); };
    main_window->onLotteryRequested = [this]() { startLottery(); };
    main_window->onGroupLotteryRequested = [this](const string& group_name) { startGroupLottery(group_name); };
    main_window->onAttendanceSelected = [this](AttendanceStatus status) { recordAttendance(status); };
    main_window->onOpenStudentManager = [this]() { openStudentManagerWindow(); };
    main_window->onOpenHistory = [this]() { openHistoryWindow(); };
    main_window->onOpenSettings = [this]() { openSettingsWindow(); };
    main_window->applyTheme(settings.dark_mode);
    main_window->setGroupChoices(getCurrentGroupNames());

    if (loaded) {
        main_window->setStatusText("成功载入 " + to_string(data_manager.getCandidates().size()) + " 名学生");
    } else {
        main_window->setStatusText("花名册载入失败，请检查 data/students.csv");
    }
    return loaded;
}

int AppController::run(int argc, char** argv) {
    Fl::scheme("gtk+");
    if (!main_window) initialize();
    main_window->show(argc, argv);
    return Fl::run();
}

void AppController::startRollCall() {
    recorded_this_round = false;
    chosen_id.clear();
    chosen_name.clear();

    auto ids = picker.draw(data_manager.getCandidates(), 1, system_clock::now());
    if (ids.empty()) {
        main_window->setResultText("抽取失败：候选名单为空。");
        main_window->enableAttendanceButtons(false);
        return;
    }

    chosen_id = ids[0];
    Candidate* student = data_manager.findCandidateById(chosen_id);
    if (!student) return;

    chosen_name = student->name;
    updateSelectedStatistics(ids);
    data_manager.saveAlgorithmStats(kStatsPath);

    main_window->setResultText("天选之子\n\n" + buildStudentDisplayText(*student));
    main_window->setResultColor(student->gender == "男" ? fl_rgb_color(0, 122, 255) : fl_rgb_color(255, 105, 180));
    main_window->setStatusText("点名完成，请记录出勤状态。");
    main_window->resetAttendanceButtons();
    main_window->enableAttendanceButtons(true);
}

void AppController::startLottery() {
    auto ids = picker.draw(data_manager.getCandidates(), settings.lottery_count, system_clock::now());
    if (ids.empty()) {
        main_window->setResultText("抽奖失败：候选名单为空。");
        main_window->enableAttendanceButtons(false);
        return;
    }

    vector<string> names;
    stringstream display;
    display << "抽奖结果\n\n";
    for (size_t i = 0; i < ids.size(); ++i) {
        Candidate* student = data_manager.findCandidateById(ids[i]);
        if (!student) continue;
        names.push_back(student->name);
        display << (i + 1) << ". " << student->name << " (" << student->id << ")\n";
    }

    updateSelectedStatistics(ids);
    data_manager.saveAlgorithmStats(kStatsPath);
    data_manager.recordDrawHistory(kDrawHistoryPath, ids, names);

    main_window->setResultText(display.str());
    main_window->setResultColor(fl_rgb_color(88, 86, 214));
    main_window->setStatusText("抽奖完成，已写入抽奖历史。");
    main_window->enableAttendanceButtons(false);
}

void AppController::startGroupLottery(const string& group_name) {
    if (group_name.empty()) {
        main_window->setStatusText("请先选择一个分组。");
        return;
    }

    auto group_candidates = data_manager.getCandidatesByGroup(group_name);
    auto ids = picker.draw(group_candidates, settings.lottery_count, system_clock::now());
    if (ids.empty()) {
        main_window->setStatusText("分组抽奖失败。");
        return;
    }

    vector<string> names;
    stringstream display;
    display << "分组抽奖：" << group_name << "\n\n";
    for (size_t i = 0; i < ids.size(); ++i) {
        Candidate* student = data_manager.findCandidateById(ids[i]);
        if (!student) continue;
        names.push_back(student->name);
        display << (i + 1) << ". " << student->name << " (" << student->id << ")\n";
    }

    updateSelectedStatistics(ids);
    data_manager.saveAlgorithmStats(kStatsPath);
    data_manager.recordDrawHistory(kDrawHistoryPath, ids, names);

    main_window->setResultText(display.str());
    main_window->setResultColor(fl_rgb_color(69, 166, 150));
    main_window->setStatusText("分组抽奖完成，已写入抽奖历史。");
    main_window->enableAttendanceButtons(false);
}

void AppController::recordAttendance(AttendanceStatus status) {
    if (chosen_id.empty()) return;
    if (data_manager.recordAttendance(kAttendancePath, chosen_id, chosen_name, status, recorded_this_round)) {
        recorded_this_round = true;
        main_window->markAttendanceButton(status);
        main_window->setStatusText("出勤状态已保存。");
    }
}

bool AppController::reloadStudents() {
    bool ok = data_manager.loadStudentsFromCSV(kStudentsPath);
    if (ok) {
        if (!data_manager.loadAlgorithmStats(kStatsPath)) {
            data_manager.saveAlgorithmStats(kStatsPath);
        }
        data_manager.syncAlgorithmStatsWithStudents(kStatsPath);
    }
    data_manager.loadAttendanceLog(kAttendancePath);
    data_manager.loadDrawHistory(kDrawHistoryPath);
    return ok;
}

bool AppController::saveStudents() {
    bool ok = data_manager.saveStudentsToCSV(kStudentsPath);
    if (ok) {
        data_manager.syncAlgorithmStatsWithStudents(kStatsPath);
        if (main_window) main_window->setGroupChoices(getCurrentGroupNames());
    }
    return ok;
}

bool AppController::importStudents(const string& filepath) {
    bool ok = data_manager.loadStudentsFromCSV(filepath);
    if (!ok) return false;
    data_manager.saveStudentsToCSV(kStudentsPath);
    data_manager.syncAlgorithmStatsWithStudents(kStatsPath);
    if (main_window) {
        main_window->setGroupChoices(getCurrentGroupNames());
        main_window->setStatusText("名单已导入。");
    }
    return true;
}

bool AppController::exportStudents() {
    return data_manager.exportStudentsToCSV(kExportPath);
}

vector<Candidate> AppController::getStudentsCopy() const {
    return data_manager.getCandidates();
}

bool AppController::addStudent(const Candidate& student) {
    return data_manager.addStudent(student);
}

bool AppController::updateStudent(const string& original_id, const Candidate& student) {
    return data_manager.updateStudent(original_id, student);
}

bool AppController::deleteStudent(const string& id) {
    return data_manager.deleteStudent(id);
}

vector<string> AppController::getAttendanceHistory() const {
    return data_manager.getAttendanceLog();
}

vector<string> AppController::getDrawHistory() const {
    return data_manager.getDrawHistory();
}

bool AppController::clearAttendanceHistory() {
    return data_manager.clearAttendanceLog(kAttendancePath);
}

bool AppController::clearDrawHistory() {
    return data_manager.clearDrawHistory(kDrawHistoryPath);
}

bool AppController::backupData() {
    if (!ensureDirectory(kBackupPath)) return false;
    bool ok = true;
    ok = copyFile(kStudentsPath, kBackupPath + "/students.csv") && ok;
    ok = copyFile(kStatsPath, kBackupPath + "/students_stats.csv") && ok;
    ok = copyFile(kAttendancePath, kBackupPath + "/attendance.csv") && ok;
    ok = copyFile(kDrawHistoryPath, kBackupPath + "/draw_history.csv") && ok;
    ok = copyFile(kSettingsPath, kBackupPath + "/settings.csv") && ok;
    return ok;
}

bool AppController::restoreData() {
    bool ok = true;
    ok = fileExists(kBackupPath + "/students.csv") && copyFile(kBackupPath + "/students.csv", kStudentsPath) && ok;
    ok = fileExists(kBackupPath + "/students_stats.csv") && copyFile(kBackupPath + "/students_stats.csv", kStatsPath) && ok;
    ok = fileExists(kBackupPath + "/attendance.csv") && copyFile(kBackupPath + "/attendance.csv", kAttendancePath) && ok;
    ok = fileExists(kBackupPath + "/draw_history.csv") && copyFile(kBackupPath + "/draw_history.csv", kDrawHistoryPath) && ok;
    ok = fileExists(kBackupPath + "/settings.csv") && copyFile(kBackupPath + "/settings.csv", kSettingsPath) && ok;
    if (ok) {
        SettingsStore store;
        store.load(kSettingsPath, settings);
        reloadStudents();
        applySettingsToPicker();
        if (main_window) {
            main_window->applyTheme(settings.dark_mode);
            main_window->setGroupChoices(getCurrentGroupNames());
            main_window->setStatusText("数据已从备份恢复。");
        }
    }
    return ok;
}

AppSettings AppController::getSettings() const {
    return settings;
}

bool AppController::updateSettings(const AppSettings& new_settings) {
    settings = new_settings;
    if (settings.lottery_count < 1) settings.lottery_count = 1;
    if (settings.gap_threshold < 0) settings.gap_threshold = 0;
    if (settings.weight_level < 1) settings.weight_level = 1;
    if (settings.weight_level > 3) settings.weight_level = 3;
    applySettingsToPicker();
    SettingsStore store;
    bool ok = store.save(kSettingsPath, settings);
    if (main_window) main_window->applyTheme(settings.dark_mode);
    if (student_window) student_window->applyTheme(settings.dark_mode);
    if (history_window) history_window->applyTheme(settings.dark_mode);
    if (settings_window) settings_window->applyTheme(settings.dark_mode);
    return ok;
}

bool AppController::resetAlgorithmStats() {
    return data_manager.resetAlgorithmStats(kStatsPath);
}

void AppController::openStudentManagerWindow() {
    if (!student_window) {
        student_window.reset(new StudentManageWindow(this));
    }
    student_window->applyTheme(settings.dark_mode);
    student_window->refreshStudents();
    student_window->show();
}

void AppController::openHistoryWindow() {
    if (!history_window) {
        history_window.reset(new HistoryWindow(this));
    }
    history_window->applyTheme(settings.dark_mode);
    history_window->refreshHistory();
    history_window->show();
}

void AppController::openSettingsWindow() {
    if (!settings_window) {
        settings_window.reset(new SettingsWindow(this));
    }
    settings_window->applyTheme(settings.dark_mode);
    settings_window->loadFromController();
    settings_window->show();
}

void AppController::applySettingsToPicker() {
    picker.setFairMode(settings.fair_mode_enabled);
    picker.setAllowRepeat(settings.allow_repeat);
    picker.setGapThreshold(settings.gap_threshold);
    picker.setWeightLevel(settings.weight_level);
}

void AppController::updateSelectedStatistics(const vector<string>& selected_ids) {
    auto now = system_clock::now();
    for (const auto& id : selected_ids) {
        Candidate* student = data_manager.findCandidateById(id);
        if (!student) continue;
        student->total_count += 1;
        student->group_count += 1;
        student->gender_count += 1;
        student->last_drawn_time = now;
    }
}

string AppController::buildStudentDisplayText(const Candidate& student) const {
    return student.name + " (" + student.id + ")\n" + student.gender + " - " + student.group_id;
}

vector<string> AppController::getCurrentGroupNames() const {
    return data_manager.getGroupList();
}
