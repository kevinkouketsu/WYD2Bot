#include "Main.h"
#include "Basedef.h"
#include "CClient.h"
#include "GetFunc.h"

sSpellData SkillData[MAX_SKILLDATA];
char g_pHeightGrid[4096][4096];
unsigned char g_pAttributeMap[1024][1024];
WORD g_pMobGrid[4096][4096];
char EffectName[256][16];
sItemData ItemList[MAX_ITEMLIST];

bool ReadHeightMap()
{
	int Handle = _open("./heightmap.dat", _O_RDONLY | _O_BINARY, NULL);

	if (Handle == -1)
		return false;

	_read(Handle, (void*)g_pHeightGrid, sizeof(g_pHeightGrid));
	_close(Handle);
	return true;
}

bool ReadAttributeMap()
{
    FILE *pFile = NULL; 
	
	fopen_s(&pFile, "AttributeMap.dat", "rb");

    if(!pFile)
		return false;

    fread(g_pAttributeMap, 1024, 1024, pFile);
    fclose(pFile);

    return true;
}

void ApplyAttribute(char *pHeight, int size)
{
	int endx = g_HeightPosX + size;
	int endy = g_HeightPosY + size;

	int xx = 0;
	int yy = 0;

	for(int y = g_HeightPosY; y < endy; y++)
	{
		for(int x = g_HeightPosX; x < endx; x++)
		{
			xx = (x >> 2) & 0x3FF;
			yy = (y >> 2) & 0x3FF;

			unsigned char att = g_pAttributeMap[yy][xx];

			if (att & 2)
				pHeight[x + g_HeightWidth * (y - g_HeightPosY) - g_HeightPosX] = 127;
		}
	}
}

// function with anwers of quiz system
BOOL ReadQuiz()
{
	FILE *pFile = NULL;
	fopen_s(&pFile, "Quiz.txt", "r");

	if (pFile)
	{
		char tmp[256];

		INT32 count = -1;
		while (fgets(tmp, sizeof tmp, pFile))
		{
			if (tmp[0] == '\n')
				continue;

			if (tmp[0] == '#')
			{
				count++;
				continue;
			}

			char cmd[32],
				question[128],
				answer[32];

			INT32 index = -1;

			INT32 ret = sscanf(tmp, "%[^=]=%[^\n]", cmd, question);
			if (!strncmp(cmd, "Question", 8))
				strncpy(Client.Quiz[count].Question, question, 96);
			else if (!strncmp(cmd, "Answer", 6))
			{
				sscanf_s(tmp, "Answer%d", &index);
				if (index < 0 || index >= 4)
					continue;

				strncpy(Client.Quiz[count].Answer[index], question, 32);
			}
		}

		Client.TotalQuiz = count + 1;
		fclose(pFile);
		return true;
	}

	return false;
}

int Rand()
{
	static long long i = 1;

	i = ((i * 214013) + 253101111);
	return ((i >> 16) & 0x7FFF);
}

void EncryptVersion(UINT32 *pVersion)
{
	INT32 random = (Rand() % 7) + 1;

	INT32 randIterator = Rand() & 0x80000003;
	if (randIterator < 0)
	{
		randIterator--;
		randIterator |= 0xFFFFFFFC;
		randIterator++;
	}

	*pVersion = ((*pVersion << (byte)(random + 5)) | (random << 2)) | randIterator;
}

void AddChat(char *msg, ...)
{
	/* Arglist */
	char buffer[256];
	va_list arglist;
	va_start(arglist, msg);
	vsprintf(buffer, msg, arglist);
	va_end(arglist);
	/* Fim arlist */
	
	time_t rawnow = time(NULL);
	struct tm *now = localtime(&rawnow);

	char tmp[256];
	sprintf(tmp, "%02d:%02d:%02d - %s", now->tm_hour, now->tm_min, now->tm_sec, buffer);
	INT32 ret = SendDlgItemMessage(g_pWindow.hWndMain, IDC_ChatList, LB_INSERTSTRING, -1, (LPARAM)tmp);
	
	SendMessage(GetDlgItem(g_pWindow.hWndMain, IDC_ChatList), LB_SETCURSEL , ret, 0);
}

