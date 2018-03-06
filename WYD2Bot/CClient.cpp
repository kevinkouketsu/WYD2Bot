#include "CClient.h"
#include "Main.h"
#include "CSocket.h"
#include "GetFunc.h"
#include "SendFunc.h"
#include "Basedef.h"

// constructor
CClient::CClient()
{
	SelectedMob = -1;
	Token		= false;
	SecCounter	= 0;
	Session		= 0;
	
	Water.Moving		= 0;
	Water.Room			= 0;
	Water.Status		= 0;
	Water.Type			= 0;

	Macro.Type			= 0;
	Macro.BarIndex		= -1;
	Macro.CurrentId		= -1;
	Macro.SkillId       = -1;
	Macro.OutOfPosition = 0;

	memset(Party, 0, sizeof Party);
	memset(Players, 0, sizeof Players);
	memset(Storage, 0, sizeof st_Item * 127);
	memset(&EnemyList, 0, sizeof EnemyList);
}

CClient::~CClient()
{
}

INT32 CClient::AutoBuff()
{
	static INT32 g_pBuffsId[4][6][2] = 
	{
		{	
			{5, 17},
			{11, 13},
			{3, 14},
			{0, 0},
			{0, 0},
			{0, 0}
		},
		{
			{37, 22},
			{41, 2},
			{43, 11},
			{44, 9},
			{45, 15},
			{46, 18}
		},
		{
			{53, 25},
			{64, 16},
			{66, 16},
			{68, 16},
			{70, 16},
			{71, 16}
		},
		{
			{75, 27},
			{76, 19},
			{77, 21},
			{81, 37},
			{87, 38},
			{0, 0}
		}
	};

	INT32 classId = Mob.ClassInfo;
	for(INT32 i = 0; i < 6; i++)
	{
		INT32 affectId = g_pBuffsId[classId][i][1];
		INT32 skillId  = g_pBuffsId[classId][i][0];

		if(!(Mob.Learn & (1 << (skillId % 24)))) // não possui a skill
			continue;

		if(affectId == 16)
		{
			INT32 t = i - 1;
			if(t != Transform)
				continue;
		}

		INT32 affectSlot = GetAffectSlot(Affect, affectId);
		if(affectSlot != -1) // quer dizer que já está buffado
			continue;
		
		INT32 delay = Macro.Delay[skillId];
		if(delay != 0)
		{
			if(clock() - delay < (SkillData[skillId].Delay) * CLOCKS_PER_SEC)
				continue;
		}

		SendAttack(clientId, skillId);
		ServerMessage("Utilizou o buff %s", ItemList[skillId + 5000].Name);
		return true;
	}

	return false;
}

void CClient::RemoveFromMobGrid(INT32 index)
{
	INT32 mobId = g_pMobGrid[Players[index].Position.Y][Players[index].Position.X];
	if(mobId != index)
	{
		for(INT32 y = 0; y < 4096; y++)
		{
			for(INT32 x = 0; x < 4096; x++)
			{
				if(g_pMobGrid[y][x] == index)
				{
					g_pMobGrid[y][x] = 0;

					break;
				}
			}
		}
	}
	else
		g_pMobGrid[Players[index].Position.Y][Players[index].Position.X] = 0;
}

void CClient::ClearEnemyList()
{
	memset(&EnemyList, 0, sizeof EnemyList);

	// adicionamos a distância padrão como 100 para análise pela próxima função
	for(INT32 i = 0; i < MAX_ENEMY; i++)
		EnemyList[i][1] = 100;
}

INT32 CClient::AddEnemyList(INT32 enemyId, INT32 distance)
{
	for(INT32 i = 0; i < MAX_ENEMY; i++)
	{
		if(EnemyList[i][0] == enemyId)
		{
			EnemyList[i][0] = enemyId;
			EnemyList[i][1] = distance;

			return 1;
		}
	}

	for(INT32 i = 0; i < MAX_ENEMY; i++)
	{
		if(EnemyList[i][0] <= 0)
		{
			EnemyList[i][0] = enemyId;
			EnemyList[i][1] = distance;

			return 1;
		}
	}

	return 0;
}

