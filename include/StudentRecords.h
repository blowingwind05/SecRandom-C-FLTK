#pragma once
#include <string>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

// --- 0.1 评价与记录数据结构 ---
enum class RecordType {
    Attendance, // 出勤/点名模式
    Score       // 打分模式
};

enum class AttendanceStatus {
    Present,    // 已到
    Absent,     // 未到
    Leave       // 请假
};

// 单次表现记录
struct Record {
    system_clock::time_point time;
    RecordType type;
    AttendanceStatus attendance_status; // 仅在 Attendance 模式下有效
    double score;                       // 仅在 Score 模式下有效
    string comment;                     // 补充备注
};

// --- 0.2 基础业务数据结构 ---
struct Student {
    string id;                   // 学号
    string name;                 // 姓名
    string gender;               // 性别
    string group_id;             // 所在小组ID

    vector<Record> records;      // 该学生的所有历史点名/打分记录

    // 辅助工具：计算出勤率
    double getAttendanceRate() const {
        int total = 0, present = 0;
        for (const auto& r : records) {
            if (r.type == RecordType::Attendance) {
                total++;
                if (r.attendance_status == AttendanceStatus::Present) present++;
            }
        }
        return total == 0 ? 1.0 : (double)present / total; // 没有记录默认 100%
    }

    // 辅助工具：计算打分平均分
    double getAverageScore() const {
        int total = 0;
        double sum = 0.0;
        for (const auto& r : records) {
            if (r.type == RecordType::Score) {
                total++;
                sum += r.score;
            }
        }
        return total == 0 ? 0.0 : sum / total;
    }
};