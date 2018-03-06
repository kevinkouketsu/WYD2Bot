#include "Main.h"
#include "CClient.h"
#include "CSocket.h"
#include "Basedef.h"
#include "GetFunc.h"

void SendRequestLogin(char *login, char *password)
{
	p20D packet;
	memset(&packet, 0, sizeof packet);

	packet.Header.PacketId = 0x20D;
	packet.Header.Size     = sizeof packet;
	packet.Header.ClientId = 0;

	strncpy(packet.Username, login, 12);
	strncpy(packet.Password, password, 12);

	packet.CliVer = 9999;//7000;//8146;
	EncryptVersion(&packet.CliVer);
	
	UUID uuid;
	UuidCreateSequential(&uuid);    // Ask OS to create UUID

	for(INT32 i = 2; i < 8; i++)
		packet.AdapterInfos[i - 2] = uuid.Data4[i];

	cServer.AddMessage((char*)&packet, sizeof packet);

	ServerMessage("Tentativa de logar na conta %s", login);
}

void SendToken(char *number)
{
	pFDE packet; 
	memset(&packet, 0, sizeof packet);

	packet.Header.PacketId = 0xFDE;
	packet.Header.Size     = sizeof packet;
	packet.Header.ClientId = Client.clientId;

	strncpy(packet.num, number, 6);
	
	cServer.AddMessage((char*)&packet, sizeof packet);
}

void SendRequestEnterWorld(INT32 charId)
{
	p213 packet;
	memset(&packet, 0, sizeof packet);

	packet.Header.PacketId = 0x213;
	packet.Header.Size     = sizeof p213;
	packet.Header.CheckSum = Client.clientId;

	packet.CharIndex = charId;
	cServer.AddMessage((char*)&packet, sizeof packet);
}

void SendArroba(char *msg)
{
	p334 packet;
	memset(&packet, 0, sizeof packet);

	packet.Header.PacketId = 0x334;
	packet.Header.Size     = sizeof packet;
	packet.Header.ClientId = Client.clientId;

	strncpy(packet.eValue,  msg, 96);
	
	cServer.AddMessage((char*)&packet, sizeof packet);
}

void SendChat(char* msg)
{
	p333 packet;
	memset(&packet, 0, sizeof packet);

	packet.Header.PacketId = 0x333;
	packet.Header.Size     = sizeof packet;
	packet.Header.ClientId = Client.clientId;

	strncpy(packet.eChat, msg, 96);

	cServer.AddMessage((char*)&packet, sizeof packet);

	AddChat("[%s]> %s", Client.Mob.Name ,msg);
}

void SendWhisper(char *nick, char *msg)
{
	p334 packet;
	memset(&packet, 0, sizeof packet);

	packet.Header.PacketId = 0x334;
	packet.Header.Size     = sizeof packet;
	packet.Header.ClientId = Client.clientId;

	strncpy(packet.eCommand, nick, 16);
	strncpy(packet.eValue,   msg, 96);
	
	cServer.AddMessage((char*)&packet, sizeof packet);
}

void SendSignal(INT32 packetId)
{
	PacketHeader Header;

	Header.ClientId = Client.clientId;
	Header.PacketId = packetId;
	
	cServer.AddMessage((char*)&Header, sizeof Header);
}

void SendAction(INT32 posX, INT32 posY, INT32 targetX, INT32 targetY, INT32 speed, INT32 type)
{
	p36C packet;
	memset(&packet, 0, sizeof packet);

	packet.Header.PacketId = 0x36C;
	packet.Header.Size     = sizeof packet;
	packet.Header.CheckSum = Client.clientId;

	packet.Destiny.X = targetX;
	packet.Destiny.Y = targetY;

	packet.LastPos.X = posX;
	packet.LastPos.Y = posY;

	packet.MoveSpeed = speed;
	packet.MoveType  = type;

	cServer.AddMessage((char*)&packet, sizeof packet);
}

