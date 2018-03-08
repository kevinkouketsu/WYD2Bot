#include <Windows.h>

#ifndef __STRUCT_H__
#define __STRUCT_H__

struct st_Item
{
	WORD Index;
	struct
	{
		BYTE Index;
		BYTE Value;
	} Effect[3];
};

#define EF1 Effect[0].Index
#define EFV1 Effect[0].Value
#define EF2 Effect[1].Index
#define EFV2 Effect[1].Value
#define EF3 Effect[2].Index
#define EFV3 Effect[2].Value

struct st_Position {
	WORD X, Y;
};

typedef struct
{
	BYTE Index;
	BYTE Master;

	WORD Value;
	DWORD Time;
} st_Affect;

struct sItemData
{
    char Name[64];

    short Mesh1;
    int Mesh2;

    short Level;
    short Str;
    short Int;
    short Dex;
    short Con;

    struct
    {
        short Index;
        short Value;
    } Effect[12];

    int Price;
    short Unique;
    unsigned short Pos;
    short Extreme;
    short Grade;
};

typedef struct
{
    int Points;
    int Target;
    int Mana;
    int Delay;
    int Range;
    int InstanceType; // Affect[0].Index
    int InstanceValue; // Affect[0].Value
    int TickType; // Affect[1].Index
    int TickValue; // Affect[1].Value
    int AffectType; // Affect[2].Index
    int AffectValue; // Affect[2].Value
    int Time;
    char Act[16];
    int InstanceAttribute;
    int TickAttribute;
    int Aggressive;
    int Maxtarget;
    int PartyCheck;
    int AffectResist;
    int Passive_Check;
	int ForceDamage;
} sSpellData;

typedef struct
{
	DWORD Level;
	DWORD Defense;
	DWORD Attack;

	union
	{
		struct  {
			BYTE Merchant : 4;
			BYTE Direction : 4;
		};
		BYTE Value;
	} Merchant;

	union {
		struct {
			BYTE Speed : 4;
			BYTE ChaosRate : 4;
		};
		BYTE Value;
	} Move;

	INT32 maxHP, maxMP;
	INT32 curHP, curMP;

	WORD STR, INT;
	WORD DEX, CON;

	WORD Mastery[4];
} st_Status;

typedef struct
{
	char Name[16]; // 0 - 15
	char CapeInfo; // 16 

	union
	{
		struct
		{
			BYTE Merchant : 6;
			BYTE CityID :  2;
		};
		BYTE Value;
	} Info; // 17

	WORD GuildIndex; // 18 19
	BYTE ClassInfo; // 20

	union
	{
		struct
		{
			BYTE SlowMov : 1;
			BYTE DrainHP : 1;
			BYTE VisionDrop : 1;
			BYTE Evasion : 1;
			BYTE Snoop : 1;
			BYTE SpeedMov : 1;
			BYTE SkillDelay : 1;
			BYTE Resist : 1;
		};

		BYTE Value;
	} AffectInfo; // 21

	union 
	{
		struct
		{
			WORD Mystical_GetQuest : 1; // 1
			WORD Mystical_CanGetAmuleto : 1; // 2
			WORD Mystical_GetAmuleto : 1; // 4
			WORD Reset50 : 1; // 8
			WORD Reset100 : 1; // 16
			WORD MestreHab : 1; // primeiro reset free? 32
			WORD Treinadores : 4; // 64 128 256 512
			WORD All : 6;
		};

		WORD Value;
	} QuestInfo;

	int Gold; // 24 - 27

	int Unkw1; // 28 - 31
	INT64 Exp; // 32 - 39

	st_Position Last; // 40 - 43
	st_Status bStatus; // 44 - 91
	st_Status Status; // 92 - 139

	st_Item Equip[16]; // 140 - 267
	st_Item Inventory[64]; // 268 - 779

	DWORD Learn; // 780 - 783
	DWORD Mimimimimimi; // 774 - 787
	WORD StatusPoint; // 788 - 789
	WORD MasterPoint; // 790 - 791
	WORD SkillPoint; //792 - 793

	BYTE Critical; // 794
	BYTE SaveMana; // 795

	char SkillBar1[4]; // 796 - 799
	char GuildMemberType; // 800

	BYTE MagicIncrement; // 801
	BYTE RegenHP; // 802 
	BYTE RegenMP; // 803

	struct
	{
		BYTE Fogo;
		BYTE Gelo;
		BYTE Sagrado;
		BYTE Trovao;
	} Resist; // 804 - 807
} st_Mob;

typedef struct
{
	WORD Size;
	BYTE Key;
	BYTE CheckSum;
	WORD PacketId;
	WORD ClientId;
	DWORD TimeStamp;
} PacketHeader;

typedef struct {
	PacketHeader Header;
	char Password[12];
	char Username[16];

	char Unknow[52];
	UINT32 CliVer;
	UINT32 Unknow_84;

	char AdapterInfos[16];
} p20D;

typedef struct
{
	PacketHeader Header;

	char Message[96];
} p101;

typedef struct 
{
	PacketHeader Header;
	INT32 npcId;
	INT32 click;
} p28B;

