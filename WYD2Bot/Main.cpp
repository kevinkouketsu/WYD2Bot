#include "Main.h"
#include "CClient.h"
#include "CSocket.h"
#include "Basedef.h"
#include "PathFinding.h"

Server cServer;
stWindow g_pWindow;
CClient Client;
Node *node;

void CreateConsole()
{
	AllocConsole();
			
	// Output fix  
	int outParam = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	FILE *stdout_stream = _fdopen(outParam, "w");

	*stdout = *stdout_stream;

	// Input fix 
	int inParam = _open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT);
	FILE *stdin_stream = _fdopen(inParam, "r");

	*stdin = *stdin_stream;
}

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	BOOL ret;
	MSG msg;
	
	node = new Node();

	g_pWindow.hWndMain = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_MainDlg), 0, MainDlgProc, 0);
	
	SetTimer(g_pWindow.hWndMain, TIMER_SEC, 500, NULL);

	ShowWindow(g_pWindow.hWndMain, nCmdShow);
	
	if(!ReadHeightMap())
		MessageBoxA(NULL, "Falha ao ler Heightmap.dat", "Erro ao ler", MB_OK);
	if(!ReadAttributeMap())
		MessageBoxA(NULL, "Falha ao ler Attributemap.dat", "Erro ao ler", MB_OK);
	
	ApplyAttribute((char*)g_pHeightGrid, 4096);

	ReadItemEffect();
	ReadItemList();
	ReadSkillData();
	ReadQuiz();

	memset(&g_pMobGrid, 0, sizeof g_pMobGrid);

	while((ret = GetMessage(&msg, 0, 0, 0)) != 0) 
	{
		if(ret == -1) /* error found */
			return -1;

		if(!IsDialogMessage(g_pWindow.hWndMain, &msg)) 
		{
			TranslateMessage(&msg); /* translate virtual-key messages */
			DispatchMessage(&msg); /* send it to dialog procedure */
		}
	}
	return TRUE;
}