BOOL CClient::CurrentIsValid()
{
	INT32 current = Macro.CurrentId;
	if(current == -1)
		return false;

	INT32 range = Range;
	if(Macro.SkillId != -1)
		range = SkillData[Macro.SkillId].Range;

	INT32 distance = GetDistance(Players[current].Position.X, Players[current].Position.Y, Position.X, Position.Y);
	if(distance > range)
	{
		if(!Macro.OutOfPosition)
			Macro.CurrentId = -1;

		return false;
	}

	if(Players[current].Status.curHP <= 0)
	{
		Macro.CurrentId = -1;
		return false;
	}

	return true;
}

void CClient::GetNextSkill()
{
	INT32 count = Macro.BarIndex;
	if(count == -1 || count >= 10)
		count = 0;

	INT32 total = 0;
	INT32 skillId = -1;
	do
	{
		total++;
		if(total == 100)
		{
			Macro.SkillId = -1;
			break;
		}

		count ++;
		if(count >= 10)
			count = 0;

		skillId		= Macro.SkillBar[count];
		if(skillId < 0 || skillId >= MAX_SKILLDATA)
		{
			skillId = -1;
			continue;
		}

		INT32 delay = Macro.Delay[skillId];
		if(delay != 0)
		{
			if(clock() - delay < (SkillData[skillId].Delay * CLOCKS_PER_SEC))
			{
				skillId = -1;
				continue;
			}
		}

	} while(skillId < 0 || skillId >= MAX_SKILLDATA);

	Macro.BarIndex = count;
	Macro.SkillId  = skillId;
}

INT32 CClient::SelectEnemy()
{
	INT32 distance = 100;
	INT32 enemyId  = 0;
	for(INT32 i = 0; i < MAX_ENEMY; i++)
	{
		if(EnemyList[i][1] < distance)
		{
			distance = EnemyList[i][1];
			enemyId  = EnemyList[i][0];
		}
	}

	if(distance == 100 || enemyId <= 0 || enemyId >= MAX_SPAWN_MOB)
		return false;

	Macro.CurrentId = enemyId;
	return true;
}

INT32 CClient::GetFirstSlot(INT32 itemId)
{
	for(INT32 i = 0; i < 60; i++)
	{
		if(Mob.Inventory[i].Index == itemId)
			return i;
	}

	return -1;
}

BOOL CClient::UseItem(st_Item *item)
{
	// log
	ServerMessage("Usado o item %s (%d)", ItemList[item->Index].Name, item->Index);

	int index = 0;
	int amount = 0;
	for(int i = 0;i < 3;i++)
	{
		if(item->Effect[i].Index == EF_AMOUNT)
		{
			index = i;
			amount = item->Effect[i].Value;

			break;
		}
	}

	if(amount <= 1)
		memset(item, 0, sizeof st_Item);
	else
		item->Effect[index].Value --;

	return true;
}

BOOL CClient::AutoFeed(INT32 feedPercent)
{
	st_Item *mont = &Mob.Equip[14];
	INT32 baseId = 0;
	if(mont->Index >= 2330 && mont->Index < 2360)
		baseId = 2330;

	if(mont->Index >= 2360 && mont->Index < 2390)
		baseId = 2360;

	INT32 mountId = (mont->Index - baseId) % 30; // local1028
	if (mountId >= 6 && mountId <= 15 || mountId == 27)
		mountId = 6;

	if (mountId == 19)
		mountId = 7;

	if (mountId == 20)
		mountId = 8;

	if (mountId == 21 || mountId == 22 || mountId == 23 || mountId == 28)
		mountId = 9;

	if (mountId == 24 || mountId == 25 || mountId == 26)
		mountId = 10;

	if (mountId == 29)
		mountId = 19;

	INT32 searched = mountId + 2420;
	INT32 slotId = GetFirstSlot(searched);

	if(slotId == -1)
		return false;

	UseItem(&Mob.Inventory[slotId]);
	SendRequestUseItem(INV_TYPE, slotId, 0, 0);
	return true;
}

