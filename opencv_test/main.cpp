#include "main.h"

direct2d_overlay* graphics;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		auto n_rect = set_overlay_pos();
		graphics->initialize(hwnd);
		graphics->resize_render_target(static_cast<float>(n_rect.right), static_cast<float>(n_rect.bottom));
		std::thread task(
			[&]
			{
				//bad solution
				while (true)
				{
					auto n_rect = set_overlay_pos();
					graphics->resize_render_target(static_cast<float>(n_rect.right), static_cast<float>(n_rect.bottom));
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			});
		if (task.joinable())
			task.detach();		
	}
	break;
	case WM_DESTROY:	
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmd, int nCmdShow) {

	WNDCLASSEX wcx;
	ZeroMemory(&wcx, sizeof(WNDCLASSEX));

	wcx.cbSize = sizeof(WNDCLASSEX);
	wcx.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	wcx.hInstance = hInstance;
	wcx.lpfnWndProc = WindowProc;
	wcx.lpszClassName = "MainWindow";
	wcx.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClassEx(&wcx);

	graphics = new direct2d_overlay();

	RECT rect = { 0, 0, 800, 600 };
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);

	HWND windowHandle = CreateWindowEx(WS_EX_LAYERED, "MainWindow", "Teste", WS_EX_TOPMOST | WS_POPUP, 100, 100, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);
	if (!windowHandle) return -1;
	g_overlay_hwnd = windowHandle;	

	const MARGINS margin = { -1 };
	SetLayeredWindowAttributes(windowHandle, RGB(0, 0, 0), 255, ULW_COLORKEY | LWA_ALPHA);
	DwmExtendFrameIntoClientArea(windowHandle, &margin);

	ShowWindow(windowHandle, nCmdShow);

	MSG messages = { 0 };
	cv::Mat image_to_process;
	auto image_to_find = cv::imread(std::string("C:\\Users\\schin\\Desktop\\opencv_test_real_time\\sample_to_find_.bmp"));
	if (image_to_find.empty()) return -1;

	while (messages.message != WM_QUIT) 
	{
		if (PeekMessage(&messages, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&messages);
			DispatchMessage(&messages);
		}
		else 
		{
			float x = 0, y = 0, w = 0, h = 0;
			cv::Mat target_window_matrix = screen_shot::get_image("health_bar.bmp (266×68) - Google Chrome");
			if (!target_window_matrix.empty())
			{
				try
				{				
					find_health_bar(target_window_matrix, image_to_find, x, y, w, h);
				}
				catch (const std::exception& e)
				{
					OutputDebugString(e.what());
					w = 0.f;
					h = 0.f;
				}
			}

			if (w > 0 && h > 0)
			{				
				graphics->render_match(x, y, w, h);
			}			
		}
	}

	delete graphics;

	return 0;
}

int find_health_bar(cv::Mat& image_to_process, cv::Mat& image_to_find, float& x, float& y, float& w, float& h)
{
	int match_method = cv::TemplateMatchModes::TM_CCORR_NORMED;
	int cols = image_to_process.cols - image_to_find.cols + 1;
	int rows = image_to_process.rows - image_to_find.rows + 1;
	cv::Mat result;
	result.create(cols, rows, CV_32FC1);
	cv::matchTemplate(image_to_process, image_to_find, result, match_method);

	bool show_image = false;
	cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

	/// Localizing the best match with minMaxLoc
	double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
	cv::Point matchLoc;

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
	
	if (match_method == cv::TemplateMatchModes::TM_SQDIFF || match_method == cv::TemplateMatchModes::TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		matchLoc = maxLoc;
	}

	if (maxVal > 0)
	{
		x = (float)maxLoc.x;
		y = (float)maxLoc.y;
		w = (float)image_to_find.cols;
		h = (float)image_to_find.rows;	
	}

	return 0;
}