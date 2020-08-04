#pragma once

#include <unordered_set>
#include <string>
#include "modloader/modloader.hpp"
#include "beatsaber-hook/utils/utils.h"
#include <functional>
#include "beatsaber-hook/utils/typedefs.h"
#include "beatsaber-hook/utils/il2cpp-utils.hpp"

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

    typedef Il2CppObject Asset;

    class AssetBundle : public Il2CppObject {
        typedef function_ptr_t<void, Asset*> AssetCallback;
        typedef function_ptr_t<void, AssetBundle*> AssetBundleCallback;

      public:
        AssetBundle() = delete;
        static bool LoadFromFileAsync(std::string_view filePath, AssetBundleCallback callback);
        static AssetBundle* LoadFromFile(std::string_view filePath);

        // Loads the asset with the given name and type from this bundle, then calls the given callback.
        // If not supplied, assetType will default to UnityEngine.GameObject.
        bool LoadAssetAsync(std::string_view assetName, AssetCallback callback, Il2CppReflectionType* assetType = nullptr);
        // Loads the asset with the given name and type from this bundle, blocking until finished.
        // If not supplied, assetType will default to UnityEngine.GameObject.
        Asset* LoadAsset(std::string_view assetName, Il2CppReflectionType* assetType = nullptr);
      private:
        static void AssetBundleComplete(AssetBundleCallback callback, Il2CppObject* assetBundleCreateRequest);
        static void AssetComplete(AssetCallback callback, Il2CppObject* assetBundleRequest);
    };
}
DEFINE_IL2CPP_ARG_TYPE(bs_utils::AssetBundle*, "UnityEngine", "AssetBundle");