void SendAttack(st_Target *target, INT32 skillId)
{
	INT32 mp = Client.Mob.Status.curMP;

	if(skillId >= 0 && skillId <= 110)
	{
		INT32 skillKind = ((skillId % 24) >> 3) + 1; // mastery
		INT32 saveMana = GetMobAbility(&Client.Mob, EF_SAVEMANA);
		INT32 mpCost = GetManaSpent(skillId, saveMana, skillKind);

		if(mpCost > mp)
		{
			ServerMessage("Mana insuficiente. Mana atual: %d de %d", mp, mpCost);

			return;
		}

		mp -= mpCost;
		
		Client.Macro.Delay[skillId] = clock();
	}

	p367 packet;
	memset(&packet, 0, sizeof packet);

	packet.Header.PacketId = 0x367;
	packet.Header.Size     = sizeof packet;
	packet.Header.ClientId = Client.clientId;

	packet.skillId		= skillId;
	packet.attackerId	= Client.clientId;

	packet.attackerPos.X = Client.Position.X;
	packet.attackerPos.Y = Client.Position.Y;

	packet.currentMp = Client.Mob.Status.curMP;

	if(skillId == -1)
	{
		INT32 wType = GetItemAbility(&Client.Mob.Equip[6], EF_WTYPE);
		if(wType == 101)
			skillId = 151;
	}

	packet.FlagLocal = 1;
	packet.Motion    = 4;
	packet.skillId   = skillId;

	memcpy(&packet.Target, target, sizeof st_Target * 13);

	for(INT32 i = 0; i < 13; i++)
	{
		if(packet.Target[i].Index <= 0)
			continue;

		if(skillId == -1 || skillId == 151)
			packet.Target[i].Damage = -2;
		else
			packet.Target[i].Damage = -1;
	}

	cServer.AddMessage((char*)&packet, sizeof packet);
}

void SendAttack(INT32 mobId, INT32 skillId)
{
	INT32 mp = Client.Mob.Status.curMP;

	if(skillId >= 0 && skillId <= 110)
	{
		INT32 skillKind = ((skillId % 24) >> 3) + 1; // mastery
		INT32 saveMana = GetMobAbility(&Client.Mob, EF_SAVEMANA);
		INT32 mpCost = GetManaSpent(skillId, saveMana, skillKind);

		if(mpCost > mp)
		{
			ServerMessage("Mana insuficiente. Mana atual: %d de %d", mp, mpCost);

			return;
		}

		mp -= mpCost;
		
		Client.Macro.Delay[skillId] = clock();
	}

	p367 packet;
	memset(&packet, 0, sizeof packet);

	packet.Header.PacketId = 0x367;
	packet.Header.Size     = sizeof packet;
	packet.Header.ClientId = Client.clientId;

	packet.skillId		= skillId;
	packet.attackerId	= Client.clientId;

	packet.attackerPos.X = Client.Position.X;
	packet.attackerPos.Y = Client.Position.Y;

	packet.currentMp = Client.Mob.Status.curMP;

	if(skillId == -1)
	{
		INT32 wType = GetItemAbility(&Client.Mob.Equip[6], EF_WTYPE);
		if(wType == 101)
			skillId = 151;
	}

	packet.FlagLocal = 1;
	packet.Motion    = 4;
	packet.skillId   = skillId;

	packet.Target[0].Index	= mobId;

	if(skillId == -1 || skillId == 151)
		packet.Target[0].Damage = -2;
	else
		packet.Target[0].Damage = -1;
	
	cServer.AddMessage((char*)&packet, sizeof packet);
}

void SendRequestCreateAutoTrade()
{
	p397 packet;
	memset(&packet, 0, sizeof packet);

	memcpy(&packet, &Client.AutoTrade, sizeof p397);

	packet.Header.PacketId = 0x397;
	packet.Header.Size     =  sizeof packet;
	packet.Header.ClientId = Client.clientId;

	for(INT32 i = 0; i < 12; i++)
	{
		if(packet.Gold[i] == 0)
		{
			packet.Slot[i] = -1;

			continue;
		}

		memcpy(&packet.Item[i], &Client.Storage[packet.Slot[i]], sizeof st_Item);
	}

	cServer.AddMessage((char*)&packet, sizeof p397);
}

void SendRequestUseItem(INT32 invType, INT32 slotId, INT32 dstType, INT32 dstSlot)
{
	p373 packet;
	memset(&packet, 0, sizeof packet);

	packet.Header.PacketId = 0x373;
	packet.Header.Size     = sizeof packet;
	packet.Header.ClientId = Client.clientId;

	packet.DstSlot = dstSlot;
	packet.DstType = dstType;

	packet.SrcSlot = slotId;
	packet.SrcType = invType;

	packet.PosX = Client.Position.X;
	packet.PosY = Client.Position.Y;
	
	cServer.AddMessage((char*)&packet, sizeof packet);
}