typedef struct
{
	PacketHeader Header;
	char Title[96];
	char Ans[4][32];
} p1C6;

typedef struct
{
	PacketHeader Header;
	BYTE Keys[16];

	INT32 Unkw;

	struct
	{
		INT16 PositionX[4];
		INT16 PositionY[4];

		char Name[4][16];

		st_Status Status[4];
		st_Item Equip[4][16];

		UINT16 GuildIndex[4];

		int Gold[4];
		INT64 Exp[4];
	} CharList;
	
	st_Item Storage[128];
	UINT32 GoldStorage;

	char UserName[16];
	BYTE Unknow[12];
} p10A;

typedef struct 
{
	PacketHeader Header;
	char num[6];
	char unk[10];
	INT32 RequestChange;
} pFDE; 

typedef struct
{
	PacketHeader Header;
    int CharIndex;
    char Zero[18];
} p213;

typedef struct
{
	PacketHeader Header; // 0 - 11
	st_Position WorldPos; // 12 - 15
	st_Mob Mob; // 16 - 823

	st_Status SubClass;
	char Tab[26];
	char GuildName[30];
	int NextMovement;

	BYTE unknow[108];

	struct
	{
		short SlotIndex; // 1040 - 1041
		short ClientIndex; // 1042 - 1043
		short evasao; // 1044 - 1045

		char SkillBar2[16]; // 1046 - 1061
		WORD Unknow; // 1062 - 1063
		int Hold; // 1064 - 1065
	} Other;
	int HPDRAIN;
	int AtkSpeed;
    int Absorcao; 
    int TimeStamp; 

	int BonusXP;
	int BonusPerf;
	int BonusDrop;

	int ControlMP;
	st_Position Old_Pos;
	st_Affect Affect[32];
	int ClassMaster;

	INT64 Unknow22;
	INT8 Unknoww[22];
	char SkillBar[2][10];

	BYTE Unknow[286];
} p114; 

typedef struct
{
	PacketHeader Header;
	char SkillBar1[4];
    char SkillBar2[16];
} p378;

typedef struct 
{
    PacketHeader Header;
    char eChat[96];    
}p333;

typedef struct
{
	PacketHeader Header; // 0 - 11

	st_Status Status; // 12 - 59

	BYTE Critical; // 60
	BYTE SaveMana; // 61

	struct
	{
		BYTE Time;
		BYTE Index;
	} Affect[32]; //62 - 125

	WORD GuildIndex; // 126 - 127

	BYTE RegenHP, // 128 
		 RegenMP; // 129

	BYTE Resist1; // 130 
	BYTE Resist2; // 131
	BYTE Resist3; // 132
	BYTE Resist4; // 133

	WORD Unknow; // 134 - 135
	WORD CurrHP; // 136 - 137
	DWORD CurrMP; // 138 - 142
	BYTE Unknow_2; // 143
	BYTE MagicIncrement; // 144 - 147
	DWORD Unknow_148; // 148 - 151
} p336;

// Spawn Info
typedef struct 
{
	PacketHeader Header; // 0 - 11

    // Spawn Info
    struct
    {
        short X, Y;
    } Current; // 12 - 15

    short Index; // 16 - 17

    char Name[12]; // 18 - 29

    // Killer Points
    unsigned char ChaosPoints; // 30 
    unsigned char CurrentKill; // 31 
    unsigned short TotalKill; // 32 - 33

    // Item Info
	WORD Item_Refine[16]; // 34 - 65

	
    // Skill Info
    struct
    {
		BYTE Time;
		BYTE Index;
    } Affect[32]; // 66 - 129

    // Guild Info
	WORD GuildIndex; // 130 - 131

	// Unknow byte 757 
	char GuildMemberType; // 132 - 133

	WORD Unknow;

    // Score Info 
    st_Status Status; // 134 - 181
	 
    // Spawn Type
    struct
    {
		WORD Type;
    } Spawn; // 182 - 185

    char pAnctCode[16];
    char pTab[26];

	BYTE Unknow_02[4];
} p3644;

typedef struct
{
	PacketHeader Header;

	INT32 Value;
} pMsgSignal;

typedef struct 
{
    PacketHeader Header;
    char eCommand[16];
    char eValue[100];
}p334;

typedef struct 
{
	PacketHeader Header;

	INT32 curHP;
	INT32 curMP;
	INT32 maxHP;
	INT32 maxMP;
} p181;

typedef struct
{
	PacketHeader Header; // 0 - 11
	st_Position LastPos;  // 12 - 15

	UINT32 MoveType; // 16 - 19
	UINT32 MoveSpeed;  // 20 - 23

	char Command[24];  // 23 - 47

	st_Position Destiny; // 48 - 50
} p36C;

typedef struct
{
	PacketHeader Header; // 0 - 11

	st_Status Status; // 12 - 59

	BYTE Critical; // 60
	BYTE SaveMana; // 61

	struct
	{
		BYTE Time;
		BYTE Index;
	} Affect[32]; //62 - 125

	WORD GuildIndex; // 126 - 127

	BYTE RegenHP, // 128 
		 RegenMP; // 129

	BYTE Resist1; // 130 
	BYTE Resist2; // 131
	BYTE Resist3; // 132
	BYTE Resist4; // 133

	WORD Unknow; // 134 - 135
	WORD CurrHP; // 136 - 137
	DWORD CurrMP; // 138 - 142
	BYTE Unknow_2; // 143
	BYTE MagicIncrement; // 144 - 147
	DWORD Unknow_148; // 148 - 151
} p366;

