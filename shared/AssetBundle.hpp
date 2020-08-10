#pragma once

#include <functional>
#include <utility>

#include "utils-logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

namespace bs_utils {
    typedef Il2CppObject Asset;

    /// @class A C++ wrapper around the C# AssetBundle type.
    class AssetBundle : public Il2CppObject {
      public:
        AssetBundle() = delete;

        /// @brief Loads an asset bundle from the provided file path, blocking until finished.
        /// @param filePath The file path of the asset bundle to load.
        /// @return The AssetBundle* loaded (nullptr on failure).
        static AssetBundle* LoadFromFile(std::string_view filePath);

        /// @brief Loads the asset with the given name and type from this bundle, blocking until finished.
        /// @param assetName The name of the asset to load.
        /// @param assetType The Il2CppReflectionType* of the asset to load. Defaults to UnityEngine.GameObject if nullptr.
        /// @return The Asset* loaded (nullptr on failure).
        Asset* LoadAsset(std::string_view assetName, Il2CppReflectionType* assetType = nullptr);

        /// @brief Loads an asset bundle from the provided file path, then calls the given callback.
        /// @param filePath The file path of the asset bundle to load.
        /// @param callback The callback to perform after the asset bundle has been loaded.
        /// @return True if successful, false otherwise.
        template <typename RETURN_TYPE, typename... ARGS>
        static bool LoadFromFileAsync(std::string_view filePath, std::function<RETURN_TYPE(ARGS...)> callback) {
            auto* assetFilePath = RET_0_UNLESS(CreateIl2CppStringPathAndEnsureExists(filePath));

            auto* bundleAsync = RET_0_UNLESS(il2cpp_utils::RunMethod("UnityEngine", "AssetBundle", "LoadFromFileAsync", assetFilePath));
            RET_0_UNLESS(il2cpp_utils::SetPropertyValue(bundleAsync, "allowSceneActivation", true));

            auto method = RET_0_UNLESS(il2cpp_utils::FindMethodUnsafe(bundleAsync, "add_completed", 1));
            auto action = RET_0_UNLESS(il2cpp_utils::MakeAction(method, 0, new std::function(callback), AssetBundleComplete));

            RET_0_UNLESS(il2cpp_utils::RunMethod(bundleAsync, method, action));
            getLogger().info("Began loading asset bundle '%s' async.", filePath.data());
            return true;
        }

        /// @brief Loads the asset with the given name and type from this bundle, then calls the given callback.
        /// @param assetName The name of the asset to load.
        /// @param callback The callback to perform after the asset has been loaded.
        /// @param assetType The Il2CppReflectionType* of the asset to load. Defaults to UnityEngine.GameObject if nullptr.
        /// @return True if successful, false otherwise.
        template <typename RETURN_TYPE, typename... ARGS>
        bool LoadAssetAsync(std::string_view assetName, std::function<RETURN_TYPE(ARGS...)> callback,
                Il2CppReflectionType* assetType = nullptr) {
            if (!assetType) assetType = CRASH_UNLESS(il2cpp_utils::GetSystemType("UnityEngine", "GameObject"));
            auto* nameStr = CRASH_UNLESS(il2cpp_utils::createcsstr(assetName));
            getLogger().info("Loading asset '%s' asynchronously.", assetName.data());

            auto* assetAsync = RET_0_UNLESS(il2cpp_utils::RunMethod(this, "LoadAssetAsync", nameStr, assetType));

            auto* method = RET_0_UNLESS(il2cpp_utils::FindMethodUnsafe(assetAsync, "add_completed", 1));
            auto* action = RET_0_UNLESS(il2cpp_utils::MakeAction(method, 0, new std::function(callback), AssetComplete));

            RET_0_UNLESS(il2cpp_utils::RunMethod(assetAsync, method, action));
            getLogger().info("Began loading asset async");
            return true;
        }

      private:
        static Il2CppString* CreateIl2CppStringPathAndEnsureExists(std::string_view filePath);

        template <typename RETURN_TYPE, typename... ARGS>
        static void AssetBundleComplete(std::function<RETURN_TYPE(ARGS...)>* callback, Il2CppObject* assetBundleCreateRequest) {
            getLogger().info("UnityAssetLoader: AssetBundleCreateRequestComplete Called!");
            auto* assetBundle = RET_V_UNLESS(il2cpp_utils::GetPropertyValue<AssetBundle*>(assetBundleCreateRequest, "assetBundle"));
            (*callback)(assetBundle);
            il2cpp_utils::RunMethod(assetBundleCreateRequest, "Finalize");
            getLogger().info("UnityAssetLoader: AssetBundleCreateRequestComplete Finished!");
            delete callback;
        }

        template <typename RETURN_TYPE, typename... ARGS>
        static void AssetComplete(std::function<RETURN_TYPE(ARGS...)>* callback, Il2CppObject* assetBundleRequest) {
            getLogger().info("AssetBundle::AssetComplete called!");
            CRASH_UNLESS(il2cpp_utils::GetPropertyValue<bool>(assetBundleRequest, "isDone").value_or(false));
            auto* asset = RET_V_UNLESS(il2cpp_utils::GetPropertyValue<Asset*>(assetBundleRequest, "asset"));
            // RET_V_UNLESS(CheckAssetClass(asset));
            (*callback)(asset);
            il2cpp_utils::RunMethod(assetBundleRequest, "Finalize");
            getLogger().info("AssetBundle::AssetComplete finished!");
            delete callback;
        }
    };
}
DEFINE_IL2CPP_ARG_TYPE(bs_utils::AssetBundle*, "UnityEngine", "AssetBundle");
