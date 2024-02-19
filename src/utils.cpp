// To use, simply add bs-utils.so to your Android.mk, and #include utils.hpp
#include "utils.hpp"
#include "utils-logging.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include <stdlib.h>

#include <mutex>

#ifndef PERSISTENT_DIR
#define PERSISTENT_DIR "/sdcard/ModData/%s/Mods/%s/"
#endif

static std::mutex submissionMutex;
static std::mutex initMutex;
static std::mutex enabledMutex;

MAKE_HOOK_FIND_CLASS_UNSAFE_STATIC (LevelCompletionResultsHelper_ProcessScore, "", "LevelCompletionResultsHelper", "ProcessScore", void, Il2CppObject* playerData, Il2CppObject* playerLevelStats, Il2CppObject* levelCompletionResults, Il2CppObject* transformedBeatmapData, Il2CppObject* difficultyBeatmap, Il2CppObject* platformLeaderboardsModel) {
    if (!bs_utils::Submission::getEnabled()) {
        getLogger().debug("Blocking vanilla score processing!");
        return;
    }
    getLogger().debug("Allowing vanilla score processing!");
    LevelCompletionResultsHelper_ProcessScore(playerData, playerLevelStats, levelCompletionResults, transformedBeatmapData, difficultyBeatmap, platformLeaderboardsModel);
}

namespace bs_utils {
    std::string getDataDir(const modloader::ModInfo& info) {
        auto path = string_format(PERSISTENT_DIR, modloader::get_application_id().c_str(), info.id.c_str());
        mkpath(path);
        return path;
    }

    std::unordered_set<DisablingModInfo, DisablingModInfoHash> Submission::disablingMods;
    bool Submission::enabled = true;
    bool Submission::initialized = false;

    void Submission::init() {
        if (!initialized) {
            initMutex.lock();
            INSTALL_HOOK_ORIG(getLogger(), LevelCompletionResultsHelper_ProcessScore);
            initialized = true;
            initMutex.unlock();
        }
    }

    void Submission::enable(const modloader::ModInfo& info) {
        init();
        submissionMutex.lock();
        auto itr = disablingMods.find(info);
        if (itr != disablingMods.end()) {
            disablingMods.erase(itr);
        }
        if (disablingMods.empty() && !enabled) {
            submissionMutex.unlock();
            // Re-enable score submission
            enabledMutex.lock();
            getLogger().info("Mod: %s is enabling score submission!", info.id.c_str());
            setenv("disable_ss_upload", "0", true);
            enabled = true;
            enabledMutex.unlock();
        } else {
            submissionMutex.unlock();
        }
    }
    void Submission::disable(const modloader::ModInfo& info) {
        init();
        submissionMutex.lock();
        if (disablingMods.find(info) != disablingMods.end()) {
            submissionMutex.unlock();
            // Don't disable again if this mod has already disabled it once.
            getLogger().info("Mod: %s is disabling score submission multiple times!", info.id.c_str());
            return;
        }
        disablingMods.insert(info);
        submissionMutex.unlock();
        if (enabled) {
            // Disable score submission
            enabledMutex.lock();
            getLogger().info("Mod: %s is disabling score submission!", info.id.c_str());
            setenv("disable_ss_upload", "1", true);
            // this will also disable vanilla score submission via main.cpp
            enabled = false;
            enabledMutex.unlock();
        }
    }
    const std::unordered_set<DisablingModInfo, DisablingModInfoHash> Submission::getDisablingMods() {
        return disablingMods;
    }
    bool Submission::getEnabled() {
        return enabled;
    }
}
