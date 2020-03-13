#include"pch.h"
#include"UnixFileSystem.h"

int main()
{
	initSystem();
	if (login() == STATUS_OK)
	{
		//displayCommands();
		commandDispatcher();
	}
	//displayCommands();
	//commandDispatcher();
}