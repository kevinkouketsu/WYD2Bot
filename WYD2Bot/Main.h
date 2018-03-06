#pragma region __N_MAIN_H__

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h> 
#include <fcntl.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <string.h>        
#include <time.h>

#include "PathFinding.h"
#include "resource.h"
#include "Struct.h"

#define TIMER_SEC 0

typedef struct
{
	HWND hWndMain;
} stWindow;

extern Node *node;

// functions
INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ProcessSecTimer();

void SendCommand(char *command);
extern stWindow g_pWindow;

#pragma endregion