BOOL CClient::AutoPot(INT32 hpPercent, INT32 mpPercent)
{
	static INT32 pots[2][3] =
	{
		{3322, 404, 3431},
		{3323, 409, 3431}
	};
	
	INT32 minHp = Mob.Status.maxHP * hpPercent / 100;
	INT32 minMp = Mob.Status.maxMP * mpPercent / 100;

	if(Mob.Status.curHP < minHp)
	{
		for(INT32 i = 0; i < 3; i++)
		{
			INT32 slotId = GetFirstSlot(pots[0][i]);
			if(slotId == -1)
				continue;

			UseItem(&Mob.Inventory[slotId]);
			SendRequestUseItem(INV_TYPE, slotId, 0, 0);
			return true;
		}
	}
	
	if(Mob.Status.curMP < minMp)
	{
		for(INT32 i = 0; i < 3; i++)
		{
			INT32 slotId = GetFirstSlot(pots[1][i]);
			if(slotId == -1)
				continue;

			UseItem(&Mob.Inventory[slotId]);
			SendRequestUseItem(INV_TYPE, slotId, 0, 0);
			return true;
		}
	}

	return false;
}

BOOL CClient::ReadSkillBar()
{
	char skillbar[96];
	GetDlgItemText(g_pWindow.hWndMain, IDC_SkillBar, skillbar, 16);
			
	INT32 barra[10];
	memset(barra, -1, sizeof barra);

	INT32 ret = sscanf_s(skillbar, "%d %d %d %d %d %d %d %d %d %d", &barra[0], &barra[1],
		&barra[2], &barra[3], &barra[4], &barra[5], &barra[6], &barra[7], &barra[8], &barra[9]);

	if(ret == 0)
		return false;

	for(INT32 i = 0; i < 10; i++)
		Macro.SkillBar[i] = barra[i];

	return true;
}

BOOL CClient::Magical()
{
	if(!ReadSkillBar())
		return false;

	// Checaremos a próxima skill a ser utilizada
	GetNextSkill();

	if(Macro.SkillId == -1)
		return false;
	
	if(Macro.SkillId == 29)
	{
		st_Target target[13];
		memset(target, 0, sizeof target);

		INT32 maxTarget = SkillData[29].Maxtarget;
		INT32 count		= 1;

		target[0].Index = clientId;

		for(INT32 i = 0; i < 12; i++)
		{
			if(Party[i].ClientID > 0 && Party[i].ClientID < MAX_PLAYER)
			{
				target[count].Index = Party[i].ClientID;

				count++;
			}

			if(count >= maxTarget)
				break;
		}

		SendAttack(target, 29);
		return true;
	}
	
	// checamos se o mob que já estávamos atacando, caso estivesse, ainda é válido
	// se não irá procurar um novo mob para atacar
	if(CurrentIsValid())
	{
		SendAttack(Macro.CurrentId, Macro.SkillId);

		return true;
	}
	
	// limpa a lista de inimigos
	ClearEnemyList();
	
	INT32 range = SkillData[Macro.SkillId].Range;
	// primeiro procuraremos no campo de visão do jogador (range da arma)
	INT32 minX = Client.Position.X - range,
		  minY = Client.Position.Y - range,
		  maxX = Client.Position.X + range,
		  maxY = Client.Position.Y + range;

	for(INT32 posY = minY; posY < maxY; posY++)
	{
		for(INT32 posX = minX; posX < maxX; posX++)
		{
			INT32 mobId = g_pMobGrid[posY][posX];
			if(mobId <= INIT_SPAWN_MOB || mobId >= MAX_SPAWN_MOB)
				continue;

			INT32 distance = GetDistance(posX, posY, Client.Position.X, Client.Position.Y);
			if(distance > range)
				continue;
			
			if(!AddEnemyList(mobId, distance))
				break;
		}
	}

	if(SelectEnemy())
	{
		SendAttack(Macro.CurrentId, Macro.SkillId);
		
		RefreshMacroStatus();
	}
}

