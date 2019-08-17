//#include <Windows.h>
#include "Graphics.h"
#include <sstream> // for ostringstream
#include <time.h>

#pragma region Variables
WNDCLASSEX WndCls;
HWND hWnd;
std::string winTitle = "Hit 'TAB' to enter command, Execute with 'ENTER'";
Graphics* Gfx = nullptr;
std::string strIB;
bool isTyping = false;
#define MODEL_COUNT 3
int currModel = 0;
bool ModelSwitched = FALSE;
#define MoveSpeed 1.0f
XMFLOAT2 centerScreen;
float last_X;
float last_Y;
float screenY;
float screenX;
float screenRatio;
#pragma endregion

#pragma region ForwardDeclarations
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDCLASSEX Init_WindowClass(const char* _className, HINSTANCE _hInst);
HWND Init_Window(int _width, int _height, std::string _title, WNDCLASSEX* _WndClass);
void ModelDraw_Switch(int nextModelToLoad);
#pragma endregion

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	screenY = static_cast<float>(GetSystemMetrics(SM_CYSCREEN));
	screenX = static_cast<float>(GetSystemMetrics(SM_CXSCREEN));
	centerScreen = { (screenX * 0.5f) ,(screenY * 0.5f) };
	last_X = centerScreen.x;
	last_Y = centerScreen.y;
	screenRatio = (hWndHeight) * (hWndWidth) / (screenY * screenX);
	SetCursorPos(static_cast<int>(centerScreen.x), static_cast<int>(centerScreen.y));

	WNDCLASSEX wc = Init_WindowClass("WndClassEX", hInstance);
	hWnd = Init_Window(hWndWidth, hWndHeight, winTitle, &wc);
	Gfx = new Graphics(hWnd);
#pragma region LOAD MODELS
	//ModelDraw_Switch(currModel); // Default Model set to draw, SPACE BAR to cycle.
	//std::string modelName[MODEL_COUNT] = { "Tester.fbx","NewDragon.fbx","Cube.fbx" }; // convert to array or vector of strings to store multiple mesh directories.
	//Gfx->LoadMesh("Tester.fbx", 1.0f, Gfx->gppMesh, 0);
	//Gfx->LoadMesh("NewDragon.fbx",10.0f, Gfx->gppMesh, 0);
	//Gfx->LoadMesh("SpaceShip_1.fbx", 1.0f, Gfx->gppMesh, 0);
	//Gfx->LoadMesh("SpaceShip_3.fbx", 0.5f, &Gfx->gppMesh, 0);
	Gfx->LoadMesh("Desk_0.fbx", 0.5f, &Gfx->gppMesh, 0);
	//Gfx->LoadMesh("Cube.fbx", 50.0f, Gfx->gppMesh, 0);
	//Gfx->LoadMesh("Cube.fbx", 50.0f, Gfx->gppMesh, 1);
