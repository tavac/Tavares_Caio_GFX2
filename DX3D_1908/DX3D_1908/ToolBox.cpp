#include "ToolBox.h"

void ToolBox::CleanUp(HWND* _hWnd)
{
	DestroyWindow(*_hWnd);
}

void ToolBox::ThrowErrorMsg(const char* ErrMsg)
{
	std::string tmpmsg = "CTERROR:-->> ";
	tmpmsg.append(ErrMsg);
	tmpmsg.append("\n");
	OutputDebugString(tmpmsg.c_str());
	MessageBox(nullptr, ErrMsg, "You've Got Mail", MB_OK | MB_ICONERROR);
}

std::string ToolBox::CommandProcesser(std::string command)
{
	std::string rtn;
	if (command.substr(0, 3) == "gc ")
	{
		for (UINT s = 3; s < (UINT)command.size(); s++)
			rtn.push_back(command.at(s));
	}
	else if (command.substr(0, 3) == "cls")
	{
		rtn = command.substr(0, 3);
	}
	else
	{
		rtn = "";
	}
		return rtn;


	//if string is this
	//call/do function
}

void Timer::StartTimer()
{
	timeStart = static_cast<float>(GetTickCount64());
	timeCur = static_cast<float>(GetTickCount64());
}

float Timer::TimeSinceStart()
{
	return timeCur - timeStart;
}

float Timer::TimeSinceTick()
{
	timeCur = static_cast<float>(GetTickCount64());
	if (deltaTime == 0)
		deltaTime = (timeCur - timeStart) / 100000000;
	deltaTime = (timeCur - deltaTime);
	deltaTime /= 100000000;
	return deltaTime;
}