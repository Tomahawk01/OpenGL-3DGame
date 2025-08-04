#include "TextFactory.h"

#include "Utilities/Logger.h"
#include "Utilities/Error.h"
#include "Utilities/TextWiden.h"
#include "Texture.h"
#include "Sampler.h"

#include <memory>
#include <span>
#include <Windows.h>
#include <wincodec.h>
#include <dwrite.h>
#include <d2d1helper.h>

namespace Game {

	Texture TextFactory::Create(std::string_view text, const Sampler* sampler) const
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

		const auto textWide = TextWiden(text);

		IDWriteTextLayout* textLayout{};
		Ensure(
			writeFactory->CreateTextLayout(
				textWide.c_str(),
				static_cast<UINT32>(textWide.length()),
				textFormat,
				99999.0f,
				99999.0f,
				&textLayout) == S_OK, "Failed to create a text layout");

		auto metrics = DWRITE_TEXT_METRICS{};
		textLayout->GetMetrics(&metrics);

		const auto width = static_cast<uint32_t>(metrics.width);
		const auto height = static_cast<uint32_t>(metrics.height);

		IWICImagingFactory* imageFactory{};
		Ensure(
			CoCreateInstance(
				CLSID_WICImagingFactory2,
				nullptr,
				CLSCTX_INPROC_SERVER,
				__uuidof(IWICImagingFactory2),
				reinterpret_cast<void**>(&imageFactory)) == S_OK, "Failed to create image factory");

		IWICBitmap* bitmap{};
		Ensure(
			imageFactory->CreateBitmap(
				width,
				height,
				GUID_WICPixelFormat32bppRGBA,
				WICBitmapCacheOnDemand,
				&bitmap) == S_OK, "Failed to create bitmap");

		const auto properties = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_STRAIGHT),
			0.0f,
			0.0f,
			D2D1_RENDER_TARGET_USAGE_NONE);

		ID2D1Factory* direct2dFactory{};
		Ensure(
			D2D1CreateFactory(
				D2D1_FACTORY_TYPE_SINGLE_THREADED,
				&direct2dFactory) == S_OK, "Failed to create d2d factory");

		const auto release = [](auto* obj) { obj->Release(); };
		auto renderTarget = std::unique_ptr<ID2D1RenderTarget, decltype(release)>{};

		Ensure(
			direct2dFactory->CreateWicBitmapRenderTarget(
				bitmap,
				&properties,
				std::out_ptr(renderTarget)) == S_OK, "Failed to create render target");

		auto brush = std::unique_ptr<ID2D1SolidColorBrush, decltype(release)>{};
		Ensure(
			renderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
				std::out_ptr(brush)) == S_OK, "Failed to create brush");

		const auto origin = D2D1::Point2F(0.0f, 0.0f);

		renderTarget->BeginDraw();
		renderTarget->Clear();
		renderTarget->DrawTextLayout(origin, textLayout, brush.get());
		renderTarget->EndDraw();

		UINT32 bitmapWidth{};
		UINT32 bitmapHeight{};
		Ensure(bitmap->GetSize(&bitmapWidth, &bitmapHeight) == S_OK, "Failed to get bitmap size");

		const WICRect rect{
			.X = 0,
			.Y = 0,
			.Width = static_cast<INT>(bitmapWidth),
			.Height = static_cast<INT>(bitmapHeight)
		};

		auto lock = std::unique_ptr<IWICBitmapLock, decltype(release)>{};
		Ensure(bitmap->Lock(&rect, WICBitmapLockRead, std::out_ptr(lock)) == S_OK, "Failed to lock bitmap");

		UINT bufferSize{};
		std::byte* buffer{};

		Ensure(
			lock->GetDataPointer(
				&bufferSize,
				reinterpret_cast<BYTE**>(&buffer)) == S_OK, "Failed to get data pointer");

		auto pixelData = std::span(buffer, buffer + bufferSize);

		return {
			TextureUsage::SRGB,
			pixelData,
			bitmapWidth,
			bitmapHeight,
			sampler
		};
	}

}
