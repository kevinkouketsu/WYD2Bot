#include "Main.h"
#include "GetFunc.h"
#include "Basedef.h"
#include "CClient.h"
#include <math.h>

double GetDistance(int x1,int y1,int x2,int y2)
{
	return sqrt(float(((x2-x1)*(x2-x1))+((y2-y1)*(y2-y1))));
}


short GetItemAbility(st_Item *itemPtr, int eff)
{
    int result = 0;

    int itemID = itemPtr->Index;

    int unique = ItemList[itemID].Unique;
    int pos = ItemList[itemID].Pos;

    if(eff == EF_DAMAGEADD || eff == EF_MAGICADD)
        if(unique < 41 || unique > 50)
            return 0;

    if(eff == EF_CRITICAL)
        if(itemPtr->Effect[1].Index == EF_CRITICAL2 || itemPtr->Effect[2].Index == EF_CRITICAL2)
            eff = EF_CRITICAL2;

    if(eff == EF_DAMAGE && pos == 32)
        if(itemPtr->Effect[1].Index == EF_DAMAGE2 || itemPtr->Effect[2].Index == EF_DAMAGE2)
            eff = EF_DAMAGE2;

    if(eff == EF_MPADD)
        if(itemPtr->Effect[1].Index == EF_MPADD2 || itemPtr->Effect[2].Index == EF_MPADD2)
            eff = EF_MPADD2;

    if(eff == EF_ACADD)
        if(itemPtr->Effect[1].Index == EF_ACADD2 || itemPtr->Effect[2].Index == EF_ACADD2)
            eff = EF_ACADD2;

    if(eff == EF_LEVEL && itemID >= 2330 && itemID < 2360)
        result = (itemPtr->Effect[1].Index - 1);
    else if(eff == EF_LEVEL)
        result += ItemList[itemID].Level;

    if(eff == EF_REQ_STR)
        result += ItemList[itemID].Str;
    if(eff == EF_REQ_INT)
        result += ItemList[itemID].Int;
    if(eff == EF_REQ_DEX)
        result += ItemList[itemID].Dex;
    if(eff == EF_REQ_CON)
        result += ItemList[itemID].Con;

    if(eff == EF_POS)
        result += ItemList[itemID].Pos;

    if(eff != EF_INCUBATE)
    {
        for(int i = 0; i < 12; i++)
        {
            if(ItemList[itemID].Effect[i].Index != eff)
                continue;

            int val = ItemList[itemID].Effect[i].Value;
            if(eff == EF_ATTSPEED && val == 1)
                val = 10;

            result += val;
            break;
        }
    }

	if(itemPtr->Index >= 2330 && itemPtr->Index < 2390)
    {
        if(eff == EF_MOUNTHP)
            return *(WORD*)&itemPtr->Effect[0].Index;

        if(eff == EF_MOUNTSANC)
            return itemPtr->Effect[1].Index;

        if(eff == EF_MOUNTLIFE)
            return itemPtr->Effect[1].Value;

        if(eff == EF_MOUNTFEED)
            return itemPtr->Effect[2].Index;

        if(eff == EF_MOUNTKILL)
            return itemPtr->Effect[2].Value;
		
		if(itemPtr->Index >= 2360 && itemPtr->Index < 2390 && *(WORD*)&itemPtr->Effect[0].Index > 0)
        {
			int mountIndex = itemPtr->Index - 2360;
			if(mountIndex < 0 || mountIndex > 29)
				return 0;

        }
		
        return result;
    }
	
	if(itemPtr->Index >= 3980 && itemPtr->Index <= 3994 && eff == EF_RUNSPEED)
		return 6;
	
    for(int i = 0; i < 3; i++)
    {
        if(itemPtr->Effect[i].Index != eff)
            continue;

        int val = itemPtr->Effect[i].Value;
        if(eff == EF_ATTSPEED && val == 1)
            val = 10;

        result += val;
    }

    if(eff == EF_RESIST1 || eff == EF_RESIST2 ||
       eff == EF_RESIST3 || eff == EF_RESIST4)
    {
        for(int i = 0; i < 12; i++)
        {
            if(ItemList[itemID].Effect[i].Index != EF_RESISTALL)
                continue;

            result += ItemList[itemID].Effect[i].Value;
            break;
        }

        for(int i = 0; i < 3; i++)
        {
            if(itemPtr->Effect[i].Index != EF_RESISTALL)
                continue;

            result += itemPtr->Effect[i].Value;
            break;
        }
    }

    int sanc = GetItemSanc(itemPtr);
	if(itemPtr->Index <= 40)
        sanc = 0;

    if(sanc == 9 && (pos & 0xF00) != 0)
        sanc = 10;

	if(sanc > 15)
		sanc = 15;

    if(sanc != 0 && eff != EF_GRID && eff != EF_CLASS &&
       eff != EF_POS && eff != EF_WTYPE && eff != EF_RANGE &&
       eff != EF_LEVEL && eff != EF_REQ_STR && eff != EF_REQ_INT &&
       eff != EF_REQ_DEX && eff != EF_REQ_CON && eff != EF_VOLATILE &&
       eff != EF_INCUBATE && eff != EF_INCUDELAY && eff != EF_UNKNOW1)
    {
		INT32 value = sanc;
			
		if(value == 10)
			value = 10;
		else if(value == 11)
			value = 12;
		else if(value == 12)
			value = 15;
		else if(value == 13)
			value = 18;
		else if(value == 14)
			value = 23;
		else if(value == 15)
			value = 28;

		result = result * (value + 10) / 10;
    }

    if(eff == EF_RUNSPEED)
    {
        if(result >= 3)
            result = 2;

        if(result > 0 && sanc >= 9)
            result++;
    }

    if(eff == EF_HWORDGUILD || eff == EF_LWORDGUILD)
    {
        int x = result;
        result = x;
    }

    if(eff == EF_GRID)
        if(result < 0 || result > 7)
            result = 0;

    return result;
}