typedef struct 
{
	INT32 Index;
	INT32 Damage;
}st_Target;

typedef struct 
{
	PacketHeader Header; // 0 - 11
	INT32 Hold; // 12 - 15
	INT32 reqMP;  // 16 - 19
	INT32 Unk; // 20 - 23
	INT64 currentExp;  // 24 - 27
	short unknow;  // 28 - 31
	st_Position attackerPos;  // 32 - 35
	st_Position targetPos;  // 36 - 39
	short attackerId;  // 40 - 41
	short attackCount;  // 42 - 43
	unsigned char Motion; // 44
	unsigned char skillParm;  // 45
	unsigned char doubleCritical; // 46
	unsigned char FlagLocal;
	short Rsv;

	int currentMp;
	short skillId;  // 48 - 51
	short reqMp;
	st_Target Target[13];  // 56 - 
} p367;

typedef struct
{
	PacketHeader Header;
    short invType;
    short invSlot;
    st_Item itemData;
} p182;

typedef struct 
{
	PacketHeader Header; // 0 - 11
	INT32 Hold; // 12 - 15
	INT32 reqMP;  // 16 - 19
	INT32 Unk; // 20 - 23
	INT64 currentExp;  // 24 - 27
	short unknow;  // 28 - 31
	st_Position attackerPos;  // 32 - 35
	st_Position targetPos;  // 36 - 39
	short attackerId;  // 40 - 41
	short attackCount;  // 42 - 43
	unsigned char Motion; // 44
	unsigned char skillParm;  // 45
	unsigned char doubleCritical; // 46
	unsigned char FlagLocal;
	//char flagLocal;  // 47
	short Rsv;

	int currentMp;
	short skillId;  // 48 - 51
	short reqMp;
	st_Target Target; //44 - 47
	//int   Padding;
} p39D;

typedef struct
{
	PacketHeader Header;

	st_Affect Affect[32];
} p3B9;

typedef struct
{
    PacketHeader Header; // 0 - 11

    char Name[24]; // 12 - 35
    st_Item Item[12]; // 36 - 131

    BYTE Slot[12]; // 132 - 143

    int Gold[12]; // 144 - 191
    short Unknown; // 192 - 193
    short Index; // 194 - 195
} p397;

// Spawn Info
typedef struct 
{
	PacketHeader Header;

    // Spawn Info
    struct
    {
        short X, Y;
    } Current;

    short Index;

    char Name[12];

    // Killer Points
    unsigned char ChaosPoints;
    unsigned char CurrentKill;
    unsigned short TotalKill;

    // Item Info
	WORD Item_Refine[16];

    // Skill Info
    struct
    {
		BYTE Time;
		BYTE Index;
    } Affect[32];

    // Guild Info
	WORD GuildIndex;

	// Unknow byte 757
	char Unknow;

    // Score Info
    st_Status Status;

    // Spawn Type
    struct
    {
        unsigned short Type : 8;
        unsigned short MemberType : 8;
    } Spawn;

    char pAnctCode[16];
    char pTab[26];

	char StoreName[27];
} p363;

typedef struct 
{
	PacketHeader Header;

	DWORD Hold;
	UINT64 Exp;
	DWORD Learn;
	DWORD Learn2;

	WORD pStatus;
	WORD pMaster;
	WORD pSkills;
	BYTE Magic; // Unknow
	BYTE Unk;

	DWORD Gold;
} p337;

typedef struct 
{       
	PacketHeader Header;
	int Hold;
    short killed;
	short killer;
	INT32 Unknow;
	INT64  Exp;
}p338;


typedef struct
{
	PacketHeader Header;
    WORD partyId;
    WORD Level;
    WORD maxHP;
    WORD curHP;
    WORD leaderId;
    char nickName[16];
    BYTE unk;
    WORD targetId;
} p37F;

typedef struct  {
	PacketHeader Header; 
	DWORD SrcType; 
	DWORD SrcSlot; 
	DWORD DstType; 
	DWORD DstSlot; 
	WORD PosX; 
	WORD PosY; 
	int warp; 
}p373;

typedef struct
{
	PacketHeader Header;
    WORD liderID;
    char nickName[16];
} p3AB;

 typedef struct
{
	PacketHeader Header;
	INT16 mobId;
} p37E;
 
 typedef struct
{
	PacketHeader Header;
    WORD LiderID;
    WORD Level;
    WORD maxHP;
    WORD curHP;
    WORD PartyID;
    char nickName[16];
    WORD ID;
} p37D;

typedef struct
{
	PacketHeader Header;

	INT32 Requested;
	BYTE Mac[6];
} p655;

typedef struct
{
	PacketHeader Header;

	INT32 Requested;
	char nick[16];
} p656;
#endif
