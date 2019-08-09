#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl/client.h>
#include <string>

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