int GetItemSanc(st_Item *item)
{
    int value = 0;

    if(item->Index >= 2360 && item->Index <= 2389)
    {
        //Montarias.
        value = (item->Effect[2].Index / 10);

        if(value > 9)
            value = 9;

        return value;
    }

    if(item->Index >= 2330 && item->Index <= 2359)
    {
        //Crias.
        return 0;
    }

    if(item->Effect[0].Index == 43)
        value = item->Effect[0].Value;
    else if(item->Effect[1].Index == 43)
        value = item->Effect[1].Value;
    else if(item->Effect[2].Index == 43)
        value = item->Effect[2].Value;
	else
	{
		for(INT32 i = 0; i < 3; i++)
		{
			if(item->Effect[i].Index >= 116 && item->Effect[i].Index <= 125)
			{
				value = item->Effect[i].Value;

				break;
			}
		}
	}

    if(value >= 230)
    {
        value = 10 + ((value - 230) / 4);
        if(value > 15)
            value = 15;
    }
    else
        value %= 10;

    return value;
}

int GetMaxAbility(st_Mob *usr, int eff)
{
    int MaxAbility = 0;

    for(int i = 0; i < 16; i++)
    {
		if(usr->Equip[i].Index == 0)
            continue;

        short ItemAbility = GetItemAbility(&usr->Equip[i], eff);
        if(MaxAbility < ItemAbility)
            MaxAbility = ItemAbility;
    }

    return MaxAbility;
}

