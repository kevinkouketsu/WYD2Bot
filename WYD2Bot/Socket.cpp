#include "Main.h"
#include "CSocket.h"

Server::Server()
{
	SethServer(SOCKET_ERROR);
}

Server::~Server()
{
}

INT32 Server::GethServer()
{
	return hServer;
}

WSADATA *Server::GetwSaData()
{
	return &wSaData;
}

SOCKADDR_IN *Server::GetService()
{
	return &Service;
}

void Server::SethServer(UINT32 value)
{
	hServer = value;
}

BOOL Server::Connect()
{
	bool Sucessful  = true;
	SOCKADDR_IN *service = GetService();

	ZeroMemory(service, sizeof SOCKADDR_IN);

	service->sin_family		= AF_INET;
	service->sin_addr.s_addr 	= inet_addr(9999);
	service->sin_port		= htons(999);

	WSADATA *wsa = GetwSaData();
	if(WSAStartup(MAKEWORD(1, 1), wsa) != NO_ERROR)
	{
		MessageBoxA(0, "WSAStartup error", "Server::WSAStartup", 4096);

		return false;
	}
	
	UINT32 hServer = -1;
	if((hServer = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) == INVALID_SOCKET)
	{
		MessageBoxA(0, "socket error", "Server::socket", 4096);

		return false;
	} 

	if(connect(hServer, (SOCKADDR*)service, sizeof SOCKADDR_IN) == SOCKET_ERROR)	
	{
		MessageBoxA(0, "connect error", "Server::connect", 4096);

		closesocket(hServer);
		return false;
	}
	
	if(WSAAsyncSelect(hServer, g_pWindow.hWndMain, WM_SERVER, FD_READ|FD_CLOSE) == SOCKET_ERROR)
	{
		MessageBoxA(0, "WSAAsyncSelect error", "Server::WSAAsyncSelect", 4096);

		closesocket(hServer);
		return false;
	}
	
	unsigned int InitCode = INITCODE;
	send(hServer, (char*)&InitCode, 4, 0);

	SethServer(hServer);
	return true;
}

BOOL Server::Receive()
{
	int Rest = RECV_BUFFER_SIZE - nRecvPosition;
	int tReceiveSize = recv(GethServer(), (char*)(pRecvBuffer + nRecvPosition), Rest, 0);

	if(tReceiveSize == SOCKET_ERROR || tReceiveSize == Rest)
		return FALSE;

	nRecvPosition = nRecvPosition + tReceiveSize;

	return TRUE;				  
}

BOOL Server::AddMessage(char *pMsg, int Size)
{
	char temp[256];
	PacketHeader *pSMsg = (PacketHeader *)pMsg;

	if(nSendPosition + Size >= SEND_BUFFER_SIZE)	
		return FALSE;

	if(KeyTable[0])
	{ // 004251CA
		INT32 hashIncrement = GetHashIncrement();
		if(hashIncrement > 15)
		{ 
			// checagem desnecessária 
			// 0042524C
			
		}
	}

	// check socket valid
	
	unsigned char iKeyWord  =	rand()%256;
	unsigned char KeyWord	=	KeyTable[iKeyWord * 2];
	unsigned char CheckSum	=	0;

	pSMsg->Size				=	Size;
	pSMsg->Key				=	iKeyWord;
	pSMsg->CheckSum			=	CheckSum;
	pSMsg->TimeStamp		=	GetCurrenttime();

	TimePacket = GetCurrentTime();

	if(pSMsg->PacketId == 0xFDE)
	{
		pFDE *packet = (pFDE*)(pSMsg);
		for(INT32 i = 0; i < 6;i++)
		{
			char val = (char)(packet->num[i]);
			if(!val)
				break;
				
			packet->num[i] ^= packet->Header.Key;
		}
	}
	unsigned char Sum1 = 0;
	unsigned char Sum2 = 0;
 	int			  pos  = KeyWord;

	for (int i = 4; i < Size; i++, pos++)
	{	
		Sum1 += pMsg[i];

		int rst = pos % 256;
		unsigned char Trans = KeyTable[(rst & 255) * 2 + 1];

		int mod = i & 0x3;
		if  (mod == 0) 
			pSendBuffer[nSendPosition + i] = pMsg[i] + (Trans << 1); 
		if  (mod == 1) 
			pSendBuffer[nSendPosition + i] = pMsg[i] - (Trans >> 3);
		if  (mod == 2)
			pSendBuffer[nSendPosition + i] = pMsg[i] + (Trans << 2);
		if  (mod == 3) 
			pSendBuffer[nSendPosition + i] = pMsg[i] - (Trans >> 5);

		Sum2 += pSendBuffer[nSendPosition + i];
	}
	
	CheckSum = Sum2 - Sum1;
	pSMsg->CheckSum = CheckSum;

	memcpy(pSendBuffer + nSendPosition, pMsg, 4);
	
	nSendPosition = nSendPosition + Size;
	
	return SendMessageA();
}