BOOL CClient::Physical()
{
	// checamos se o mob que já estávamos atacando, caso estivesse, ainda é válido
	// se não irá procurar um novo mob para atacar
	if(CurrentIsValid())
	{
		SendAttack(Macro.CurrentId, -1);
		
		// TODO : debug
	//	ServerMessage("Atacou o personagem %s %dx %dy (%d)", Players[Macro.CurrentId].Name, Players[Macro.CurrentId].Position.X, Players[Macro.CurrentId].Position.Y,
	//		Macro.CurrentId);
		return true;
	}

	// primeiro procuraremos no campo de visão do jogador (range da arma)
	INT32 minX = Client.Position.X - Range,
		  minY = Client.Position.Y - Range,
		  maxX = Client.Position.X + Range,
		  maxY = Client.Position.Y + Range;

	// limpa a lista de inimigos
	ClearEnemyList();

	for(INT32 posY = minY; posY < maxY; posY++)
	{
		for(INT32 posX = minX; posX < maxX; posX++)
		{
			INT32 mobId = g_pMobGrid[posY][posX];
			if(mobId <= INIT_SPAWN_MOB || mobId >= MAX_SPAWN_MOB)
				continue;

			INT32 distance = GetDistance(posX, posY, Client.Position.X, Client.Position.Y);
			if(distance > Range || !Players[mobId].Status.curHP)
				continue;
			
			if(!AddEnemyList(mobId, distance))
				break;
		}
	}

	if(SelectEnemy())
	{
		SendAttack(Macro.CurrentId, -1);
		
		RefreshMacroStatus();
	//	ServerMessage("Atacou o personagem %s %dx %dy", Players[Macro.CurrentId].Name, Players[Macro.CurrentId].Position.X, Players[Macro.CurrentId].Position.Y);
	}
	return true;
	/*
	else
	{
		Macro.Position.X = Position.X;
		Macro.Position.Y = Position.Y;
		
		minX = Client.Position.X - Range - 5,
		minY = Client.Position.Y - Range - 5,
		maxX = Client.Position.X + Range + 5,
		maxY = Client.Position.Y + Range + 5;
		
		for(INT32 posY = minY; posY < maxY; posY++)
		{
			for(INT32 posX = minX; posX < maxX; posX++)
			{
				INT32 mobId = g_pMobGrid[posY][posX];
				if(mobId <= INIT_SPAWN_MOB || mobId >= MAX_SPAWN_MOB)
					continue;
				
				INT32 distance = GetDistance(posX, posY, Client.Position.X, Client.Position.Y);
				if(!AddEnemyList(mobId, distance))
					break;
			}
		}

		// depois de vasculharmos ao redor, encontramos um player
		if(SelectEnemy())
		{
			node->Path(Client.Position.X, Client.Position.Y, Client.Position.X + 2, Client.Position.Y + 2);
			node->Queue = 1;

			Macro.OutOfPosition = 1;

			ServerMessage("Movendo-se para novo local");
			RefreshMacroStatus();
		}
	}*/
}

void CClient::MacroSystem()
{
	if(Mob.Status.curHP <= 0 || Session != USER_PLAY)
		return;

	if(IsDlgButtonChecked(g_pWindow.hWndMain, IDC_MacrobUFF))
	{
		if(AutoBuff())
			return;
	}
	
	if(GetAttribute(Client.Position.X, Client.Position.Y) & 1)
		return;

	if(IsDlgButtonChecked(g_pWindow.hWndMain, IDC_MacroFisico))
		Physical();
	else if(IsDlgButtonChecked(g_pWindow.hWndMain, IDC_MacroMagico))
		Magical();
	
	INT32 hpPercent = GetDlgItemInt(g_pWindow.hWndMain, IDC_MacroHP, FALSE, TRUE);
	INT32 mpPercent = GetDlgItemInt(g_pWindow.hWndMain, IDC_MacroMP, FALSE, TRUE);
	INT32 feedPercent = GetDlgItemInt(g_pWindow.hWndMain, IDC_MacroRacao, FALSE, TRUE);

	if(hpPercent != 0 || mpPercent != 0)
	{
		if(AutoPot(hpPercent, mpPercent))
			return;
	}

	if(feedPercent != 0)
	{
		if(AutoFeed(feedPercent))
			return;
	}
}

