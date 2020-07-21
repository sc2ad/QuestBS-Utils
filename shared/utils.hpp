#pragma once

#include <unordered_set>
#include <string>
#include "../extern/beatsaber-hook/include/modloader.hpp"
#include "../extern/beatsaber-hook/shared/utils/utils.h"
#include <functional>
#include "../extern/beatsaber-hook/shared/utils/typedefs.h"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp"

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
    class AssetImporter {
        public:
            Il2CppString* assetFilePath = nullptr;
            Il2CppString* assetName = nullptr;
            Il2CppReflectionType* assetType = nullptr;
            std::function<void(AssetImporter*)> whenDone;

            const bool pathExists = false;
            /// @brief Returns if the asset bundle has been loaded and is not null
            /// @return Asset bundle loaded
            const bool LoadedAssetBundle() const { return assetBundle; }
            /// @brief Returns if the asset has been loaded and is not null
            /// @return Asset loaded
            const bool LoadedAsset() const { return asset; }
            /// @brief Returns the loaded asset bundle
            /// @return Asset bundle
            const Il2CppObject* GetAssetBundle() const { return assetBundle; }
            /// @brief Returns loaded asset
            /// @return Asset
            const Il2CppObject* GetAsset() const { return asset; }

            bool SetNextAssetType(std::string_view assetNamespace, std::string_view assetClassName)
            {
                // TODO: instantly return true if assetType is already set to these params
                if (assetAsync && !asset)
                    return false;
                assetType = RET_0_UNLESS(il2cpp_utils::GetSystemType(assetNamespace, assetClassName));
                assetAsync = nullptr;
                asset = nullptr;
                return true;
            }

            AssetImporter(std::string_view assetBundlePath)
            {
                assetFilePath = RET_V_UNLESS(il2cpp_utils::createcsstr(assetBundlePath));
                pathExists = fileexists(assetBundlePath);
            }

            AssetImporter(std::string_view assetBundlePath, std::string_view assetNameS, std::string_view assetNamespace = "UnityEngine",
                std::string_view assetClassName = "GameObject")
                : AssetImporter(assetBundlePath)
            {
                assetName = RET_V_UNLESS(il2cpp_utils::createcsstr(assetNameS));
                SetNextAssetType(assetNamespace, assetClassName);
            }

            // Instantiates the asset using the provided namespace, class name, and method.
            // Returns the instantiated asset, or nullptr if it failed.
            Il2CppObject* InstantiateAsset(std::string_view nameSpace = "UnityEngine", std::string_view klass = "Object",
                std::string_view method = "Instantiate") const;

            // Returns true if the asset bundle load has been requested, false otherwise
            bool LoadAssetBundle(bool alsoLoadAsset = false);
            // Returns true if the asset load has been requested, false otherwise
            bool LoadAsset(std::string_view assetNameS = "");
            // Returns true if the callback was stored, false if it was immediately called
            bool RegisterOrDoCallback(decltype(whenDone) callback)
            {
                if (LoadedAsset()) {
                    callback(this);
                    return false;
                } else {
                    whenDone = callback;
                    return true;
                }
            }

        private:
            Il2CppObject* bundleAsync = nullptr;
            Il2CppObject* assetBundle = nullptr;
            Il2CppObject* assetAsync = nullptr;
            Il2CppObject* asset = nullptr;
            bool permissionToLoadAsset = false;

            static void AssetBundleComplete(AssetImporter* obj, Il2CppObject* asyncOp);
            static void AssetComplete(AssetImporter* obj, Il2CppObject* asyncOp);
    };
}