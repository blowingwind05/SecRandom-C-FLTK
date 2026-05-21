#include "../include/DataManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <algorithm>

namespace {
string formatTimePoint(const std::chrono::system_clock::time_point& time_point) {
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time_point.time_since_epoch());
    if (seconds.count() == 0) {
        return "未抽取";
    }

    auto time_value = std::chrono::system_clock::to_time_t(time_point);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_value), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::chrono::system_clock::time_point parseSavedTime(const string& saved_time) {
    if (saved_time.empty() || saved_time == "未抽取") {
        return std::chrono::system_clock::time_point();
    }

    try {
        size_t parsed_chars = 0;
        long long epoch_seconds = stoll(saved_time, &parsed_chars);
        if (parsed_chars == saved_time.size()) {
            return std::chrono::system_clock::time_point(std::chrono::seconds(epoch_seconds));
        }
    } catch (...) {
    }

    std::tm parsed_time = {};
    std::stringstream ss(saved_time);
    ss >> std::get_time(&parsed_time, "%Y-%m-%d %H:%M:%S");
    if (!ss.fail()) {
        return std::chrono::system_clock::from_time_t(std::mktime(&parsed_time));
    }

    return std::chrono::system_clock::time_point();
}
}

DataManager::DataManager() {}

bool DataManager::loadStudentsFromCSV(const string& filepath) {
    ifstream file(filepath);
    if (!file.is_open()) {
        cerr << "Failed to open " << filepath << endl;
        return false;
    }

    string line;
    // 跳过表头 (学号,姓名,性别,分组)
    getline(file, line);

    all_candidates.clear();
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        string id, name, gender, group;
        
        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, gender, ',');
        getline(ss, group, ',');
        
        Candidate c;
        c.id = id;
        c.name = name;
        c.gender = gender;
        c.group_id = group;
        c.total_count = 0;       // 刚载入的时候所有抽中次数都为0
        c.group_count = 0;
        c.gender_count = 0;
        c.last_drawn_time = std::chrono::system_clock::time_point(); // 初始时间为Epoch
        
        all_candidates.push_back(c);
    }
    file.close();
    return true;
}

bool DataManager::saveStudentsToCSV(const string& filepath) {
    ofstream file(filepath);
    if (!file.is_open()) return false;

    file << "学号,姓名,性别,分组\n";
    for (const auto& c : all_candidates) {
        file << c.id << "," << c.name << "," << c.gender << "," << c.group_id << "\n";
    }
    return true;
}

bool DataManager::exportStudentsToCSV(const string& filepath) {
    return saveStudentsToCSV(filepath);
}

bool DataManager::addStudent(const Candidate& student) {
    if (student.id.empty() || student.name.empty()) return false;
    if (findCandidateById(student.id) != nullptr) return false;

    Candidate copy = student;
    copy.total_count = 0;
    copy.group_count = 0;
    copy.gender_count = 0;
    copy.last_drawn_time = std::chrono::system_clock::time_point();
    all_candidates.push_back(copy);
    return true;
}

bool DataManager::updateStudent(const string& original_id, const Candidate& student) {
    if (student.id.empty() || student.name.empty()) return false;

    for (const auto& c : all_candidates) {
        if (c.id == student.id && c.id != original_id) return false;
    }

    Candidate* existing = findCandidateById(original_id);
    if (!existing) return false;

    existing->id = student.id;
    existing->name = student.name;
    existing->gender = student.gender;
    existing->group_id = student.group_id;
    return true;
}

bool DataManager::deleteStudent(const string& id) {
    auto old_size = all_candidates.size();
    all_candidates.erase(
        remove_if(all_candidates.begin(), all_candidates.end(), [&](const Candidate& c) {
            return c.id == id;
        }),
        all_candidates.end()
    );
    return all_candidates.size() != old_size;
}

vector<string> DataManager::getGroupList() const {
    vector<string> groups;
    for (const auto& c : all_candidates) {
        if (c.group_id.empty()) continue;
        if (find(groups.begin(), groups.end(), c.group_id) == groups.end()) {
            groups.push_back(c.group_id);
        }
    }
    return groups;
}

vector<Candidate> DataManager::getCandidatesByGroup(const string& group_name) const {
    vector<Candidate> result;
    for (const auto& c : all_candidates) {
        if (group_name.empty() || c.group_id == group_name) {
            result.push_back(c);
        }
    }
    return result;
}

// 把当前的所有统计信息读取并合并
bool DataManager::loadAlgorithmStats(const string& csv_path) {
    ifstream file(csv_path);
    if (!file.is_open()) return false; // 首次运行文件不存在很正常
    
    string line;
    // 跳过表头 (id,total_count,last_drawn_time)
    getline(file, line);
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        string id, total_str, time_str;
        getline(ss, id, ',');
        getline(ss, total_str, ',');
        getline(ss, time_str, ',');
        
        for (auto& c : all_candidates) {
            if (c.id == id) {
                try {
                    c.total_count = stoi(total_str);
                    c.last_drawn_time = parseSavedTime(time_str);
                } catch (...) {}
                break;
            }
        }
    }
    
    file.close();
    return true;
}