#pragma endregion
	Gfx->InitDevice();

	// Message Loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		//if (ModelSwitched)
		//{
		//	ModelSwitched = FALSE;
		//	ModelDraw_Switch(currModel);
		//	Gfx->InitDevice();
		//}


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
		//case WM_CHAR: // TranslateMessage(); processes WM_CHAR allow for easy key input.
		//{
		//	if (wParam == VK_TAB || isTyping)
		//	{
		//		if (!isTyping)
		//		{
		//			isTyping = true;
		//			break;
		//		}
		//		// While Typing...
		//		// "Enter" to send strIB to string processor
		//		if (wParam == VK_RETURN)
		//		{
		//			if ((strIB = ToolBox::CommandProcesser(strIB)) != "")
		//			{
		//				if (strIB == "cls")
		//				{
		//					PostQuitMessage(0);
		//					isTyping = false;
		//				}
		//				//else if (strIB.substr(0,6) == "alight")
		//				else if (strIB.substr(0, 6) == "dlight")
		//				{
		//					DirLight_ComProc(strIB);
		//					isTyping = false;
		//				}
		//			}
		//			strIB = "";
		//			SetWindowText(hWnd, winTitle.c_str());
		//			break;
		//		}
		//		strIB.push_back((char)wParam);
		//		SetWindowText(hWnd, strIB.c_str());
		//	}
		//
		//}
		//break;
	case WM_LBUTTONDOWN: // Left Click to get coordinate of raster where (0,0) is top left.
	{
		POINTS pt = MAKEPOINTS(lParam);
		std::ostringstream oss;
		oss << "Cursor Position: (" << pt.x << "," << pt.y << ")" << std::endl;
		OutputDebugString(oss.str().c_str());
		oss.clear();
	}
	break;
	// Camera Controls
	case WM_MOUSEWHEEL:
	{
		//if (/*direction > 0 && */)
		//	//if ((char)wParam == 'f') // Fov Wider // zoom out
		//{
		//	Gfx->FoV_angle += direction;
		//	Gfx->globalProj = XMMatrixPerspectiveFovLH(degToRad(Gfx->FoV_angle), 1280.0f / 720.0f, Gfx->nearPlane, Gfx->farPlane);
		//}
		//else if ((char)wParam == 'r') // Fov narrower // zoom in
		if (Gfx->FoV_angle <= 120 && Gfx->FoV_angle >= 30)
		{
			float direction = GET_WHEEL_DELTA_WPARAM(wParam) * 0.01f;
			Gfx->FoV_angle += direction;
			Gfx->globalProj = XMMatrixPerspectiveFovLH(degToRad(Gfx->FoV_angle), 1280.0f / 720.0f, Gfx->nearPlane, Gfx->farPlane);
		}
		else
		{
			if (Gfx->FoV_angle > 120) Gfx->FoV_angle -= 0.01f;
			else if (Gfx->FoV_angle < 30) Gfx->FoV_angle += 0.01f;
		}
	}
	break;
	case WM_KEYDOWN:
	{
		if (wParam == VK_UP)
		{
			//float xPos = GET_X_LPARAM(lParam);
			//float yPos = GET_Y_LPARAM(lParam);
			//if (xPos > 0 && xPos < hWndWidth && yPos > 0 && yPos < hWndHeight)
			//{
			//if (xPos != last_X)
			//{
				//float Yangle = xPos - last_X;
				//XMMATRIX rotationY = XMMatrixRotationY(degToRad(Yangle * screenRatio));
				//XMVECTOR saver = Gfx->Camera.r[3];
				//Gfx->Camera.r[3] = XMVectorSet(0, 0, 0, 1);
				//Gfx->Camera = XMMatrixMultiply(Gfx->Camera, rotationY);
				//Gfx->Camera.r[3] = saver;
			//}
			//if (yPos != last_Y)
			//{
				//float Xangle = yPos - last_Y;
				//XMMATRIX rotationX = XMMatrixRotationX(degToRad(Xangle * 0.5f));
			XMVECTOR saver = Gfx->Camera.r[3];
			Gfx->Camera.r[3] = XMVectorSet(0, 0, 0, 1);
			Gfx->Camera = XMMatrixMultiply(XMMatrixRotationX(degToRad(-2.5f)), Gfx->Camera);
			Gfx->Camera.r[3] = saver;
			//}
			//}
			//last_X = xPos;
			//last_Y = yPos;
		}
		else if (wParam == VK_DOWN)
		{
			XMVECTOR saver = Gfx->Camera.r[3];
			Gfx->Camera.r[3] = XMVectorSet(0, 0, 0, 1);
			Gfx->Camera = XMMatrixMultiply(XMMatrixRotationX(degToRad(2.5f)), Gfx->Camera);
			Gfx->Camera.r[3] = saver;
		}
		else if (wParam == VK_LEFT)
		{
			XMVECTOR saver = Gfx->Camera.r[3];
			Gfx->Camera.r[3] = XMVectorSet(0, 0, 0, 1);
			Gfx->Camera = XMMatrixMultiply(Gfx->Camera, XMMatrixRotationY(degToRad(-2.5f)));
			Gfx->Camera.r[3] = saver;
		}
		else if (wParam == VK_RIGHT)
		{
			XMVECTOR saver = Gfx->Camera.r[3];
			Gfx->Camera.r[3] = XMVectorSet(0, 0, 0, 1);
			Gfx->Camera = XMMatrixMultiply(Gfx->Camera, XMMatrixRotationY(degToRad(2.5f)));
			Gfx->Camera.r[3] = saver;
		}

		// Up
		if (GetAsyncKeyState(VK_SPACE))
		{
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			//Gfx->globalView.r[3] += {0.0f, -0.1f, 0.0f};
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			XMMATRIX trans = XMMatrixTranslation(0.0f, MoveSpeed, 0.0f);
			Gfx->Camera = XMMatrixMultiply(Gfx->Camera, trans);
		}
		// Down
		if (GetAsyncKeyState(VK_LSHIFT))
		{
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			//Gfx->globalView.r[3] += {0.0f, 0.1f, 0.0f};
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			XMMATRIX trans = XMMatrixTranslation(0.0f, -MoveSpeed, 0.0f);
			Gfx->Camera = XMMatrixMultiply(Gfx->Camera, trans);
		}
	}
	case WM_CHAR:
	{
		if ((char)wParam == 'p')
		{
			std::ostringstream oss;
			XMFLOAT4 currPos;
			XMStoreFloat4(&currPos, Gfx->Camera.r[3]);
			oss << "Camera Position: (" << currPos.x << "," << currPos.y << "," << currPos.z << ")" << std::endl;
			OutputDebugString(oss.str().c_str());
			oss.clear();
		}
		if ((char)wParam == 'l')
		{
			if (Gfx->PointLight_A < 1.0f)
				Gfx->PointLight_A = 1.0f;
			else
				Gfx->PointLight_A = 0.0f;
		}
		if ((char)wParam == 'o' || (char)wParam == '0') // Go Home Cam youre drunk
		{
			Gfx->globalView = XMMatrixLookAtLH(Gfx->Eye, Gfx->At, Gfx->Up);
			Gfx->Camera = XMMatrixInverse(nullptr, Gfx->globalView);
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		if ((char)wParam == 'w') // Forward
		{
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			//Gfx->globalView.r[2] -= {0.0f, 0.0f, 0.1f};
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			XMMATRIX trans = XMMatrixTranslation(0.0f, 0.0f, MoveSpeed);
			Gfx->Camera = XMMatrixMultiply(trans, Gfx->Camera);
		}
		else if ((char)wParam == 's') // Back
		{
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			//Gfx->globalView.r[2] += {0.0f, 0.0f, 0.1f};
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			XMMATRIX trans = XMMatrixTranslation(0.0f, 0.0f, -MoveSpeed);
			Gfx->Camera = XMMatrixMultiply(trans, Gfx->Camera);
		}
		else if ((char)wParam == 'a') // Left
		{
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			//Gfx->globalView.r[2] += {0.1f, 0.0f, 0.0f};
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			XMMATRIX trans = XMMatrixTranslation(-MoveSpeed, 0.0f, 0.0f);
			Gfx->Camera = XMMatrixMultiply(trans, Gfx->Camera);
		}
		else if ((char)wParam == 'd') // Right
		{
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			//Gfx->globalView.r[2] -= {0.1f, 0.0f, 0.0f};
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			XMMATRIX trans = XMMatrixTranslation(MoveSpeed, 0.0f, 0.0f);
			Gfx->Camera = XMMatrixMultiply(trans, Gfx->Camera);
		}

		//// roll left
		//if ((char)wParam == 'q')
		//{
		//	XMMATRIX rotation = XMMatrixRotationZ(degToRad(2));
		//	XMVECTOR saver = Gfx->Camera.r[3];
		//	Gfx->Camera.r[3] = XMVectorSet(0, 0, 0, 1);
		//	Gfx->Camera = XMMatrixMultiply(rotation, Gfx->Camera);
		//	Gfx->Camera.r[3] = saver;
		//}
		//// roll right
		//else if ((char)wParam == 'e')
		//{
		//	XMMATRIX rotation = XMMatrixRotationZ(degToRad(-2));
		//	XMVECTOR saver = Gfx->Camera.r[3];
		//	Gfx->Camera.r[3] = XMVectorSet(0, 0, 0, 1);
		//	Gfx->Camera = XMMatrixMultiply(rotation, Gfx->Camera);
		//	Gfx->Camera.r[3] = saver;
		//}

		if ((char)wParam == '=') // Far plane closer
		{
			if (Gfx->farPlane > 50.0f)
			{
				Gfx->farPlane -= 50.0f;
				Gfx->globalProj = XMMatrixPerspectiveFovLH(degToRad(Gfx->FoV_angle), 1280.0f / 720.0f, Gfx->nearPlane, Gfx->farPlane);
			}
		}
		else if ((char)wParam == '-') // Far plane further
		{
			if (Gfx->farPlane < 500.0f)
			{
				Gfx->farPlane += 50.0f;
				Gfx->globalProj = XMMatrixPerspectiveFovLH(degToRad(Gfx->FoV_angle), 1280.0f / 720.0f, Gfx->nearPlane, Gfx->farPlane);
			}
		}
		else if ((char)wParam == '[') // Near plane closer
		{
			if (Gfx->nearPlane > 0.001)
			{
				Gfx->nearPlane /= 10.0f;
				Gfx->globalProj = XMMatrixPerspectiveFovLH(degToRad(Gfx->FoV_angle), 1280.0f / 720.0f, Gfx->nearPlane, Gfx->farPlane);
			}
		}
		else if ((char)wParam == ']') // Near plane further
		{
			if (Gfx->nearPlane < 25.0f)
			{
				Gfx->nearPlane *= 10.0f;
				Gfx->globalProj = XMMatrixPerspectiveFovLH(degToRad(Gfx->FoV_angle), 1280.0f / 720.0f, Gfx->nearPlane, Gfx->farPlane);
			}
		}
		if ((char)wParam == ',')
		{
			if (Gfx->SpotLightWidth > 0.001f)
				Gfx->SpotLightWidth -= 0.01f;
			std::ostringstream oss;
			oss << Gfx->SpotLightWidth << std::endl;
			OutputDebugString(oss.str().c_str());
			oss.clear();
		}
		else if ((char)wParam == '.')
		{
			if (Gfx->SpotLightWidth < 1.0f)
				Gfx->SpotLightWidth += 0.01f;
			std::ostringstream oss;
			oss << Gfx->SpotLightWidth << std::endl;
			OutputDebugString(oss.str().c_str());
			oss.clear();
		}
	}
	}
	if (Gfx)
		Gfx->globalView = XMMatrixInverse(nullptr, Gfx->Camera);
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
	ClientRect.left = labs(centerScreen.x - (_width * 0.5f));
	ClientRect.right = ClientRect.left + _width;
	ClientRect.top = labs(centerScreen.y - (_height * 0.5f));
	ClientRect.bottom = ClientRect.top + static_cast<long>(_height);
	AdjustWindowRect(&ClientRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);
	HWND hWnd = CreateWindowEx(
		0, _WndClass->lpszClassName, _title.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		ClientRect.left, ClientRect.top, ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top,
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

//void ModelDraw_Switch(int modelToDraw)
//{
//	if (modelToDraw == 0)
//	{
//		Gfx->LoadMesh("Cube.fbx", 50.0f, Gfx->gppMesh, 0);
//		OutputDebugString("CUBE.FBX");
//	}
//	else if (modelToDraw == 1)
//	{
//		Gfx->LoadMesh("Cube.fbx", 100.0f, Gfx->gppMesh, 1);
//		OutputDebugString("CUBE.FBX BUT BIGGER");
//	}
//	else if (modelToDraw == 2)
//	{
//		Gfx->LoadMesh("Cube.fbx", 10.0f, Gfx->gppMesh, 2);
//		OutputDebugString("CUBE.FBX BUT smaller");
//	}
//	else
//	{
//		if (modelToDraw < 0) currModel = MODEL_COUNT;
//		else if (modelToDraw > MODEL_COUNT) currModel = 0;
//	}
//}