void AddGroup(char *msg, ...)
{
	/* Arglist */
	char buffer[256];
	va_list arglist;
	va_start(arglist, msg);
	vsprintf(buffer, msg, arglist);
	va_end(arglist);
	/* Fim arlist */
	
	INT32 ret = SendDlgItemMessage(g_pWindow.hWndMain, IDC_ListGroup, LB_INSERTSTRING, -1, (LPARAM)buffer);
	SendMessage(GetDlgItem(g_pWindow.hWndMain, IDC_ListGroup), LB_SETCURSEL , ret, 0);
}

void ServerMessage(char *msg, ...)
{
	/* Arglist */
	char buffer[256];
	va_list arglist;
	va_start(arglist, msg);
	vsprintf(buffer, msg, arglist);
	va_end(arglist);
	/* Fim arlist */
	
	time_t rawnow = time(NULL);
	struct tm *now = localtime(&rawnow);

	char tmp[256];
	sprintf(tmp, "%02d:%02d:%02d - %s", now->tm_hour, now->tm_min, now->tm_sec, buffer);
	INT32 ret = SendDlgItemMessage(g_pWindow.hWndMain, IDC_MessageList, LB_INSERTSTRING, -1, (LPARAM)tmp);
	SendMessage(GetDlgItem(g_pWindow.hWndMain, IDC_MessageList), LB_SETCURSEL , ret, 0);
}

void SetLabel(INT32 id, char *msg, ...)
{
	/* Arglist */
	char buffer[256];
	va_list arglist;
	va_start(arglist, msg);
	vsprintf(buffer, msg, arglist);
	va_end(arglist);
	/* Fim arlist */

	HWND hWnd = GetDlgItem(g_pWindow.hWndMain, id);
	if(hWnd)
		SetWindowText(hWnd, buffer);
}

void CharLogOut()
{
	SendMessage(GetDlgItem(g_pWindow.hWndMain, IDC_MessageList), LB_RESETCONTENT, 0, 0);
	SendMessage(GetDlgItem(g_pWindow.hWndMain, IDC_ChatList), LB_RESETCONTENT, 0, 0);
	SendMessage(GetDlgItem(g_pWindow.hWndMain, IDC_Mobs), LB_RESETCONTENT, 0, 0);
	
	SetLabel(IDC_Nickname, "Nome:");
	SetLabel(IDC_Level,	"Level:");
	SetLabel(IDC_Position, "%dx %dy", 0, 0);
	SetLabel(IDC_HP, "HP: 0/0");
	SetLabel(IDC_MP, "MP: 0/0");
}

void RefreshStatus()
{
	SetLabel(IDC_Nickname, "Nome: %s", Client.Mob.Name);
	SetLabel(IDC_Level,	"Level: %d", Client.Mob.Status.Level + 1);
	SetLabel(IDC_Position, "%dx %dy", Client.Position.X, Client.Position.Y);
	SetLabel(IDC_HP, "HP: %d/%d", Client.Mob.Status.curHP, Client.Mob.Status.maxHP);
	SetLabel(IDC_MP, "MP: %d/%d", Client.Mob.Status.curMP, Client.Mob.Status.maxMP);
	SetLabel(IDC_Gold, "Gold: %d", Client.Mob.Gold);
	SetLabel(IDC_Exp, "EXP: %.2f%%", GetPercentLevelExp(Client.Mob.Status.Level, Client.Mob.Equip[0].EFV2, Client.Mob.Exp));
}

