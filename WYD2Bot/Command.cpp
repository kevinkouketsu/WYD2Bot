#include "Main.h"
#include "CClient.h"
#include "PathFinding.h"
#include "SendFunc.h"
#include "Basedef.h"
#include "CSocket.h"
#include "GetFunc.h"

void SendCommand(char *command)
{
	char cmd[32];
	char value[108];

	INT32 ret = sscanf(command, "#%s %[^\n]", cmd, value);
	if(ret != 2)
	{
		MessageBoxA(NULL, "Comando inválido", "Erro ao enviar comando!", MB_OK);

		return;
	}

	if(!strcmp(cmd, "move"))
	{
		INT32 x = -1,
			  y = -1;

		ret = sscanf(value, "%d %d", &x, &y);
		if(x == -1 || y == -y || ret != 2)
		{
			MessageBoxA(NULL, "Comando inválido", "Erro ao enviar comando!", MB_OK);
			return;
		}
		
		node->Path(Client.Position.X, Client.Position.Y, x, y);
		
		ServerMessage("Movendo de %dx %dy para %dx %dy", Client.Position.X, Client.Position.Y, x, y);
		node->Queue = 1;

		RefreshStatus();
	}
	else if(!strcmp(cmd, "use"))
	{
		INT32 slotId = Client.GetFirstSlot(atoi(value));
		if(slotId == -1)	
		{
			ServerMessage("Não foi encontrado o item %s (%d)", ItemList[atoi(value)].Name, atoi(value));

			return;
		}

		Client.UseItem(&Client.Mob.Inventory[slotId]);
		SendRequestUseItem(INV_TYPE, slotId, 0, 0);
	}
	else if(!strcmp(cmd, "water"))
	{
		INT32 status = 0,
			  type   = 0;

		ret = sscanf(value, "%d %d", &status, &type);

		Client.Water.Status = status;
		Client.Water.Type	= type;

		ServerMessage("A água foi %s. Type: %d", (status) ? "ativada" : "desativada", type);
	}
	else if(!strcmp(cmd, "party"))
	{
		INT32 leaderId = 0;
		char nickname[16];
		
		ret = sscanf(value, "%16s %d", nickname, &leaderId);
		if(!strncmp(nickname, "exit", 4))
		{
			p37E packet;
			memset(&packet, 0, sizeof packet);

			packet.Header.PacketId = 0x37E;
			packet.Header.Size     = sizeof packet;
			packet.Header.ClientId = Client.clientId;

			packet.mobId = 0;
			
			cServer.AddMessage((char*)&packet, sizeof packet);
			ServerMessage("Solicitação para sair do grupo!", nickname);
		}
		else if(!strncmp(nickname, "kick", 4))
		{
			p37E packet;
			memset(&packet, 0, sizeof packet);

			packet.Header.PacketId = 0x37E;
			packet.Header.Size     = sizeof packet;
			packet.Header.ClientId = Client.clientId;

			packet.mobId = leaderId;
			
			cServer.AddMessage((char*)&packet, sizeof packet);
			ServerMessage("Solicitação para kickar %d", leaderId);
		}
		else if(!strncmp(nickname, "invt", 4))
		{
			p37F packet;
			memset(&packet, 0, sizeof packet);

			packet.Header.PacketId = 0x37F;
			packet.Header.Size     = sizeof packet;
			packet.Header.ClientId = Client.clientId;

			packet.leaderId = Client.clientId;
			packet.targetId = leaderId;

			packet.Level = Client.Mob.Status.Level;
			packet.curHP = Client.Mob.Status.curHP;
			packet.maxHP = Client.Mob.Status.maxHP;

			strncpy(packet.nickName, Client.Mob.Name, 12);

			cServer.AddMessage((char*)&packet, sizeof packet);
			ServerMessage("Enviado convite para %s (%d)", Client.Players[leaderId].Name, leaderId);
		}
		else 
		{
			p3AB packet;
			memset(&packet, 0, sizeof packet);

			packet.Header.Size = sizeof packet;
			packet.Header.PacketId = 0x3AB;
			packet.Header.ClientId = Client.clientId;

			packet.liderID = leaderId;
			strncpy(packet.nickName, nickname, 16);

			cServer.AddMessage((char*)&packet, sizeof packet);

			ServerMessage("Aceitado grupo de %s", nickname);
		}
	}
	else if(!strcmp(cmd, "tele"))
	{
		SendSignal(0x290);

		ServerMessage("Enviado pedido de uso de teleporte!");
	}
	else if(!strcmp(cmd, "transform"))
	{
		Client.Transform = atoi(value);

		ServerMessage("Setado a transformação como %d", atoi(value));
	}
}