#pragma once

#include <Windows.h>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

namespace screen_shot
{
	static cv::Mat get_image(std::string window_name)
	{
		cv::Mat desktop_matrix;
		RECT rect = { 0 };

		HWND target_window_hwnd = FindWindow(NULL, window_name.c_str());
		if (target_window_hwnd == NULL) return desktop_matrix;

		//get target window size
		GetClientRect(target_window_hwnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		auto hdc = GetDC(target_window_hwnd);
		auto hbitmap = CreateCompatibleBitmap(hdc, width, height);
		auto memdc = CreateCompatibleDC(hdc);
		auto oldbmp = SelectObject(memdc, hbitmap);

		BitBlt(memdc, 0, 0, width, height, hdc, 0, 0, SRCCOPY);

		desktop_matrix = cv::Mat(height, width, CV_8UC4);
		BITMAPINFOHEADER bi = { sizeof(BITMAPINFOHEADER), width, -height, 1, 32, BI_RGB, 0};
		GetDIBits(hdc, hbitmap, 0, height, desktop_matrix.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);	

		//remove alpha channel
		cv::Mat dst;
		cv::cvtColor(desktop_matrix, dst, cv::COLOR_RGBA2BGR);

		//cv::imwrite("screenshot.bmp", desktop_matrix);
		//cv::imwrite("dst.bmp", dst);

		SelectObject(memdc, oldbmp);
		DeleteDC(memdc);
		DeleteObject(hbitmap);
		ReleaseDC(target_window_hwnd, hdc);

		return dst;
	}	
}