#include "initmod.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

DLL_EXP void MainEntry () {
	printf("MainEntry was called\n");
	engineModule *enginePointer = new engineModule;
	if(enginePointer) {
		enginePointer->mainRoutineCall();
	}
	delete enginePointer;
}
BOOL APIENTRY DllMain (HINSTANCE hInst/* Library instance handle. */,DWORD reason, LPVOID reserved) {
	switch (reason) {
		  case DLL_PROCESS_ATTACH:
			   MainEntry();
			break;
			/*
		  case DLL_PROCESS_DETACH:
			   MainEntry();
			break;

		  case DLL_THREAD_ATTACH:
			   MainEntry();
			break;

		  case DLL_THREAD_DETACH:
			   MainEntry();
			break;
			*/
    }
    return TRUE;
}
