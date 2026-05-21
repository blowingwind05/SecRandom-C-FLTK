#pragma once

#include <string>

struct AppSettings {
    int lottery_count = 3;
    bool fair_mode_enabled = true;
    bool allow_repeat = false;
    int gap_threshold = 2;
    bool reset_stats = false;
    int weight_level = 1;
    bool dark_mode = false;
};

class SettingsStore {
public:
    bool load(const std::string& filepath, AppSettings& settings) const;
    bool save(const std::string& filepath, const AppSettings& settings) const;
};
