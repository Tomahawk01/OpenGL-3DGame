#include "SystemInfo.h"

#include "Renderer/OpenGL.h"
#include "TextWiden.h"
#include "Exception.h"

#include <format>

#include <Windows.h>
#include <WbemIdl.h>
#include <comdef.h>

namespace {

	std::string GetOSVersion()
	{
		::OSVERSIONINFOEXA versionInfo{};
		versionInfo.dwOSVersionInfoSize = sizeof(::OSVERSIONINFOEXA);

#pragma warning(disable: 4996)
		if (::GetVersionExA(reinterpret_cast<::OSVERSIONINFOA*>(&versionInfo)) != 0)
		{
			return std::format("Major: {} Minor: {} Build: {} Platform ID: {} Service pack major: {}, Service pack minor: {} Product type: {}",
							   versionInfo.dwMajorVersion,
							   versionInfo.dwMinorVersion,
							   versionInfo.dwBuildNumber,
							   versionInfo.dwPlatformId,
							   versionInfo.wServicePackMajor,
							   versionInfo.wServicePackMinor,
							   versionInfo.wProductType);
		}
#pragma warning(default: 4996)

		return {};
	}

	std::string GetGPUID()
	{
		do
		{
			::IWbemLocator* locator{};
			if (::CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC, IID_IWbemLocator, reinterpret_cast<void**>(&locator)) != S_OK)
			{
				break;
			}

			::IWbemServices* services{};
			if (locator->ConnectServer(_bstr_t(L"root\\CIMV2"), nullptr, nullptr, 0, NULL, 0, 0, &services) != S_OK)
			{
				break;
			}

			if (::CoSetProxyBlanket(services, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE) != S_OK)
			{
				break;
			}

			::IEnumWbemClassObject* enumerator{};
			if (services->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_VideoController"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &enumerator) != S_OK)
			{
				break;
			}

			::IWbemClassObject* obj{};
			::ULONG ret{};
			std::string id{};
			while (enumerator)
			{
				if (enumerator->Next(WBEM_INFINITE, 1, &obj, &ret) != S_OK)
				{
					break;
				}

				::VARIANT prop{};
				if (obj->Get(L"Caption", 0, &prop, 0, 0) == S_OK)
				{
					const std::wstring wstr(prop.bstrVal, ::SysStringLen(prop.bstrVal));
					try
					{
						id += Game::TextNarrow(wstr) + " ";
					}
					catch (Game::Exception&)
					{
						return {};
					}
				}

				::VariantClear(&prop);
			}

			return id;

		} while (false);

		return {};
	}

}

namespace Game {

	SystemInfo GetSystemInfo()
	{
		return {
			.osVersion = GetOSVersion(),
			.gpuID = GetGPUID(),
			.gpuDriver = {},
			.systemMemory = {},
			.openglInfo = {}
		};
	}

	std::string to_string(const SystemInfo& info)
	{
		const auto getOrEmpty =
			[](const auto& s)
			{
				return s.empty() ? "unknown" : s;
			};

		return std::format("\nOS version: {}\nGPU id: {}\nGPU driver: {}\nSystem memory: {}\nOpenGL info: {}",
						   getOrEmpty(info.osVersion),
						   getOrEmpty(info.gpuID),
						   getOrEmpty(info.gpuDriver),
						   getOrEmpty(info.systemMemory),
						   getOrEmpty(info.openglInfo));
	}

}
