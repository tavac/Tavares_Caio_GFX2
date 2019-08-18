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

void Timer::StartTimer(Timer* t)
{
	t->timeStart = static_cast<float>(GetTickCount64());
	t->timeCur = static_cast<float>(GetTickCount64());
}

float Timer::TimeSinceStart(Timer* t)
{
	return t->timeCur - t->timeStart;
}

float Timer::TimeSinceTick(Timer* t)
{
	t->timeCur = static_cast<float>(GetTickCount64());
	t->deltaTime = (t->timeCur - t->timeStart) / 1000;
	return t->deltaTime;
}