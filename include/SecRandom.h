#pragma once
#include <vector>
#include <string>
#include <chrono>
#include "StudentRecords.h"

// Candidate 继承自 Student，除了包含基础业务信息外，还包含算法需要的历史统计数据
struct Candidate : public Student {
    int total_count = 0;                                   // 总抽中次数
    std::chrono::system_clock::time_point last_drawn_time; // 上次抽中时间戳
    int group_count = 0;                                   // 在特定小组下被抽中的次数
    int gender_count = 0;                                  // 在特定性别下被抽中的次数
    
    // 算法计算过程中使用的临时状态
    double current_weight = 0.0;
};

class SecRandomPicker {
private:
    bool fair_mode_enabled = true;
    bool allow_repeat = false;
    int gap_threshold = 2;              // 差距保护限制
    double frequency_weight = 1.0;      // 频率权重系数
    double time_weight = 1.0;           // 时间权重系数
    double group_weight = 1.0;          // 小组权重系数
    double gender_weight = 1.0;         // 性别权重系数
    int shield_duration_days = 1;       // 屏蔽保护时长（天数）
    
    double min_weight = 0.1;
    double max_weight = 5.0;

    std::vector<Candidate*> filterCandidates(std::vector<Candidate>& candidates, int N);

public:
    SecRandomPicker();

    void setFairMode(bool enabled);
    void setAllowRepeat(bool enabled);
    void setGapThreshold(int threshold);
    void setWeightLevel(int level);

    // 核心抽取函数：从全部候选人中抽取 N 人
    std::vector<std::string> draw(std::vector<Candidate>& all_candidates, int N, std::chrono::system_clock::time_point current_time);
};
