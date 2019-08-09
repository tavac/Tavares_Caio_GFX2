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
	if (command.substr(0, 3) == "gc ")
	{
		//for(int i = 0; i < 3; i++) command.;
		// return command
		return command;
	}
	else
	{
		command = "";
		return command;
	}


	//if string is this
	//call/do function
}

//// Initializes the timer.
//float ToolBox::StartTimer()
//{
//	init_time_stamp = std::chrono::
//	return 0.0f;
//}
//// Delta time since last call of TimerTick() or since StartTimer() if first time calling TimerTick().
//float ToolBox::TimerTick(std::chrono::steady_clock::time_point tp)
//{
//	std::chrono::duration<float> t_span = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - tp);
//	tp = std::chrono::steady_clock::now();
//	return t_span.count();
//}
//// Time since timer started.
//float ToolBox::TimerDelta()
//{
//	return (std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - init_time_stamp)).count();
//}
