#include "overlay.h"

direct2d_overlay::direct2d_overlay()
{
	p_factory = NULL;
	p_render_target = NULL;
	p_solid_brush = NULL;
}

direct2d_overlay::~direct2d_overlay()
{
	if (p_factory) p_factory->Release();
	if (p_render_target) p_render_target->Release();
	if (p_solid_brush) p_solid_brush->Release();
}

bool direct2d_overlay::initialize(HWND window_hwnd)
{
	HRESULT res = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &p_factory);
	if (res != S_OK) return false;

	RECT rect = { 0 };
	GetClientRect(window_hwnd, &rect);

	p_factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)),
		D2D1::HwndRenderTargetProperties(window_hwnd, D2D1::SizeU(rect.right, rect.bottom)),
		&p_render_target);

	if (res != S_OK) return false;

	res = p_render_target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0), &p_solid_brush);
	if (res != S_OK) return false;

	return true;
}

void direct2d_overlay::clear_screen(float r, float g, float b)
{
	p_render_target->Clear(D2D1::ColorF(r, g, b));
}

void direct2d_overlay::draw_circle(float x, float y, float radius, float r, float g, float b, float a)
{
	p_solid_brush->SetColor(D2D1::ColorF(r, g, b, a));
	p_render_target->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), p_solid_brush, 3.0f);
}

void direct2d_overlay::render()
{
	begin_draw();
	clear_screen(0.0f, 0.0f, 0.5f);
	draw_circle(100, 100, 50, 1.0f, 0.0f, 0.0f, 1.0f);
	end_draw();
}

void direct2d_overlay::render_match(float x, float y, float w, float h)
{
	begin_draw();
	p_render_target->Clear();

	const D2D1_SIZE_F rtSize = p_render_target->GetSize();
	p_solid_brush->SetColor(D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f));
	auto rect = D2D1::RectF(x, y, rtSize.width / 2 + w / 2, rtSize.height / 2 + h);

	p_render_target->DrawRectangle(rect, p_solid_brush, 2.f);

	p_render_target->SetTransform(D2D1::Matrix3x2F::Identity());
	p_solid_brush->SetColor(D2D1::ColorF(0.0f, 0.0f, 1.0f, 1.0f));
	rect = D2D1::RectF(0, 0, rtSize.width, rtSize.height);
	p_render_target->DrawRectangle(rect, p_solid_brush);

	end_draw();
}

void direct2d_overlay::resize_render_target(float w, float h)
{
	D2D1_SIZE_U size = D2D1::SizeU((UINT32)w, (UINT32)h);
	p_render_target->Resize(size);
}