void RefreshGroup()
{
	SendMessage(GetDlgItem(g_pWindow.hWndMain, IDC_ListGroup), LB_RESETCONTENT, 0, 0);

	for(INT32 i = 0; i < 12; i++)
	{
		INT32 mobId = Client.Party[i].ClientID;
		if(mobId <= 0)
			continue;

		// check group leader 
		char tmp[256];
		if(Client.Party[i].ClientID == Client.LeaderId)
			sprintf(tmp, "%s (%d) [L]", Client.Party[i].nickName, Client.Party[i].ClientID);
		else
			sprintf(tmp, "%s (%d)", Client.Party[i].nickName, Client.Party[i].ClientID);

		// get if player have experience box
		INT32 box = GetAffectSlot(Client.Players[mobId].Affect, 39);
		if(box != -1)
			strcat(tmp, " [B]");

		// get if player have chicken
		INT32 chicken = GetAffectSlot(Client.Players[mobId].Affect, 30);
		if(chicken != -1)
			strcat(tmp, " [F]");

		AddGroup(tmp);
	}
}

void RefreshMacroStatus()
{
	INT32 id = Client.Macro.CurrentId;
	if(id == -1)
		return;

	SetLabel(IDC_MacroName, "Nome: %s", Client.Players[id].Name);
	SetLabel(IDC_MacroPosition, "Posição: %dx %dy", Client.Players[id].Position.X, Client.Players[id].Position.Y);
	SetLabel(IDC_MacroMobHP, "HP: %d/%d", Client.Players[id].Status.curHP, Client.Players[id].Status.maxHP);
}

void RefreshMobStatus()
{
	if(Client.SelectedMob == -1)
		return;

	INT32 index = Client.SelectedMob;
	SetLabel(IDC_MobName, "Nome: %s", Client.Players[index].Name);
	SetLabel(IDC_MobPosition, "Posição: %dx %dy", Client.Players[index].Position.X, Client.Players[index].Position.Y);
	SetLabel(IDC_MobHP, "HP: %d/%d", Client.Players[index].Status.curHP, Client.Players[index].Status.maxHP);
}


short get_effect_index(const char *s)
{
    int i;
    for(i = 0; i < 256; i++)
        if(!strcmp(EffectName[i], s))
            return i;

    return atoi(s);
}

bool ReadItemEffect()
{
    FILE *fp = fopen("ItemEffect.h", "rt");
    if(fp == NULL)
    {
        memset(EffectName, 0, sizeof(EffectName));
        return false;
    }

    int ret, index;
    const char *cmm = "#define";

    char line[1024];
    char val[64];

    while(fgets(line, sizeof(line), fp))
    {
        if(strncmp(line, cmm, strlen(cmm)) == 0)
        {
            index = -1;
            *val = '\0';
            ret = sscanf(line, "#define %15s %d", val, &index);
            if(ret != 2)
                continue;

            if(index >= 0 && index < 256)
                strcpy(EffectName[index], val);
        }
    }

    fclose(fp);
    return true;
}

bool ReadItemList()
{
    FILE *fp = NULL;
	fopen_s(&fp, "ItemList.csv", "rt");

    if(fp == NULL)
    {
        memset(ItemList, 0, sizeof(ItemList));
        return false;
    }

	if(!ReadItemEffect())
	{
		if(fp)
			fclose(fp);

		return false;
	}

    int itemID, ret;
    sItemData item;

    char line[1024];

    memset(ItemList, 0, sizeof(ItemList));
    while(fgets(line, sizeof(line), fp))
    {
        char meshBuf[MAX_MESH_BUFFER];
		char scoreBuf[MAX_SCORE_BUFFER] = { 0, };
        char effBuf[MAX_EFFECT][MAX_EFFECT_NAME];

        if(*line == '\n' || *line == '#')
            continue;

        memset(effBuf, 0, sizeof(effBuf));
        memset(meshBuf, 0, sizeof(meshBuf));
        memset(scoreBuf, 0, sizeof(scoreBuf));
        memset(&item, 0, sizeof(sItemData));

        char *p = line;
        while(*p != '\0')
        {
            if(*p == ',')
                *p = ' ';
            p++;
        }

        ret = sscanf(line, "%d %63s %15s %31s %hd %d %hd %hd %hd %15s %hd %15s %hd %15s %hd %15s %hd %15s %hd %15s %hd %15s %hd %15s %hd %15s %hd %15s %hd %15s %hd %15s %hd",
                     &itemID, item.Name, meshBuf, scoreBuf, &item.Unique, &item.Price, &item.Pos, &item.Extreme, &item.Grade,
                     effBuf[ 0], &item.Effect[ 0].Value, effBuf[ 1], &item.Effect[ 1].Value, effBuf[ 2], &item.Effect[ 2].Value,
                     effBuf[ 3], &item.Effect[ 3].Value, effBuf[ 4], &item.Effect[ 4].Value, effBuf[ 5], &item.Effect[ 5].Value,
                     effBuf[ 6], &item.Effect[ 6].Value, effBuf[ 7], &item.Effect[ 7].Value, effBuf[ 8], &item.Effect[ 8].Value,
                     effBuf[ 9], &item.Effect[ 9].Value, effBuf[10], &item.Effect[10].Value, effBuf[11], &item.Effect[11].Value);

        if(ret < 9 || itemID <= 0 || itemID >= MAX_ITEMLIST)
            continue;

        sscanf(meshBuf, "%hd.%d", &item.Mesh1, &item.Mesh2);
        sscanf(scoreBuf, "%hd.%hd.%hd.%hd.%hd", &item.Level, &item.Str, &item.Int, &item.Dex, &item.Con);

        int i;
        for(i = 0; i < MAX_EFFECT; i++)
            item.Effect[i].Index = get_effect_index(effBuf[i]);

        memcpy(&ItemList[itemID], &item, sizeof(sItemData));
    }

    fclose(fp);
    return true;
}

