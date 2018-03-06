#include "Main.h"
#include "CClient.h"
#include "Basedef.h"
#include "GetFunc.h"
#include "PathFinding.h"

Node::Node()
{
	RouteCount = 0;
	RouteActual = 0;
	Queue = 0;
}

Node::~Node()
{

}

INT32 Node::Path(INT32 startX, INT32 startY, INT32 endX, INT32 endY)
{
	INT32 x = 0, 
		  y = 0;

	INT32 dx = 0,
		  dy = 0;
	
	INT32 lowestf = 100000;
	
	RouteCount = 0;
	RouteActual = 0;
	memset(Route, 0, sizeof Route);

	// lista de possibilidades
	memset(&List, 0, sizeof List);

	INT32 currentX = startX,
		  currentY = startY;

	List[startY][startX].Open = 1;
	
	if(startX == endX && startY == endY)
		return 0;

	while((currentX != endX || currentY != endY))
	{
		INT32 countF = 0;
		if(lowestf == 1000000 && currentX != startX && currentY != startY)
			break;

		List[currentY][currentX].Closed = true;
		List[currentY][currentX].Open   = false;

		lowestf = 1000000;
		Route[RouteCount].X = currentX;
		Route[RouteCount].Y = currentY;

		RouteCount++;

		INT32 tmpX = currentX,
			  tmpY = currentY;

		for(dy = -1; dy<=1;dy++)
		{
			for(dx = -1; dx <= 1; dx++)
			{
				if(g_pHeightGrid[currentY + dy][currentX + dx] == 127)
					continue;

				if(!List[currentY + dy][currentX + dx].Closed)
				{
					if(!List[currentY + dy][currentX + dx].Open)
					{
						List[currentY + dy][currentX + dx].Open = true;
						List[currentY + dy][currentX + dx].Closed = false;
						List[currentY + dy][currentX + dx].ParentX = currentX;
						List[currentY + dy][currentX + dx].ParentY = currentY;

						if(dx != 0 && dy != 0)
							List[currentY + dy][currentX + dx].G = 14;
						else
							List[currentY + dy][currentX + dx].G = 10;
						
						List[currentY + dy][currentX + dx].H = (abs(endX - (currentX + dx)) + abs(endY - (currentY + dy))) * 10;
						List[currentY + dy][currentX + dx].F = List[currentY + dy][currentX + dx].G + List[currentY + dy][currentX + dx].H;

						if(List[currentY + dy][currentX + dx].F < lowestf)
						{
							List[tmpY][tmpX].Open = false;

							tmpX = currentX + dx;
							tmpY = currentY + dy;
							lowestf  = List[currentY + dy][currentX + dx].F ;
							List[tmpY][tmpX].Open = true;
						}

					//	printf("Adicionado %dx %dy na lista aberta com F %d. H: %d.\n", currentX + dx, currentY + dy,List[currentY + dy][currentX + dx].F, List[currentY + dy][currentX + dx].H);
					}
					else
					{
						if(dx == 0 || dy == 0)
						{
							if(List[currentY + dy][currentX + dx].G != 10)
							{
								List[currentY + dy][currentX + dx].G = 10;
								List[currentY + dy][currentX + dx].ParentX = currentX;
								List[currentY + dy][currentX + dx].ParentY = currentY;
								List[currentY + dy][currentX + dx].H = (abs(endX - (currentX + dx)) + abs(endY - (currentY + dy))) * 10;
								List[currentY + dy][currentX + dx].F = List[currentY + dy][currentX + dx].G + List[currentY + dy][currentX + dx].H;
								
								if(List[currentY + dy][currentX + dx].F < lowestf)
								{
									List[tmpY][tmpX].Open = false;

									tmpX = currentX + dx;
									tmpY = currentY + dy;
									lowestf  = List[currentY + dy][currentX + dx].F ;
									List[tmpY][tmpX].Open = true;
								}
						//		printf("Encontrado melhor caminho\n");
							}
						}
					}
				}
			}
		}

		currentX = tmpX;
		currentY = tmpY;
	}

	return true;
}
/*
INT32 Node::Path(st_Position *current, st_Position *target)
{
	// position temporary
	st_Position tmpCur;
	tmpCur.X = current->X;
	tmpCur.Y = current->Y;

	INT32 step = 0;
	while(true)
	{
		INT32 posX = tmpCur.X,
			  posY = tmpCur.Y;

		if(posX == target->X && posY == target->Y)
			break;

		// N = y + 1
		// S = y - 1
		// L = x + 1
		// O = x - 1

		double northD	= GetDistance(tmpCur.X, tmpCur.Y, posX, posY + 1);
		double southD	= GetDistance(tmpCur.X, tmpCur.Y, posX, posY - 1);
		double eastD	= GetDistance(tmpCur.X, tmpCur.Y, posX + 1, posY);
		double westD	= GetDistance(tmpCur.X, tmpCur.Y, posX - 1, posY);
		
		char north		= g_pHeightGrid[posY + 1][posX];
		char south		= g_pHeightGrid[posY - 1][posX];
		char east		= g_pHeightGrid[posY][posX + 1];
		char west		= g_pHeightGrid[posY][posX - 1];

		INT32 mobNorth	= g_pMobGrid[posY + 1][posX];
		INT32 mobSouth	= g_pMobGrid[posY - 1][posX];
		INT32 mobEast	= g_pMobGrid[posY][posX + 1];
		INT32 mobWest	= g_pMobGrid[posY][posX - 1];

		if(mobNorth != 0)
			north = 127;

		if(mobSouth != 0)
			south = 127;

		if(mobEast != 0)
			east = 127;

		if(mobWest != 0)
			west = 127;

	}
}*/