int GetMobAbility(st_Mob *usr, int eff)
{
    int LOCAL_1 = 0;

    if(eff == EF_RANGE)
    {
        LOCAL_1 = GetMaxAbility(usr, eff);

		int LOCAL_2 = (usr->Equip[0].Index / 10);
        if(LOCAL_1 < 2 && LOCAL_2 == 3)
            if((usr->Learn & 0x40) != 0)
                LOCAL_1 = 2;

        return LOCAL_1;
    }

    int LOCAL_18[16];
    for(int LOCAL_19 = 0; LOCAL_19 < 16; LOCAL_19++)
    {
        LOCAL_18[LOCAL_19] = 0;

		int LOCAL_20 = usr->Equip[LOCAL_19].Index;
        if(LOCAL_20 == 0 && LOCAL_19 != 7)
            continue;

        if(LOCAL_19 >= 1 && LOCAL_19 <= 5)
            LOCAL_18[LOCAL_19] = ItemList[LOCAL_20].Unique;

        if(eff == EF_DAMAGE && LOCAL_19 == 6)
            continue;

        if(eff == EF_MAGIC && LOCAL_19 == 7)
            continue;

        if(LOCAL_19 == 7 && eff == EF_DAMAGE)
        {
            int dam1 = (GetItemAbility(&usr->Equip[6], EF_DAMAGE) +
                        GetItemAbility(&usr->Equip[6], EF_DAMAGE2));
            int dam2 = (GetItemAbility(&usr->Equip[7], EF_DAMAGE) +
                        GetItemAbility(&usr->Equip[7], EF_DAMAGE2));

			int arm1 = usr->Equip[6].Index;
            int arm2 = usr->Equip[7].Index;

            int unique1 = 0;
            if(arm1 > 0 && arm1 < MAX_ITEMLIST)
                unique1 = ItemList[arm1].Unique;

            int unique2 = 0;
            if(arm2 > 0 && arm2 < MAX_ITEMLIST)
                unique2 = ItemList[arm2].Unique;

            if(unique1 != 0 && unique2 != 0)
            {
                int porc = 0;
                if(unique1 == unique2)
                    porc = 70;
                else
                    porc = 50;

                if(dam1 > dam2)
                    LOCAL_1 = ((LOCAL_1 + dam1) + ((dam2 * porc) / 100));
                else
                    LOCAL_1 = ((LOCAL_1 + dam2) + ((dam1 * porc) / 100));

                continue;
            }

            if(dam1 > dam2)
                LOCAL_1 += dam1;
            else
                LOCAL_1 += dam2;

            continue;
        }

        int LOCAL_28 = GetItemAbility(&usr->Equip[LOCAL_19], eff);
        if(eff == EF_ATTSPEED && LOCAL_28 == 1)
            LOCAL_28 = 10;

        LOCAL_1 += LOCAL_28;
    }

    if(eff == EF_AC && LOCAL_18[1] != 0)
        if(LOCAL_18[1] == LOCAL_18[2] && LOCAL_18[2] == LOCAL_18[3] &&
           LOCAL_18[3] == LOCAL_18[4] && LOCAL_18[4] == LOCAL_18[5])
            LOCAL_1 = ((LOCAL_1 * 105) / 100);

    return LOCAL_1;
}

INT32 GetManaSpent(INT32 skillId, INT32 saveMana, INT32 mastery)
{
	INT32 manaSpent = SkillData[skillId].Mana; // -4

	manaSpent = (((mastery >> 1) + 100) * manaSpent) / 100;
	manaSpent = ((100 - saveMana) * manaSpent) / 100;

	return manaSpent;
}

st_Item *GetItemPointer(INT32 invType, INT32 invSlot)
{
	if(invType < 0 || invType >= 3 || invSlot < 0 || invSlot >= 128)
		return NULL;

	if(invType == EQUIP_TYPE) // equip
	{
		if(invSlot >= 16)
			return NULL;

		return &Client.Mob.Equip[invSlot];
	}
	else if(invType == INV_TYPE) // inv
	{
		if(invSlot >= 64)
			return NULL;

		return &Client.Mob.Inventory[invSlot];
	}
	else if(invType == STORAGE_TYPE)
	{
		if(invSlot >= 128)
			return NULL;

		return &Client.Storage[invSlot];
	}

	return NULL;
}

INT32 GetAffectSlot(st_Affect *affect, INT32 buffId)
{
	for(INT32 i = 0; i < MAX_AFFECT; i++)
	{
		if(affect[i].Index == buffId)
			return i;
	}

	return -1;
}

unsigned char GetAttribute(int posX, int posY)
{
	if (posX < 0 || posX > 4096 || posY < 0 || posY > 4096)
		return g_pAttributeMap[0][0];

	int gridX = (posX >> 2) & 0x3FF;
	int gridY = (posY >> 2) & 0x3FF;

	return g_pAttributeMap[gridY][gridX];
}

double GetPercentLevelExp(INT32 level, INT32 ev, INT64 exp)
{
	if(level < 0 || level >= 400 || ev < 0 || ev >= 5)
		return 100.0f;

	INT64 exp1 = g_pNextLevel[ev][level - 1];
	INT64 exp2 = g_pNextLevel[ev][level];

	double dif = exp2 - exp1;
	if(dif <= 0)
		return 100.0f;

	double dif_exp = exp - exp1;
	
	double result = (dif_exp / dif) * 100;
	return result;
}