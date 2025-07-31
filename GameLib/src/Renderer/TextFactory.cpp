#include "TextFactory.h"

#include "Utilities/Logger.h"
#include "Utilities/Error.h"
#include "Texture.h"

#include <Windows.h>
#include <dwrite.h>

namespace Game {

	Texture* TextFactory::Create(std::string_view text) const
	{
		Logger::Trace("Creating text for: {}", text);

		IDWriteFactory* writeFactory{};
		Ensure(
			DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(IDWriteFactory),
				reinterpret_cast<IUnknown**>(&writeFactory)) == S_OK, "Failed to create write factory");

		IDWriteTextFormat* textFormat{};
		Ensure(
			writeFactory->CreateTextFormat(
				L"Arial",
				nullptr,
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				10.0f * (96.0f / 72.0f),
				L"en-CA",
				&textFormat) == S_OK, "Failed to create text format");

		return {};
	}

}
