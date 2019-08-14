#pragma once
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
//#include <Windows.h>
#include <Windowsx.h>
#include <wrl/client.h>
#include <string>
#include <stdio.h>
#include <iostream>

#define	hWndWidth 1280
#define hWndHeight 720
#define degToRad(deg) (deg*(3.1415/180))

class ToolBox
{
public:
	static void CleanUp(HWND* _hWnd);
	static void ThrowErrorMsg(const char* ErrMsg);
	static std::string CommandProcesser(std::string command);
	//static float StartTimer();
	//static float TimerTick(std::chrono::steady_clock::time_point tp);
	//static float TimerDelta();
};