bool DataManager::saveAlgorithmStats(const string& csv_path) {
    ofstream file(csv_path);
    if (!file.is_open()) return false;
    
    // 写入表头
    file << "id,total_count,last_drawn_time\n";
    
    for (const auto& c : all_candidates) {
        file << c.id << "," << c.total_count << "," << formatTimePoint(c.last_drawn_time) << "\n";
    }
    
    file.close();
    return true;
}

bool DataManager::resetAlgorithmStats(const string& csv_path) {
    for (auto& c : all_candidates) {
        c.total_count = 0;
        c.group_count = 0;
        c.gender_count = 0;
        c.last_drawn_time = std::chrono::system_clock::time_point();
        c.current_weight = 0.0;
    }
    return saveAlgorithmStats(csv_path);
}

bool DataManager::syncAlgorithmStatsWithStudents(const string& csv_path) {
    std::ifstream file(csv_path);
    std::vector<std::pair<std::string, std::pair<int, std::chrono::system_clock::time_point>>> old_stats;

    if (file.is_open()) {
        std::string line;
        getline(file, line);
        while (getline(file, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string id, total_str, time_str;
            getline(ss, id, ',');
            getline(ss, total_str, ',');
            getline(ss, time_str, ',');
            int total = 0;
            try {
                total = stoi(total_str);
            } catch (...) {
                total = 0;
            }
            old_stats.push_back({id, {total, parseSavedTime(time_str)}});
        }
    }

    for (auto& c : all_candidates) {
        c.total_count = 0;
        c.group_count = 0;
        c.gender_count = 0;
        c.last_drawn_time = std::chrono::system_clock::time_point();
        c.current_weight = 0.0;

        for (const auto& item : old_stats) {
            if (item.first == c.id) {
                c.total_count = item.second.first;
                c.last_drawn_time = item.second.second;
                break;
            }
        }
    }

    return saveAlgorithmStats(csv_path);
}

bool DataManager::loadAttendanceLog(const string& csv_path) {
    attendance_log.clear();
    ifstream file(csv_path);
    if (!file.is_open()) return false;
    
    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            attendance_log.push_back(line);
        }
    }
    file.close();
    return true;
}

vector<string> DataManager::getAttendanceLog() const {
    return attendance_log;
}

bool DataManager::clearAttendanceLog(const string& csv_path) {
    attendance_log.clear();
    ofstream file(csv_path);
    return file.is_open();
}

bool DataManager::recordAttendance(const string& csv_path, const string& id, const string& name, AttendanceStatus status, bool is_update_last) {
    // 构造本次写盘的一行数据内容
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream time_ss;
    time_ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");

    string status_str = "未知";
    if (status == AttendanceStatus::Present) status_str = "出勤";
    if (status == AttendanceStatus::Leave)   status_str = "请假";
    if (status == AttendanceStatus::Absent)  status_str = "缺勤";

    string new_line = time_ss.str() + "," + id + "," + name + "," + status_str;

    // 根据模式决定是追加还是覆盖最后一条
    if (is_update_last && !attendance_log.empty()) {
        // 如果是反悔操作，直接覆盖内存中最后一个抽到的记录
        attendance_log.back() = new_line;
    } else {
        // 否则追加一个全新的记录
        attendance_log.push_back(new_line);
    }
    
    // 全覆盖方式刷新文件（因为文本通常很小，直接全写最容易）
    ofstream file(csv_path);
    if (!file.is_open()) return false;
    for (const auto& l : attendance_log) {
        file << l << "\n";
    }
    file.close();

    return true;
}

bool DataManager::loadDrawHistory(const string& csv_path) {
    draw_history_log.clear();
    ifstream file(csv_path);
    if (!file.is_open()) return false;

    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            draw_history_log.push_back(line);
        }
    }
    return true;
}

vector<string> DataManager::getDrawHistory() const {
    return draw_history_log;
}

bool DataManager::recordDrawHistory(const string& csv_path, const vector<string>& ids, const vector<string>& names) {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream time_ss;
    time_ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");

    string joined_ids;
    string joined_names;
    for (size_t i = 0; i < ids.size(); ++i) {
        if (i > 0) {
            joined_ids += "|";
            joined_names += "|";
        }
        joined_ids += ids[i];
        if (i < names.size()) joined_names += names[i];
    }

    string new_line = time_ss.str() + ",抽奖," + to_string(ids.size()) + "," + joined_ids + "," + joined_names;
    draw_history_log.push_back(new_line);

    ofstream file(csv_path);
    if (!file.is_open()) return false;
    for (const auto& l : draw_history_log) {
        file << l << "\n";
    }
    return true;
}

bool DataManager::clearDrawHistory(const string& csv_path) {
    draw_history_log.clear();
    ofstream file(csv_path);
    return file.is_open();
}

vector<Candidate>& DataManager::getCandidates() {
    return all_candidates;
}

const vector<Candidate>& DataManager::getCandidates() const {
    return all_candidates;
}

Candidate* DataManager::findCandidateById(const string& id) {
    for (auto& c : all_candidates) {
        if (c.id == id) return &c;
    }
    return nullptr;
}
