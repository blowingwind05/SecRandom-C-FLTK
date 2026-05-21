#include "../include/Settings.h"

#include <fstream>
#include <sstream>

namespace {
bool parseBool(const std::string& value) {
    return value == "1" || value == "true" || value == "yes" || value == "on";
}

int parseIntOr(const std::string& value, int fallback) {
    try {
        return std::stoi(value);
    } catch (...) {
        return fallback;
    }
}
}

bool SettingsStore::load(const std::string& filepath, AppSettings& settings) const {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string key;
        std::string value;
        std::getline(ss, key, ',');
        std::getline(ss, value);

        if (key == "lottery_count") {
            settings.lottery_count = parseIntOr(value, settings.lottery_count);
        } else if (key == "fair_mode_enabled") {
            settings.fair_mode_enabled = parseBool(value);
        } else if (key == "allow_repeat") {
            settings.allow_repeat = parseBool(value);
        } else if (key == "gap_threshold") {
            settings.gap_threshold = parseIntOr(value, settings.gap_threshold);
        } else if (key == "weight_level") {
            settings.weight_level = parseIntOr(value, settings.weight_level);
        } else if (key == "dark_mode") {
            settings.dark_mode = parseBool(value);
        }
    }

    if (settings.lottery_count < 1) settings.lottery_count = 1;
    if (settings.gap_threshold < 0) settings.gap_threshold = 0;
    if (settings.weight_level < 1) settings.weight_level = 1;
    if (settings.weight_level > 3) settings.weight_level = 3;
    return true;
}

bool SettingsStore::save(const std::string& filepath, const AppSettings& settings) const {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;

    file << "lottery_count," << settings.lottery_count << "\n";
    file << "fair_mode_enabled," << (settings.fair_mode_enabled ? 1 : 0) << "\n";
    file << "allow_repeat," << (settings.allow_repeat ? 1 : 0) << "\n";
    file << "gap_threshold," << settings.gap_threshold << "\n";
    file << "weight_level," << settings.weight_level << "\n";
    file << "dark_mode," << (settings.dark_mode ? 1 : 0) << "\n";
    return true;
}
