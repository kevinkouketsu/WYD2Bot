#include <Windows.h>
#include "Struct.h"

#ifndef __SENDFUNC__
#define __SENDFUNC__

void SendRequestLogin(char *login, char *password);
void SendToken(char *number);
void SendRequestEnterWorld(INT32 charId);
void SendChat(char *);
void SendWhisper(char *nick, char *msg);
void SendSignal(INT32 packetId);
void SendAction(INT32 posX, INT32 posY, INT32 targetX, INT32 targetY, INT32 speed, INT32 type);
void SendAttack(INT32 mobId, INT32 skillId);
void SendAttack(st_Target *target, INT32 skillId);
void SendRequestCreateAutoTrade();
void SendRequestUseItem(INT32 invType, INT32 slotId, INT32 dstType, INT32 dstSlot);
void SendArroba(char *msg);

#endif