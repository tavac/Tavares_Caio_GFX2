#pragma once
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
//#include <Windows.h>
#include <Windowsx.h>
#include <wrl/client.h>
#include <string>
#include <stdio.h>
#include <iostream>

#pragma region FROM GRAPHICS_H
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>
#include <fbxsdk.h>
#include <vector>
#include "DDSTextureLoader.h"
#pragma endregion

#pragma region FROM WINDOWS_CPP
#include <sstream> // for ostringstream
#include <time.h>
#pragma endregion

static UINT hWndWidth = 1280;
static UINT hWndHeight = 720;
static const double hWndAspectRatio = (hWndHeight / hWndWidth);
#define degToRad(deg) (deg*(3.1415f/180.0f))

class ToolBox
{
public: /*Triforce*/
	ToolBox() = default;
	ToolBox(const ToolBox&) = delete;
	ToolBox& operator=(const ToolBox&) = delete;
	~ToolBox() = default;

public:
	static void CleanUp(HWND* _hWnd);
	static void ThrowErrorMsg(const char* ErrMsg);
	static std::string CommandProcesser(std::string command);

};

class Timer
{
public:
	float timeStart = 0;
	float deltaTime = 0;
	float timeCur = 0;

	void StartTimer();
	float TimeSinceStart();
	float TimeSinceTick();
};