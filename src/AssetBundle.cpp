#include "utils.hpp"
#include "utils-logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

namespace bs_utils {
    bool CheckAssetClass(Il2CppObject* asset) {
        auto* cAsset = CRASH_UNLESS(il2cpp_functions::object_get_class(asset));
        if (cAsset == il2cpp_functions::defaults->string_class) {
            Il2CppString* str = reinterpret_cast<Il2CppString*>(asset);
            getLogger().error("asset is string?! '%s'", to_utf8(csstrtostr(str)).c_str());
            return false;
        } else {
            getLogger().info("asset is %s ?", il2cpp_utils::ClassStandardName(cAsset).c_str());
        }
        return true;
    }

    void AssetBundle::AssetComplete(AssetCallback callback, Il2CppObject* assetBundleRequest) {
        getLogger().info("AssetBundle::AssetComplete called!");
        CRASH_UNLESS(il2cpp_utils::GetPropertyValue<bool>(assetBundleRequest, "isDone").value_or(false));
        auto* asset = RET_V_UNLESS(il2cpp_utils::GetPropertyValue<Asset*>(assetBundleRequest, "asset"));
        // RET_V_UNLESS(CheckAssetClass(asset));
        callback(asset);
        il2cpp_utils::RunMethod(assetBundleRequest, "Finalize");
        getLogger().info("AssetBundle::AssetComplete finished!");
    }

    Asset* AssetBundle::LoadAsset(std::string_view assetName, Il2CppReflectionType* assetType) {
        if (!assetType) assetType = CRASH_UNLESS(il2cpp_utils::GetSystemType("UnityEngine", "GameObject"));
        auto* nameStr = CRASH_UNLESS(il2cpp_utils::createcsstr(assetName));

        getLogger().info("Loading asset '%s' synchronously.", assetName.data());
        return RET_0_UNLESS(il2cpp_utils::RunMethod<Asset*>(this, "LoadAsset", nameStr, assetType));
    }

    bool AssetBundle::LoadAssetAsync(std::string_view assetName, AssetCallback callback, Il2CppReflectionType* assetType) {
        if (!assetType) assetType = CRASH_UNLESS(il2cpp_utils::GetSystemType("UnityEngine", "GameObject"));
        auto* nameStr = CRASH_UNLESS(il2cpp_utils::createcsstr(assetName));
        getLogger().info("Loading asset '%s' asynchronously.", assetName.data());

        auto* assetAsync = RET_0_UNLESS(il2cpp_utils::RunMethod(this, "LoadAssetAsync", nameStr, assetType));

        auto* method = RET_0_UNLESS(il2cpp_utils::FindMethodUnsafe(assetAsync, "add_completed", 1));
        auto* action = RET_0_UNLESS(il2cpp_utils::MakeAction(method, 0, (void*)callback, AssetComplete));

        RET_0_UNLESS(il2cpp_utils::RunMethod(assetAsync, method, action));
        getLogger().info("Began loading asset async");
        return true;
    }

    void AssetBundle::AssetBundleComplete(AssetBundleCallback callback, Il2CppObject* assetBundleCreateRequest) {
        getLogger().info("UnityAssetLoader: AssetBundleCreateRequestComplete Called!");
        auto* assetBundle = RET_V_UNLESS(il2cpp_utils::GetPropertyValue<AssetBundle*>(assetBundleCreateRequest, "assetBundle"));
        callback(assetBundle);
        il2cpp_utils::RunMethod(assetBundleCreateRequest, "Finalize");
        getLogger().info("UnityAssetLoader: AssetBundleCreateRequestComplete Finished!");
    }

    Il2CppString* CreateIl2CppStringPathAndEnsureExists(std::string_view filePath) {
        auto* assetFilePath = RET_0_UNLESS(il2cpp_utils::createcsstr(filePath));
        bool pathExists = RET_0_UNLESS(il2cpp_utils::RunMethod<bool>("System.IO", "File", "Exists", assetFilePath));
        RET_0_UNLESS(pathExists);
        return assetFilePath;
    }

    AssetBundle* AssetBundle::LoadFromFile(std::string_view filePath) {
        auto* assetFilePath = RET_0_UNLESS(CreateIl2CppStringPathAndEnsureExists(filePath));
        return RET_0_UNLESS(il2cpp_utils::RunMethod<AssetBundle*>("UnityEngine", "AssetBundle", "LoadFromFile", assetFilePath));
    }

    bool AssetBundle::LoadFromFileAsync(std::string_view filePath, AssetBundleCallback callback) {
        auto* assetFilePath = RET_0_UNLESS(CreateIl2CppStringPathAndEnsureExists(filePath));

        auto* bundleAsync = RET_0_UNLESS(il2cpp_utils::RunMethod("UnityEngine", "AssetBundle", "LoadFromFileAsync", assetFilePath));
        RET_0_UNLESS(il2cpp_utils::SetPropertyValue(bundleAsync, "allowSceneActivation", true));

        auto method = RET_0_UNLESS(il2cpp_utils::FindMethodUnsafe(bundleAsync, "add_completed", 1));
        auto action = RET_0_UNLESS(il2cpp_utils::MakeAction(method, 0, (void*)callback, AssetBundleComplete));

        RET_0_UNLESS(il2cpp_utils::RunMethod(bundleAsync, method, action));
        getLogger().info("Began loading asset bundle '%s' async.", filePath.data());
        return true;
    }
}