#include "Main.h"
#include "CSocket.h"
#include "CClient.h"
#include "SendFunc.h"
#include "Basedef.h"

INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_TIMER:
		{
			if(wParam == TIMER_SEC)
				ProcessSecTimer();
		}
		break;

	case WM_COMMAND:
	{
		switch(LOWORD(wParam))
		{
		case IDC_Charlist:
			{
				switch (HIWORD(wParam)) 
				{ 
				case LBN_SELCHANGE:
					{
					}
					break;
				}
			}
			break;
		case IDC_Mobs:
			{
				switch (HIWORD(wParam)) 
				{ 
				case LBN_SELCHANGE:
					{
						HWND hwndList = GetDlgItem(hDlg, IDC_Mobs); 

						// Get selected index.
						int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0); 
						
						char buffer[32];
						SendMessage(hwndList, LB_GETTEXT, (WPARAM)(int)(lbItem), (LPARAM)(&buffer));

						INT32 mobId = -1;
						sscanf(buffer, "%*s (%d)", &mobId);

						if(mobId <= 0 || mobId >= 30000)
							return false;

						Client.SelectedMob = mobId;
						RefreshMobStatus();
					}
					break;
				}
			}
			break;
		case IDC_Clear:
			{
				HWND hwndList = GetDlgItem(hDlg, IDC_MessageList); 

				SendMessage(hwndList, LB_RESETCONTENT, 0, 0);
			}
			break;
		case IDC_Reviver:
			{
				SendSignal(0x289);

				ServerMessage("Solicitado para reviver!");
			}
			break;
		case IDC_Logar:
			{
				if(cServer.GethServer() == SOCKET_ERROR && !cServer.Connect())
				{
					MessageBox(hDlg, "Falha ao tentar conectar ao servidor!!", "Erro de conexão", MB_ICONERROR);

					return false;
				}
				else if(cServer.pRecvBuffer == NULL || cServer.pSendBuffer == NULL)
				{
					if(cServer.pRecvBuffer == NULL)
						cServer.pRecvBuffer = new char[RECV_BUFFER_SIZE];

					if(cServer.pSendBuffer == NULL)
						cServer.pSendBuffer = new char[RECV_BUFFER_SIZE];
				}

				char username[16];
				GetDlgItemText(hDlg, IDC_Login, username, 16);
			
				char password[16];
				GetDlgItemText(hDlg, IDC_Login2, password, 16);

				if(!username[0])
				{
					MessageBox(hDlg, "Digite um nome de usuário", "Nome de usuário muito curto", MB_ICONERROR);

					return false;
				}

				SendRequestLogin(username, password);
			}
			break;
			
		case IDC_SendToken:
			{
				if(Client.Token || Client.Session != USER_SELCHAR)
				{
					MessageBoxA(NULL, "Você já enviou sua senha numérica ou não está na seleção de personagem!", "Erro", MB_OK);

					return false;
				}

				char number[6];
				GetDlgItemText(g_pWindow.hWndMain, IDC_Token, number, 6);

				if(number[0])
				{
					SendToken(number);

					ServerMessage("Senha numérica %s enviada", number);
				}
				else
					MessageBoxA(NULL, "Digite uma senha numérica", "Erro ao logar", MB_OK);
			}
			break;
		case IDC_EnterWorld:
			{
				if(Client.Session != USER_SELCHAR)
				{
					MessageBoxA(NULL, "Para logar em um personagem, você deve ir para a seleção de personagem", "Erro ao logar", MB_OK);

					return false;
				}

				if(!Client.Token)
				{
					MessageBoxA(NULL, "Você não enviou a senha numérica correta!", "Erro ao logar", MB_OK);

					return false;
				}

				HWND hwndList = GetDlgItem(hDlg, IDC_Charlist); 

				// Get selected index.
				int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0); 
						
				char buffer[32];
				SendMessage(hwndList, LB_GETTEXT, (WPARAM)(int)(lbItem), (LPARAM)(&buffer));

				INT32 charId = -1;
				sscanf(buffer, "%*s (%d)", &charId);

				if(charId < 0 || charId >= 4)
				{
					MessageBox(g_pWindow.hWndMain, "Selecione um personagem!", "Erro ao logar", MB_OK);

					return false;
				}

				ServerMessage("Logando no personagem %s (%d)", Client.charList.CharList.Name[charId], charId);

				SendRequestEnterWorld(charId);
			}
			break;
		case IDC_SendChat:
			{
				char chat[96];
				GetDlgItemText(hDlg, IDC_Chat, chat, 96);
				
				if(!chat[0])
				{
					MessageBox(NULL, "Digite uma mensagem para ser enviada!", "Erro ao enviar chat", MB_OK);

					return false;
				}

				if((chat[0] >= 'a' && chat[0] <= 'z') || (chat[0] >= 'A' && chat[0] <= 'Z') || (chat[0] >= '0' && chat[0] <= '9'))
					SendChat(chat);
				else if(chat[0] == '#')
					SendCommand(chat);
				else if(chat[0] == '@')
					SendArroba(chat);
				else if(chat[0] == '/')
				{
					char nickname[16];
					char message[96];

					INT32 ret = sscanf(chat, "/%16s %[^\n]", nickname, message);
					if(ret < 1)
					{
						MessageBoxA(NULL, "Digite o player/comando que deseja enviar!", "Erro", MB_OK);

						return false;
					}

					SendWhisper(nickname, message);
					AddChat("[PM] %s> %s", Client.Mob.Name, message);
				}
			}
			break;
		case IDC_AutoTrade:
			{
				if(!ReadAutoTrade())
				{
					MessageBoxA(NULL, "Falha ao ler Autotrade.txt", "Erro", MB_OK);

					return false;
				}

				SendRequestCreateAutoTrade();
				ServerMessage("Tentativa de criar loja!");
			}
			break;
		case IDC_SelChar:
			{
				if(Client.Session != USER_PLAY)
				{
					MessageBoxA(NULL, "Você deve estar no jogo para ir para a seleção de personagem!!", "Erro", MB_OK);

					return false;
				}

				SendSignal(0x215);
			}
			break;
		}
		break;
	}
	break;
	case WM_SERVER:
		{
			if (WSAGETSELECTEVENT(lParam) != FD_READ)
			{
				closesocket(cServer.GethServer());
				
				cServer.SethServer(SOCKET_ERROR);
				Client.Session = USER_EMPTY;

				ServerMessage("Você foi desconectado!");
				return false;
			}

			int ret = cServer.Receive();
			if(ret == FALSE || ret == -1)
			{
				ret = WSAGetLastError();
				
				closesocket(cServer.GethServer());
				break;
			}

			int Error = FALSE;
			int ErrorCode = FALSE;
			while (TRUE)
			{
				char* Msg = cServer.ReadMessage(&Error, &ErrorCode);
				if (Msg == NULL)
					break;

				if (Error == 2)
				{
					closesocket(cServer.GethServer());

					break;
				}

				Client.PacketControl((BYTE*)Msg, ErrorCode);
			}
		}
		break;
	case WM_CLOSE:
		if(MessageBox(hDlg, "Você deseja fechar o aplicativo?", "Close", MB_ICONQUESTION | MB_YESNO) == IDYES)
			DestroyWindow(hDlg);
		return false;

	case WM_DESTROY:
		PostQuitMessage(0);
		return false;
	}

	return false;
}