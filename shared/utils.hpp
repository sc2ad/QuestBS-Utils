#pragma once

#include <unordered_set>
#include <string>
#include "../extern/beatsaber-hook/include/modloader.hpp"

namespace bs_utils {
    struct DisablingModInfo {
        std::string id;
        std::string version;
        DisablingModInfo(const ModInfo& info) {
            id = info.id;
            version = info.version;
        }
        bool operator==(const DisablingModInfo& info) const {
            return id == info.id && version == info.version;
        }
    };
    class DisablingModInfoHash
    {
        public:
            size_t operator()(const DisablingModInfo& info) const {
                return strHasher(info.id) ^ strHasher(info.version);
            }
        private:
            std::hash<std::string> strHasher;
    };
    class Submission {
        public:
            static void disable(const ModInfo& info);
            static void enable(const ModInfo& info);
            static const std::unordered_set<DisablingModInfo, DisablingModInfoHash> getDisablingMods();
            static const bool getEnabled();
        private:
            static std::unordered_set<DisablingModInfo, DisablingModInfoHash> disablingMods;
            static bool enabled;
    };
}