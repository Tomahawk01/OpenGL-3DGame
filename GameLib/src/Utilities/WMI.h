#pragma once

#include "ComRelease.h"

#include <string>
#include <vector>
#include <memory>

#include <Windows.h>
#include <WbemIdl.h>
#include <comdef.h>

namespace Game {

	class WMI
	{
	public:
		WMI();

		std::vector<std::string> Query(const std::string& wql, const std::string& propertyName) const;

	private:
		std::unique_ptr<::IWbemLocator, ComRelease> m_Locator;
		std::unique_ptr<::IWbemServices, ComRelease> m_Services;
	};

}
