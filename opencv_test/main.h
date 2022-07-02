#pragma once

#include <iostream>
#include <atomic>
#include <string>

#include "screenshot.h"
#include "overlay.h"

#include <opencv2/core/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

const std::wstring health_bar_bmp = L"C:\\Users\\jfilho\\source\\repos\\opencv_test\\health_bar.bmp";
static HWND g_overlay_hwnd = nullptr;

int find_health_bar(cv::Mat& image_to_process, cv::Mat& image_to_find, float& x, float& y, float& w, float& h);

static RECT set_overlay_pos()
{	
	HWND find_window = FindWindow(NULL, "health_bar.bmp (266×68) - Google Chrome");
	
	RECT relative_rect = {0};
	RECT client_rect = {0};
	GetClientRect(find_window, &relative_rect);
	GetClientRect(find_window, &client_rect);
	MapWindowPoints(find_window, GetDesktopWindow(), (LPPOINT)&relative_rect, 2);
	SetWindowPos(g_overlay_hwnd, HWND_TOPMOST, relative_rect.left, relative_rect.top, client_rect.right, client_rect.bottom, SWP_ASYNCWINDOWPOS | SWP_SHOWWINDOW);
	RECT n_rect;
	n_rect.top = 0;
	n_rect.left = 0;
	n_rect.right = client_rect.right; //w
	n_rect.bottom = client_rect.bottom; //h
	return n_rect;
}