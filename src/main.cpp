#include "beatsaber-hook/utils/utils.h"
#include "beatsaber-hook/utils/typedefs.h"
#include "beatsaber-hook/utils/il2cpp-utils.hpp"
#include "utils-logging.hpp"
#include "utils.hpp"

MAKE_HOOK_OFFSETLESS(SoloFreePlayFlowCoordinator_ProcessScore, void, Il2CppObject* self, Il2CppObject* playerLevelStats, Il2CppObject* levelCompletionResults, Il2CppObject* difficultyBeatmap) {
    if (!bs_utils::Submission::getEnabled()) {
        getLogger().debug("Blocking vanilla score processing!");
        return;
    }
    getLogger().debug("Allowing vanilla score processing!");
    SoloFreePlayFlowCoordinator_ProcessScore(self, playerLevelStats, levelCompletionResults, difficultyBeatmap);
}

extern "C" void setup(ModInfo& info) {
    info.id = "BSUtils";
    info.version = VERSION;
}

extern "C" void load() {
    getLogger().debug("Installing BS-Utils!");
    INSTALL_HOOK_OFFSETLESS(SoloFreePlayFlowCoordinator_ProcessScore, il2cpp_utils::FindMethodUnsafe("", "SoloFreePlayFlowCoordinator", "ProcessScore", 3));
}