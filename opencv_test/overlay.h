#pragma once

#include <Windows.h>
#include <d2d1.h>
#include <dwmapi.h>

#pragma comment(lib, "D2d1.lib")
#pragma comment(lib, "Dwmapi.lib")

class direct2d_overlay 
{
private:
	ID2D1Factory* p_factory;
	ID2D1HwndRenderTarget* p_render_target;
	ID2D1SolidColorBrush* p_solid_brush;

public:
	direct2d_overlay();
	~direct2d_overlay();

	bool initialize(HWND window_hwnd);

	void begin_draw() 
	{ 
		p_render_target->BeginDraw(); 
	}
	void end_draw() 
	{ 
		p_render_target->EndDraw(); 
	}

	void render();
	void render_match(float x, float y, float w, float h);
	void resize_render_target(float w, float h);
	void clear_screen(float r, float g, float b);
	void draw_circle(float x, float y, float radius, float r, float g, float b, float a);
};