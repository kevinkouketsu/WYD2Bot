#include <Windows.h>

#ifndef __GETFUNC__
#define __GETFUNC__

double GetDistance(int x1,int y1,int x2,int y2);
int GetItemSanc(st_Item *item);
short GetItemAbility(st_Item *itemPtr, int eff);
int GetMobAbility(st_Mob *usr, int eff);
int GetMaxAbility(st_Mob *usr, int eff);
INT32 GetManaSpent(INT32 skillId, INT32 saveMana, INT32 mastery);
st_Item *GetItemPointer(INT32 invType, INT32 invSlot);
INT32 GetAffectSlot(st_Affect *affect, INT32 buffId);
unsigned char GetAttribute(int posX, int posY);
double GetPercentLevelExp(INT32 level, INT32 ev, INT64 exp);
#endif