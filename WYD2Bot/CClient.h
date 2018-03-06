#include <Windows.h>

#ifndef __CCLIENT__
#define __CCLIENT__

#include "Struct.h"

#define USER_EMPTY       0   
#define USER_ACCEPT      1   
#define USER_LOGIN       2   


#define USER_SELCHAR     11  
#define USER_CHARWAIT    12
#define USER_CREWAIT     13   
#define USER_DELWAIT     14   

#define USER_PLAY        22 

#define MAX_AFFECT				32

class CClient
{
public:
	CClient();
	~CClient();

	void GetNextSkill();
	void MacroSystem();
	void ClearEnemyList();
	void RemoveFromMobGrid(INT32 index);
	BOOL CurrentIsValid();
	BOOL UseItem(st_Item *item);
	BOOL GetFirstSlot(INT32 itemId);
	BOOL AutoPot(INT32 hpPercent, INT32 mpPercent);
	BOOL AutoFeed(INT32 feedPercent);
	BOOL ReadSkillBar();
	BOOL Physical();
	BOOL Magical();
	INT32 AddEnemyList(INT32 enemyId, INT32 distance);
	INT32 SelectEnemy();
	INT32 PacketControl(BYTE *pBuffer, INT32 size);
	INT32 AutoBuff();
	INT32 DoWater();

	INT32 clientId;
	INT32 charId;
	INT32 SecCounter;
	INT32 Token;
	INT32 Session;
	INT32 Range;
	struct
	{
		INT32 Status;
		INT32 Delay;
	} Event;

	st_Affect Affect[MAX_AFFECT];

	p10A charList;
	st_Mob Mob;

	st_Position Position;

	char SkillBar[2][10];
	INT32 Transform;

	//

	struct
	{
		INT32 Status;
		INT32 Type; // 0 = N, 1 = M, 2 = A
		INT32 Room;
		INT32 Moving;
	} Water;

	struct
	{
		char Name[16];

		st_Status Status;
		st_Position Position;
	
		INT16 GuildIndex;
		INT16 Index;

		st_Affect Affect[32];
	} Players[30000];

	WORD LeaderId;

	struct
	{
		WORD ClientID;
		char nickName[16];
	} Party[12];
	INT32 SelectedMob;

	struct
	{
		// Físico ou Mágico
		INT32 Type;

		
		// Monstro atual que está sendo atacado
		INT32 CurrentId;

		// SkillBar - Em qual skill da barra está 
		INT32 BarIndex;

		// skillId que será atacado
		INT32 SkillId;

		char SkillBar[10];

		INT32 Delay[256];

		INT32 OutOfPosition;
		st_Position Position;
	} Macro;

	INT32 EnemyList[10][2];

	st_Item Storage[128];

	p397 AutoTrade;

	struct
	{
		char Question[96];
		char Answer[4][32];
	} Quiz[64];

	INT32 TotalQuiz;
};

extern CClient Client;

#endif