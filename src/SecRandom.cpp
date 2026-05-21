#include "../include/SecRandom.h"
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;
using namespace std::chrono;

SecRandomPicker::SecRandomPicker() {}

void SecRandomPicker::setFairMode(bool enabled) {
    fair_mode_enabled = enabled;
}

void SecRandomPicker::setAllowRepeat(bool enabled) {
    allow_repeat = enabled;
}

void SecRandomPicker::setGapThreshold(int threshold) {
    gap_threshold = max(0, threshold);
}

void SecRandomPicker::setWeightLevel(int level) {
    level = max(1, min(level, 3));
    if (level == 1) {
        frequency_weight = 0.8;
        time_weight = 0.8;
        group_weight = 0.7;
        gender_weight = 0.7;
        shield_duration_days = 1;
        min_weight = 0.1;
        max_weight = 4.0;
    } else if (level == 2) {
        frequency_weight = 1.0;
        time_weight = 1.0;
        group_weight = 1.0;
        gender_weight = 1.0;
        shield_duration_days = 1;
        min_weight = 0.1;
        max_weight = 5.0;
    } else {
        frequency_weight = 1.2;
        time_weight = 1.2;
        group_weight = 1.15;
        gender_weight = 1.15;
        shield_duration_days = 0;
        min_weight = 0.1;
        max_weight = 6.0;
    }
}

vector<string> SecRandomPicker::draw(vector<Candidate>& all_candidates, int N, system_clock::time_point current_time) {
        if (all_candidates.empty() || N <= 0) return {};
        if (!allow_repeat && all_candidates.size() <= N) {
            vector<string> result;
            for (const auto& c : all_candidates) result.push_back(c.id);
            return result;
        }

        random_device rd;
        mt19937 gen(rd());

        if (!fair_mode_enabled) {
            vector<string> result;
            if (allow_repeat) {
                uniform_int_distribution<> dist(0, (int)all_candidates.size() - 1);
                for (int i = 0; i < N; ++i) {
                    result.push_back(all_candidates[dist(gen)].id);
                }
                return result;
            }

            vector<int> indexes(all_candidates.size());
            iota(indexes.begin(), indexes.end(), 0);
            shuffle(indexes.begin(), indexes.end(), gen);
            for (int i = 0; i < N && i < (int)indexes.size(); ++i) {
                result.push_back(all_candidates[indexes[i]].id);
            }
            return result;
        }

        // --- 2. 第一阶段：硬公平过滤 (Avg-Gap Protection) ---
        vector<Candidate*> pool = filterCandidates(all_candidates, N);
        if (pool.empty()) return {};

        if (allow_repeat && pool.size() < (size_t)N) {
            pool.clear();
            for (auto& c : all_candidates) {
                pool.push_back(&c);
            }
        }

        // 获取用于计算权重的全局极值
        int max_total_count = 0;
        int max_group_count = 0;
        int max_gender_count = 0;
        for (const auto& c : all_candidates) {
            max_total_count = max(max_total_count, c.total_count);
            max_group_count = max(max_group_count, c.group_count);
            max_gender_count = max(max_gender_count, c.gender_count);
        }

        // --- 3. 第二阶段：多维动态权重计算 (Weighted Selection) ---
        vector<double> weights;
        for (auto* candidate : pool) {
            // 3.1 基础权重
            double w_base = 1.0;

            // 3.2 频率罚分因子 (F_penalty)
            double f_penalty = (sqrt(max_total_count + 1.0) / sqrt(candidate->total_count + 1.0)) * frequency_weight;
            if (all_candidates.size() < 10 && f_penalty < 0.8) {
                f_penalty = 0.8; // 冷启动保底修正
            }

            // 3.3 维度平衡因子
            double w_group = max_group_count > 0 ? (1.0 - (double)candidate->group_count / max_group_count) * group_weight : group_weight;
            double w_gender = max_gender_count > 0 ? (1.0 - (double)candidate->gender_count / max_gender_count) * gender_weight : gender_weight;

            // 3.4 时间遗忘增益 (T_gain)
            auto time_diff = duration_cast<hours>(current_time - candidate->last_drawn_time).count();
            double days_diff = time_diff / 24.0;
            double t_gain = min(1.0, days_diff / 30.0) * time_weight;

            // 3.5 最终权重合成
            double w_total = w_base + f_penalty + w_group + w_gender + t_gain;

            // --- 4. 第三阶段：屏蔽保护 ---
            if (days_diff < shield_duration_days) {
                w_total = 0.001; // 触发屏蔽保护，权重极小值
            }

            // 权重截断限制
            w_total = max(min_weight, min(w_total, max_weight));
            candidate->current_weight = w_total;
            weights.push_back(w_total);
        }

        // --- 4.2 权重抽取逻辑逻辑 ---
        vector<string> selected_ids;
        
        // 抽取 N 个人，抽取后将该人从本次池中剔除，防止重复抽取
        for (int i = 0; i < N && !pool.empty(); ++i) {
            discrete_distribution<> dist(weights.begin(), weights.end());
            int chosen_index = dist(gen);
            
            selected_ids.push_back(pool[chosen_index]->id);

            if (!allow_repeat) {
                pool.erase(pool.begin() + chosen_index);
                weights.erase(weights.begin() + chosen_index);
            }
        }

        return selected_ids;
    }

vector<Candidate*> SecRandomPicker::filterCandidates(vector<Candidate>& candidates, int N) {
        int max_c = candidates[0].total_count;
        int min_c = candidates[0].total_count;
        double sum_c = 0;

        for (const auto& c : candidates) {
            max_c = max(max_c, c.total_count);
            min_c = min(min_c, c.total_count);
            sum_c += c.total_count;
        }
        double avg = sum_c / candidates.size();

        vector<Candidate*> pool;
        int current_threshold = avg;

        // 差距保护限制
        if (max_c - min_c > gap_threshold) {
            sum_c = 0;
            int count = 0;
            for (auto& c : candidates) {
                if (c.total_count != max_c) { // 强制排除 max 
                    sum_c += c.total_count;
                    count++;
                }
            }
            if (count > 0) {
                current_threshold = sum_c / count;
            }
        }

        // 基础过滤
        for (auto& c : candidates) {
            if (max_c - min_c > gap_threshold && c.total_count == max_c) continue; 
            if (c.total_count <= current_threshold) {
                pool.push_back(&c);
            }
        }

        // 自动补齐 (向上放宽阈值)
        while (pool.size() < N && current_threshold <= max_c) {
            current_threshold++;
            pool.clear();
            for (auto& c : candidates) {
                if (c.total_count <= current_threshold) {
                    pool.push_back(&c);
                }
            }
        }

        return pool;
    }

/* 
// 示例用法:
int main() {
    vector<Candidate> candidates = {
        {{"ID01", "Alice", "F", "G1"}, 5, system_clock::now() - hours(24 * 10), 2, 3},
        {{"ID02", "Bob",   "M", "G1"}, 1, system_clock::now() - hours(24 * 40), 0, 1},
        {{"ID03", "Carol", "F", "G2"}, 8, system_clock::now() - hours(24 * 2),  4, 4},
        {{"ID04", "Dave",  "M", "G2"}, 2, system_clock::now() - hours(24 * 15), 1, 1}
    };

    SecRandomPicker picker;
    // 从上述 4 人中选取 2 人
    vector<string> result = picker.draw(candidates, 2, system_clock::now());

    cout << "Selected IDs: ";
    for (const string& id : result) cout << id << " ";
    cout << endl;

    return 0;
}
*/
