#pragma once
#include <filesystem>
#include <Windows.h>

inline std::filesystem::path GetPluginAssetRoot()
{
    char buf[MAX_PATH];
    DWORD needed = ::GetEnvironmentVariableA("DX12_PLUGIN_ASSET_ROOT", buf, (DWORD)std::size(buf));
    if (needed > 0 && needed < std::size(buf)) {
        return std::filesystem::path(std::string(buf, buf + needed));
    }


}