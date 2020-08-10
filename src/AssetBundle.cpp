#include "AssetBundle.hpp"

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

    Asset* AssetBundle::LoadAsset(std::string_view assetName, Il2CppReflectionType* assetType) {
        if (!assetType) assetType = CRASH_UNLESS(il2cpp_utils::GetSystemType("UnityEngine", "GameObject"));
        auto* nameStr = CRASH_UNLESS(il2cpp_utils::createcsstr(assetName));

        getLogger().info("Loading asset '%s' synchronously.", assetName.data());
        return RET_0_UNLESS(il2cpp_utils::RunMethod<Asset*>(this, "LoadAsset", nameStr, assetType));
    }

    Il2CppString* AssetBundle::CreateIl2CppStringPathAndEnsureExists(std::string_view filePath) {
        auto* assetFilePath = RET_0_UNLESS(il2cpp_utils::createcsstr(filePath));
        bool pathExists = RET_0_UNLESS(il2cpp_utils::RunMethod<bool>("System.IO", "File", "Exists", assetFilePath));
        RET_0_UNLESS(pathExists);
        return assetFilePath;
    }

    AssetBundle* AssetBundle::LoadFromFile(std::string_view filePath) {
        auto* assetFilePath = RET_0_UNLESS(CreateIl2CppStringPathAndEnsureExists(filePath));
        return RET_0_UNLESS(il2cpp_utils::RunMethod<AssetBundle*>("UnityEngine", "AssetBundle", "LoadFromFile", assetFilePath));
    }
}