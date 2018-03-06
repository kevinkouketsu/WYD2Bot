#include "CClient.h"
#include "Main.h"
#include "CSocket.h"
#include "SendFunc.h"
#include "Basedef.h"
#include "GetFunc.h"

void ProcessSecTimer()
{
	INT32 sec = Client.SecCounter;
	
	if(!(sec % 3) && node->Queue)
	{
		st_Position pos;
		pos.X = node->Route[node->RouteActual].X;
		pos.Y = node->Route[node->RouteActual].Y;

		if(pos.X == 0 || pos.Y == 0)
		{
			node->Queue       = 0;
			node->RouteActual = 0;
			node->RouteCount  = 0;
		}
		else
		{
			SendAction(Client.Position.X, Client.Position.Y, node->Route[node->RouteActual].X, node->Route[node->RouteActual].Y, Client.Mob.Status.Move.Speed, 0);

			Client.Position.X = node->Route[node->RouteActual].X;
			Client.Position.Y = node->Route[node->RouteActual].Y;
		
			ServerMessage("Caminhou para %dx %dy", Client.Position.X, Client.Position.Y);

			node->RouteActual++;
			if(node->RouteActual >= node->RouteCount)
			{
				node->Queue       = 0;
				node->RouteActual = 0;
				node->RouteCount  = 0;

				char map = GetAttribute(Client.Position.X, Client.Position.Y);
				if(map & 16)
					ServerMessage("Há um teleporte neste local: %dx %dy", Client.Position.X, Client.Position.Y);

				if(Client.Water.Moving)
					Client.Water.Moving = 0;
			}
		}
	}

	if(!(sec % 2) && !node->Queue)
		Client.MacroSystem();

	if(!(sec % 2) && !node->Queue)
		Client.DoWater();

	if(Client.Session > USER_SELCHAR)
	{
		if(!(sec % 30))
			SendSignal(0x3A0);
	}

	Client.SecCounter ++;
}