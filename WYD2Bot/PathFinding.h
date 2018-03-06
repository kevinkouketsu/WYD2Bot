#include <Windows.h>

#ifndef __PATHFINDING__
#define __PATHFINDING__

#include "Struct.h"

typedef struct
{
	INT32 ParentX;
	INT32 ParentY;
	INT32 Closed;
	INT32 Open  ;
	INT32 Walkable;

	INT32 F;
	INT32 G;
	INT32 H; 
} stNode;

#define MAX_ROUTE 256

class Node
{
public:
	Node();
	~Node();

	INT32 Queue;

	st_Position Route[MAX_ROUTE];
	stNode List[4096][4096];

	INT32 RouteCount;
	INT32 RouteActual;
	INT32 G;
	INT32 H;
	INT32 F;

	INT32 Path(INT32 startX, INT32 startY, INT32 endX, INT32 endY);
};

#endif