bool ReadSkillData()
{
	FILE *fp = fopen("SkillData.csv", "rt");
	if(fp == NULL)
	{
		memset(SkillData, 0, sizeof(SkillData));
		return false;
	}

	char line[1024];
	while(fgets(line, sizeof(line), fp))
	{
		int index;
		sSpellData spell;

		if(*line == '\n' || *line == '#')
			continue;

		index = -1;
		memset(&spell, 0, sizeof(spell));

		char *p = line;
		while(*p != '\0')
		{
			if(*p == ',')
				*p = ' ';
			p++;
		}

		int ret = sscanf(line, "%d %d %d %d %d %d %d %d %d %d %d %d %d %*s %*s %d %d %d %d %d %d %d %d",
				&index, &spell.Points, &spell.Target, &spell.Mana, &spell.Delay, &spell.Range,
				&spell.InstanceType, &spell.InstanceValue, &spell.TickType, &spell.TickValue,
				&spell.AffectType, &spell.AffectValue, &spell.Time, &spell.InstanceAttribute,
				&spell.TickAttribute, &spell.Aggressive, &spell.Maxtarget, &spell.PartyCheck,
				&spell.AffectResist, &spell.Passive_Check, &spell.ForceDamage);

		if(ret < 19 || index < 0 || index >= MAX_SKILLDATA)
			continue;

		memcpy(&SkillData[index], &spell, sizeof(sSpellData));
	}

	fclose(fp);
	return true;
}

BOOL ReadAutoTrade()
{
	FILE *pFile = NULL;
	fopen_s(&pFile, "AutoTrade.txt", "r");

	memset(&Client.AutoTrade, 0, sizeof p397);

	if(pFile)
	{
		INT32 i = 0;
		char line[1024];
		while(fgets(line, sizeof(line), pFile))
		{
			if(*line == '\n' || *line == '#')
				continue;

			INT32 slotId = 0,
				  price  = 0;

			char cmd[32],
				 val[32];

			char name[24];
			INT32 ret = sscanf(line, "%[^=]=%[^=]", cmd, val);
			if(ret != 2)
				continue;

			if(!strcmp(cmd, "Name"))
				strncpy(Client.AutoTrade.Name, val, 24);
			else if(!strcmp(cmd, "Item"))
			{
				ret = sscanf(val, "%d %d", &slotId, &price);
				if(ret != 2)
					continue;

				Client.AutoTrade.Gold[i] = price;
				Client.AutoTrade.Slot[i] = slotId;
				i++;
			}
		}


		fclose(pFile);
		return true;
	}

	return false;
}