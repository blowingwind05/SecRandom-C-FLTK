#pragma once

#include <memory>
#include <string>
#include <vector>

#include "DataManager.h"
#include "SecRandom.h"
#include "Settings.h"

class MainWindow;
class StudentManageWindow;
class HistoryWindow;
class SettingsWindow;

class AppController {
public:
    AppController();
    ~AppController();

    bool initialize();
    int run(int argc, char** argv);

    void startRollCall();
    void startLottery();
    void startGroupLottery(const std::string& group_name);
    void recordAttendance(AttendanceStatus status);

    bool reloadStudents();
    bool saveStudents();
    bool importStudents(const std::string& filepath);
    bool exportStudents();

    std::vector<Candidate> getStudentsCopy() const;
    bool addStudent(const Candidate& student);
    bool updateStudent(const std::string& original_id, const Candidate& student);
    bool deleteStudent(const std::string& id);

    std::vector<std::string> getAttendanceHistory() const;
    std::vector<std::string> getDrawHistory() const;
    bool clearAttendanceHistory();
    bool clearDrawHistory();
    bool backupData();
    bool restoreData();

    AppSettings getSettings() const;
    bool updateSettings(const AppSettings& new_settings);
    bool resetAlgorithmStats();

    void openStudentManagerWindow();
    void openHistoryWindow();
    void openSettingsWindow();

private:
    void applySettingsToPicker();
    void updateSelectedStatistics(const std::vector<std::string>& selected_ids);
    std::vector<std::string> getCurrentGroupNames() const;
    std::string buildStudentDisplayText(const Candidate& student) const;

    DataManager data_manager;
    SecRandomPicker picker;
    AppSettings settings;

    std::unique_ptr<MainWindow> main_window;
    std::unique_ptr<StudentManageWindow> student_window;
    std::unique_ptr<HistoryWindow> history_window;
    std::unique_ptr<SettingsWindow> settings_window;

    std::string chosen_id;
    std::string chosen_name;
    bool recorded_this_round = false;
};
