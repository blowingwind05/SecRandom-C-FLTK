#pragma once
#include <string>
#include <vector>
#include "StudentRecords.h"
#include "SecRandom.h" // 为了拿到 Candidate 结构 和 SecRandomPicker

using namespace std;

class DataManager {
public:
    DataManager();
    
    // 从 CSV 文件加载学生花名册
    bool loadStudentsFromCSV(const string& filepath);
    bool saveStudentsToCSV(const string& filepath);
    bool exportStudentsToCSV(const string& filepath);

    bool addStudent(const Candidate& student);
    bool updateStudent(const string& original_id, const Candidate& student);
    bool deleteStudent(const string& id);
    vector<string> getGroupList() const;
    vector<Candidate> getCandidatesByGroup(const string& group_name) const;
    
    // 数据持久化：读写 CSV 算法统计信息 (比如抽中次数、最后抽中时间)
    bool loadAlgorithmStats(const string& csv_path);
    bool saveAlgorithmStats(const string& csv_path);
    bool resetAlgorithmStats(const string& csv_path);
    bool syncAlgorithmStatsWithStudents(const string& csv_path);

    // 数据持久化：读取并加载已有的考勤流水账
    bool loadAttendanceLog(const string& csv_path);
    vector<string> getAttendanceLog() const;
    bool clearAttendanceLog(const string& csv_path);

    // 数据持久化：向 CSV 考勤报表里打卡（支持直接覆盖本轮最新一条记录）
    bool recordAttendance(const string& csv_path, const string& id, const string& name, AttendanceStatus status, bool is_update_last);

    bool loadDrawHistory(const string& csv_path);
    vector<string> getDrawHistory() const;
    bool recordDrawHistory(const string& csv_path, const vector<string>& ids, const vector<string>& names);
    bool clearDrawHistory(const string& csv_path);
    
    // 获取当下的所有学生数据作为抽取候选人
    vector<Candidate>& getCandidates();
    const vector<Candidate>& getCandidates() const;
    Candidate* findCandidateById(const string& id);

private:
    vector<Candidate> all_candidates;
    vector<string> attendance_log; // 内存暂存流水账，方便立即修改并覆写
    vector<string> draw_history_log;
};
