//#include <Windows.h>
#include "Graphics.h"
#include <string>
#include <sstream> // for ostringstream
#include <time.h>

#pragma region Variables
WNDCLASSEX WndCls;
HWND hWnd;
std::string winTitle = "Hit 'TAB' to enter command, Execute with 'ENTER'";
Graphics* Gfx = nullptr;
std::string strIB;
bool isTyping = false;
#pragma endregion

#pragma region ForwardDeclarations
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDCLASSEX Init_WindowClass(const char* _className, HINSTANCE _hInst);
HWND Init_Window(int _width, int _height, std::string _title, WNDCLASSEX* _WndClass);
bool DirLight_ComProc(std::string s);
#pragma endregion

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{

	WNDCLASSEX wc = Init_WindowClass("WndClassEX", hInstance);
	hWnd = Init_Window(1280, 720, winTitle, &wc);

	Gfx = new Graphics(hWnd);
	Gfx->InitDevice();

	// Message Loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Gfx->Render();
		}
	}

	// CLEAN UP
	if (Gfx != nullptr)
	{
		delete Gfx;
		Gfx = nullptr;
	}
	ToolBox::CleanUp(&hWnd);

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0); // is the arbitrary value we want to return on quit through wParam.
		break;
	//case VK_RETURN:
	//{
	//
	//}
	//break;
	case WM_CHAR: // TranslateMessage(); processes WM_CHAR allow for easy key input.
	{
		if (wParam == VK_TAB || isTyping)
		{
			if (!isTyping)
			{
				isTyping = true;
				break;
			}
			// While Typing...
			// "Enter" to send strIB to string processor
			if (wParam == VK_RETURN)
			{
				if ((strIB = ToolBox::CommandProcesser(strIB)) != "")
				{
					if (strIB == "cls")
					{
						PostQuitMessage(0);
						isTyping = false;
					}
					//else if (strIB.substr(0,6) == "alight")
					else if (strIB.substr(0, 6) == "dlight")
					{
						DirLight_ComProc(strIB);
						isTyping = false;
					}
				}
				strIB = "";
				SetWindowText(hWnd, winTitle.c_str());
				break;
			}
			strIB.push_back((char)wParam);
			SetWindowText(hWnd, strIB.c_str());
		}

	}
	break;
	case WM_LBUTTONDOWN: // Left Click to get coordinate of raster where (0,0) is top left.
	{
		POINTS pt = MAKEPOINTS(lParam);
		std::ostringstream oss;
		oss << "Cursor Position: (" << pt.x << "," << pt.y << ")" << std::endl;
		OutputDebugString(oss.str().c_str());
		oss.clear();
	}
	break;
	case WM_MOUSEWHEEL:
	{
		Gfx->CameraMoveInOut(GET_WHEEL_DELTA_WPARAM(wParam) * 0.0001f);
	}
	break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

WNDCLASSEX Init_WindowClass(const char* _className, HINSTANCE _hInst)
{
	// Register window class.
	WndCls = { 0 };
	WndCls.cbSize = sizeof(WndCls);
	WndCls.style = CS_OWNDC;
	WndCls.lpfnWndProc = WndProc;
	WndCls.cbClsExtra = 0;
	WndCls.cbWndExtra = 0;
	WndCls.hInstance = _hInst;
	WndCls.hIcon = nullptr;
	WndCls.hCursor = nullptr;
	WndCls.hbrBackground = nullptr;
	WndCls.lpszMenuName = nullptr;
	WndCls.lpszClassName = _className;

	if (RegisterClassEx(&WndCls) == 0)
		ToolBox::ThrowErrorMsg("CTERROR: In_WindowClass(const char*, HINSTANCE)\nReturned 0\nWindow Class Failed To Register");

	return WndCls;
}

HWND Init_Window(int _width, int _height, std::string _title, WNDCLASSEX* _WndClass)
{

	// Create window.
	RECT ClientRect;
	ClientRect.left = 100;
	ClientRect.right = ClientRect.left + _width;
	ClientRect.top = 100;
	ClientRect.bottom = ClientRect.top + _height;
	AdjustWindowRect(&ClientRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);
	HWND hWnd = CreateWindowEx(
		0, _WndClass->lpszClassName, _title.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200, 200, ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top,
		nullptr, nullptr, _WndClass->hInstance, nullptr);

	if (hWnd == nullptr)
		ToolBox::ThrowErrorMsg("CTERROR: BuildWindow(int,int,string,wndclassex)\nhWnd was null");

	// Draw hWnd.
	ShowWindow(hWnd, SW_SHOW);
	return hWnd;
}

bool DirLight_ComProc(std::string s)
{
	bool rtn;
	if (strIB == "dlight red")
	{
		Gfx->LightState = Graphics::DirectionLight_Red;
		rtn = true;
	}
	else if (strIB == "dlight green")
	{
		Gfx->LightState = Graphics::DirectionLight_Green;
		rtn = true;
	}
	else if (strIB == "dlight blue")
	{
		Gfx->LightState = Graphics::DirectionLight_Blue;
		rtn = true;
	}
	else if (strIB == "dlight reset")
	{
		Gfx->LightState = Graphics::DirectionLight_Default;
		rtn = true;
	}
	else
	{
		strIB = "";
		OutputDebugString("Invalid Command");
		rtn = false;
	}
	return rtn;
}