INT32 CClient::DoWater()
{
	// checa se o sistema de macro água está ligado
	if(!Water.Status)
		return false;

	INT32 type = Water.Type; // tipo de água está indo, 0 = N, 1 = M, 2 = A
	INT32 posX = Position.X, // posição X atual do personagem
		  posY = Position.Y; // posiçaõ Y atual do personagem

	// Procura se está dentro de alguma sala
	INT32 t = 0;
	for( ; t < 9; t++)
	{
		if(posX >= waterMaxMin[type][t][0] && posX <= waterMaxMin[type][t][2] && posY >= waterMaxMin[type][t][1] && posY <= waterMaxMin[type][t][3])
			break;
	}

	unsigned char map = GetAttribute(posX, posY);

	// Checa se está na entrada da água
	if(posX > 1961 && posX < 1970 && posY > 1770 && posY < 1778)
	{
		// Se estivermos, usamos o item referente a sala
		INT32 slotId = GetFirstSlot(3173 + Water.Room);
		if(slotId == -1)
		{
			ServerMessage("Não encontrado o item %s", ItemList[3173 + Water.Room].Name);

			// Como não foi encontrado o item da sala atual,
			// iremos tentar o da próxima sala
			Water.Room ++;
			if(Water.Room >= 9)
				Water.Room = 0;

			return false;
		}

		// Caso o item seja encontrado, usamos o item! : ) 
		UseItem(&Mob.Inventory[slotId]);
		SendRequestUseItem(INV_TYPE, slotId, 0, 0);

		// Somamos a sala para ser usado da próxima vez
		Water.Room ++;
	}
	// Caso o personagem esteja dentro de alguma sala da água
	else if(t != 9)
	{
		INT32 totalMobs = 0; 
		
		// Contamos todos os mobs dentro da área da sala para sabermos se a sala já foi finalizada ou não
		for(INT32 i = INIT_SPAWN_MOB; i < MAX_SPAWN_MOB; i++)
		{
			INT32 tmpX = Players[i].Position.X, 
				  tmpY = Players[i].Position.Y;

			if(tmpX >= waterMaxMin[type][t][0] && tmpX <= waterMaxMin[type][t][2] && tmpY >= waterMaxMin[type][t][1] && tmpY <= waterMaxMin[type][t][3] && Players[i].Status.curHP)
				totalMobs++;
		}

		// Checamos se o total de mobs é menor que 0 (quer dizer que a sala foi finalizada)
		// para podermos procurar o próximo item 
		if(totalMobs <= 0)
		{
			INT32 slotId = GetFirstSlot(3173 + Water.Room);
			if(slotId == -1)
			{
				ServerMessage("Não encontrado o item %s", ItemList[3173 + Water.Room].Name);

				// Como não foi encontrado o item da sala atual,
				// iremos tentar o da próxima sala
				Water.Room ++;
				if(Water.Room >= 9)
					Water.Room = 0;

				return false;
			}

			// Caso o item seja encontrado, usamos o item! : ) 
			UseItem(&Mob.Inventory[slotId]);
			SendRequestUseItem(INV_TYPE, slotId, 0, 0);

			// Somamos a sala para ser usado da próxima vez
			Water.Room ++;
		}

	}
	// checa se está na cidade
	else if(map & 1)
	{
		// es estiver na cidade, usa o item Pergaminho dO tELEPORTE
		INT32 slotId = GetFirstSlot(699);
		if(slotId == -1)
		{
			ServerMessage("Não enconttrado o item %s", ItemList[699].Name);

			return false;
		}

		UseItem(&Mob.Inventory[slotId]);
		SendRequestUseItem(INV_TYPE, slotId, 0, 0);
		return true;
	}
	else 
	{
		// CAso não esteja na cidade e nem em alguma sala,
		// especula-se que esteja na entrada da água, por isso,
		// movemos para a área onde o item é usável.

		// TODO : Pode ser checado de uma forma melhor utilizando >> com a coordenada atual
		// podendo ter certeza a respeito da posição atual.
		if(!Water.Moving)
		{
			// faz o cálculo de caminho para a frente da água
			node->Path(posX, posY, 1966, 1774);

			node->Queue = 1;
			Water.Moving = 1;
		}
	}
}