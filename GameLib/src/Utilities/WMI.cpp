#include "WMI.h"

#include "Error.h"
#include "TextWiden.h"

namespace {

	struct AutoVariant
	{
		~AutoVariant()
		{
			::VariantClear(&var);
		}

		::VARIANT& var;
	};

}

namespace Game {

	WMI::WMI()
		: m_Locator{}
		, m_Services{}
	{
		Ensure(::CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC, IID_IWbemLocator, std::out_ptr(m_Locator)) == S_OK, "Failed to create locator");

		Ensure(m_Locator->ConnectServer(_bstr_t(L"root\\CIMV2"), nullptr, nullptr, 0, NULL, 0, 0, std::out_ptr(m_Services)) == S_OK, "Failed to create services");

		Ensure(::CoSetProxyBlanket(m_Services.get(), RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE) == S_OK, "Failed to set blanket");
	}

	std::vector<std::string> WMI::Query(const std::string& wql, const std::string& propertyName) const
	{
		std::unique_ptr<::IEnumWbemClassObject, ComRelease> enumerator{};
		Ensure(m_Services->ExecQuery(bstr_t("WQL"), bstr_t(wql.c_str()), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, std::out_ptr(enumerator)) == S_OK, "Failed to execute query: {}", wql);

		std::unique_ptr<::IWbemClassObject, ComRelease> obj{};
		::ULONG ret{};

		const auto propertyNameWide = TextWiden(propertyName);
		std::vector<std::string> values{};

		while (enumerator)
		{
			if (enumerator->Next(WBEM_INFINITE, 1, std::out_ptr(obj), &ret) != S_OK)
			{
				break;
			}

			{
				::VARIANT prop{};
				AutoVariant autoVar{ prop };

				if (obj->Get(propertyNameWide.c_str(), 0, &prop, 0, 0) == S_OK)
				{
					const std::wstring wstr(prop.bstrVal, ::SysStringLen(prop.bstrVal));
					values.push_back(TextNarrow(wstr));
				}
			}
		}

		return values;
	}

}
