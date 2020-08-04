// To use, simply add bs-utils.so to your Android.mk, and #include utils.hpp
#include "utils.hpp"
#include "utils-logging.hpp"
#include "beatsaber-hook/utils/logging.hpp"
#include <stdlib.h>

namespace bs_utils {
    std::unordered_set<DisablingModInfo, DisablingModInfoHash> Submission::disablingMods;
    bool Submission::enabled = true;
    void Submission::enable(const ModInfo& info) {
        auto itr = disablingMods.find(info);
        if (itr != disablingMods.end()) {
            disablingMods.erase(itr);
        }
        if (disablingMods.empty() && !enabled) {
            // Re-enable score submission
            getLogger().info("Mod: %s is enabling score submission!", info.id.c_str());
            setenv("disable_ss_upload", "0", true);
            enabled = true;
        }
    }
    void Submission::disable(const ModInfo& info) {
        if (disablingMods.find(info) != disablingMods.end()) {
            // Don't disable again if this mod has already disabled it once.
            getLogger().info("Mod: %s is disabling score submission multiple times!", info.id.c_str());
            return;
        }
        if (!disablingMods.empty() && enabled) {
            // Disable score submission
            getLogger().info("Mod: %s is disabling score submission!", info.id.c_str());
            setenv("disable_ss_upload", "1", true);
            // this will also disable vanilla score submission via main.cpp
            enabled = false;
        }
        disablingMods.insert(info);
    }
    const std::unordered_set<DisablingModInfo, DisablingModInfoHash> Submission::getDisablingMods() {
        return disablingMods;
    }
    const bool Submission::getEnabled() {
        return enabled;
    }
}