char* Server::ReadMessage(int *ErrorCode, int* ErrorType)
{
    *ErrorCode = 0;
 
    if(nProcPosition >= nRecvPosition)
    {
        nRecvPosition = 0;
        nProcPosition = 0;
        
		return NULL;
    }
 
    if((nRecvPosition - nProcPosition) < 12)
		return NULL;
 
	unsigned short Size = *((unsigned short*)(pRecvBuffer + nProcPosition));
	unsigned short CheckSum = *((unsigned char*)(pRecvBuffer + nProcPosition + 2));
 
    if(Size > MAX_MESSAGE_SIZE || Size < sizeof(PacketHeader))
    {
        nRecvPosition = 0;
        nProcPosition = 0;
       
		*ErrorCode   =  2;
        *ErrorType   =  Size;
        return NULL;
    }
 
    unsigned short Rest = nRecvPosition - nProcPosition;
    if(Size > Rest)
		return NULL;
 
	char*pMsg = (char*)&(pRecvBuffer[nProcPosition]);
    nProcPosition = nProcPosition + Size;
    if(nRecvPosition <= nProcPosition)
    {
        nRecvPosition = 0;
        nProcPosition = 0;
    }

    unsigned int i;
    int pos, Key;
    int sum1 = 0, sum2 = 0;

    pos = KeyTable[CheckSum * 2];
    for(i = 4; i < Size; i++, pos++)
    {
        sum1 += pMsg[i];
        Key = KeyTable[((pos & 0xFF) * 2) + 1];
        switch(i & 3)
        {
            case 0:
                    Key <<= 1;
                    Key &= 255;
                    pMsg[i] -= Key;
                    break;
            case 1:
                    Key >>= 3;
                    Key &= 255;
                    pMsg[i] += Key;
                    break;
            case 2:
                    Key <<= 2;
                    Key &= 255;
                    pMsg[i] -= Key;
                    break;
            case 3: default:
                    Key >>= 5;
                    Key &= 255;
                    pMsg[i] += Key;
                    break;
        }
        sum2 += pMsg[i];
    }
 
    sum2 &= 255;
    sum1 &= 255;

    if(sum2 != sum1)
	{
		*ErrorCode = 1;
		*ErrorType = Size;
	}

    return pMsg;
}

BOOL Server::SendMessageA()
{
	INT32 socket = GethServer();
	if(socket <= 0)
		return false;

	if(nSentPosition > 0)
		RefreshSendBuffer();

	if(nSendPosition > SEND_BUFFER_SIZE || nSendPosition < 0 || socket < 0)
		return false;

	if(nSentPosition > nSendPosition || nSentPosition > SEND_BUFFER_SIZE || nSentPosition < 0)
		return false;

	
	INT32 err = 0;
	INT32 LOCAL_68 = nSendPosition - nSentPosition;

	INT32 LOCAL_69 = send(socket, (char*)pSendBuffer + nSentPosition, LOCAL_68, 0);
	if(LOCAL_69 != -1)
		nSentPosition += LOCAL_69;
	else
		err = WSAGetLastError();

	if(err != 0)
		printf("ERROR IN SEND\n");

	if(nSendPosition >= nSendPosition || err != 0)
	{
		nSendPosition = 0;
		nSentPosition = 0;

		return true;
	}

	if(nSendPosition > SEND_BUFFER_SIZE)
		return false;

	return true;
}

void Server::RefreshSendBuffer()
{
	INT32 LOCAL_2 = nSendPosition - nSentPosition;
	if(LOCAL_2 <= 0)
		return;

	memcpy(pSendBuffer, pSendBuffer + nSentPosition, LOCAL_2);

	nSentPosition = 0;
	nSendPosition -= LOCAL_2;
}

void Server::RefreshRecvBuffer()
{	
	int left = nRecvPosition - nProcPosition;
	if(left <= 0) 
		return;

	memcpy(pRecvBuffer, pRecvBuffer + nProcPosition, left);

	nProcPosition = 0;
	nRecvPosition -= left;
}

void Server::SetHashIncrement(INT32 value)
{
	if(value < 0 || value >= 16)
		return;

	HashIncrement = value;
}

INT32 Server::GetCurrenttime()
{
	return (TimePacket + (GetCurrentTime() - LastPacket));
}

INT32 Server::GetHashIncrement()
{
	return HashIncrement;
}
