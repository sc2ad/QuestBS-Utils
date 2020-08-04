#pragma once

#include <unordered_set>
#include <string>
#include "modloader/modloader.hpp"
#include "beatsaber-hook/utils/utils.h"
#include <functional>
#include "beatsaber-hook/utils/typedefs.h"
#include "beatsaber-hook/utils/il2cpp-utils.hpp"

namespace bs_utils {
    /// @struct Information about the mod that is currentlying disabling score submission
    struct DisablingModInfo {
        // Id of the mod disabling
        std::string id;
        // Version of the mod disabling
        std::string version;
        /// @brief Construct a new disabling mod info from a ModInfo&
        /// @param info The ModInfo& to construct this from
        DisablingModInfo(const ModInfo& info) {
            id = info.id;
            version = info.version;
        }
        /// @brief Compare two DisablingModInfo objects
        /// @param info The other DisablingModInfo
        bool operator==(const DisablingModInfo& info) const {
            return id == info.id && version == info.version;
        }
    };
    /// @class Hash function for DisablingModInfos so it can be used in a map.
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
            /// @brief Disable score submission for the provided ModInfo&.
            /// @param info ModInfo& to disable score under.
            static void disable(const ModInfo& info);
            /// @brief Enable score submission for the provided ModInfo&.
            /// @param info ModInfo& to enable score under.
            static void enable(const ModInfo& info);
            /// @brief Get a set of all mods currently disabling score submission.
            /// @return The const unordered_set of DisablingModInfos
            static const std::unordered_set<DisablingModInfo, DisablingModInfoHash> getDisablingMods();
            /// @brief See if score submission is enabled.
            /// @return Whether score submission is enabled or not.
            static bool getEnabled();
        private:
            static std::unordered_set<DisablingModInfo, DisablingModInfoHash> disablingMods;
            static bool enabled;
    };

    typedef Il2CppObject Asset;

    /// @class A C++ wrapper around the C# AssetBundle type.
    class AssetBundle : public Il2CppObject {
        typedef function_ptr_t<void, Asset*> AssetCallback;
        typedef function_ptr_t<void, AssetBundle*> AssetBundleCallback;

      public:
        AssetBundle() = delete;
        /// @brief Loads an asset bundle from the provided file path, then calls the given callback.
        /// @param filePath The file path of the asset bundle to load.
        /// @param callback The callback to perform after the asset bundle has been loaded.
        /// @return True if successful, false otherwise.
        static bool LoadFromFileAsync(std::string_view filePath, AssetBundleCallback callback);
        /// @brief Loads an asset bundle from the provided file path, blocking until finished.
        /// @param filePath The file path of the asset bundle to load.
        /// @return The AssetBundle* loaded (nullptr on failure).
        static AssetBundle* LoadFromFile(std::string_view filePath);

        /// @brief Loads the asset with the given name and type from this bundle, then calls the given callback.
        /// @param assetName The name of the asset to load.
        /// @param callback The callback to perform after the asset has been loaded.
        /// @param assetType The Il2CppReflectionType* of the asset to load. Defaults to UnityEngine.GameObject if nullptr.
        /// @return True if successful, false otherwise.
        bool LoadAssetAsync(std::string_view assetName, AssetCallback callback, Il2CppReflectionType* assetType = nullptr);
        /// @brief Loads the asset with the given name and type from this bundle, blocking until finished.
        /// @param assetName The name of the asset to load.
        /// @param assetType The Il2CppReflectionType* of the asset to load. Defaults to UnityEngine.GameObject if nullptr.
        /// @return The Asset* loaded (nullptr on failure).
        Asset* LoadAsset(std::string_view assetName, Il2CppReflectionType* assetType = nullptr);
      private:
        static void AssetBundleComplete(AssetBundleCallback callback, Il2CppObject* assetBundleCreateRequest);
        static void AssetComplete(AssetCallback callback, Il2CppObject* assetBundleRequest);
    };
}
DEFINE_IL2CPP_ARG_TYPE(bs_utils::AssetBundle*, "UnityEngine", "AssetBundle");