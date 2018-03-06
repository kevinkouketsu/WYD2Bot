#include "Main.h"
#include "Basedef.h"
#include "CClient.h"
#include "SendFunc.h"
#include "CSocket.h"
#include "GetFunc.h"

#pragma comment(lib, "Rpcrt4.lib")

INT32 CClient::PacketControl(BYTE *pBuffer, INT32 size)
{
	PacketHeader *Header = (PacketHeader*)(pBuffer);
	
	cServer.LastPacket = GetCurrentTime();
	cServer.TimePacket = Header->TimeStamp;

	switch(Header->PacketId)
	{
	case 0x101:
		{
			p101 *packet = (p101*)(Header);
			
			ServerMessage(packet->Message);
		}
		break;
	case 0xFDE:
		{
			ServerMessage("Senha numérica correta! Aguardando para entrar no jogo!");

			Token = true;
		}
		break;
	case 0xFDF:
		{
			ServerMessage("Senha numérica incorreta!");

			Token = false;
		}
		break;
	case 0x10A:
		{
			p10A *packet = (p10A*)(Header);
			memcpy(&Client.charList, packet, sizeof p10A);

			memcpy(&Client.Storage, &packet->Storage, sizeof st_Item * 128);
			for(INT32 i = 0; i < 4;i ++)
			{
				if(!packet->CharList.Name[i][0])
					continue;

				char tmp[32];
				sprintf(tmp, "%s (%d)", packet->CharList.Name[i], i);
				SendDlgItemMessage(g_pWindow.hWndMain, IDC_Charlist, LB_INSERTSTRING, -1, (LPARAM)tmp);
			}

			ServerMessage("Entrou na seleção de personagem!");

			Session = USER_SELCHAR;

			char number[6];
			GetDlgItemText(g_pWindow.hWndMain, IDC_Token, number, 6);

			if(number[0])
			{
				SendToken(number);

				ServerMessage("Senha numérica %s enviada", number);
			}
		}
		break;
	case 0x336:
		{
			p336 *packet = (p336*)(Header);

			if(packet->Header.ClientId == clientId)
			{
				memcpy(&Mob.Status, &packet->Status, sizeof st_Status);

				Mob.SaveMana = packet->SaveMana;
				Mob.Critical = packet->Critical;

				Mob.Status.curHP = packet->CurrHP;
				Mob.Status.curMP = packet->CurrMP;

				Mob.GuildIndex   = packet->GuildIndex;

				Mob.RegenHP      = packet->RegenHP;
				Mob.RegenMP      = packet->RegenMP;
			}
			else
			{
				INT32 id = packet->Header.ClientId;
				Players[id].GuildIndex = packet->GuildIndex;
				
				memcpy(&Players[id].Status, &packet->Status, sizeof st_Status);
			}
		}
		break;
	case 0x3B9:
		{
			p3B9 *packet = (p3B9*)(Header);
			
			INT32 index = packet->Header.ClientId;
			if(index == clientId)
				memcpy(&Affect, &packet->Affect, sizeof st_Affect * 32);
			//else
			//	memcpy(&Players[index].Affect, &packet->Affect, sizeof st_Affect * 32);
		}
		break;
	case 0x378:
		{
			p378 *packet = (p378*)(Header);

			if(packet->Header.ClientId == clientId)
			{
				memcpy(SkillBar, &packet->SkillBar1, 4);
				memcpy(&SkillBar[4], &packet->SkillBar2, 16);
			}
		}
		break;
	case 0x114:
		{
			p114 *packet = (p114*)(Header);

			Position.X	= packet->WorldPos.X;
			Position.Y	= packet->WorldPos.Y;
			charId		= packet->Other.SlotIndex;
			clientId	= packet->Other.ClientIndex;

			memcpy(SkillBar, &packet->Mob.SkillBar1, 4);
			memcpy(&SkillBar[4], &packet->Other.SkillBar2, 16);

			memcpy(&Mob, &packet->Mob, sizeof st_Mob);

			INT32 range = GetMobAbility(&Mob, EF_RANGE);
			if(Mob.Learn & 0x20000000)
				range ++;

			if(Mob.Learn & 0x80000)
				range ++;

			Range = range + 1;
			ServerMessage("Logado no personagem %s em %dx %dy", Mob.Name, Position.X, Position.Y);
			
			Session = USER_PLAY;

			RefreshStatus();
		}
		break;
	case 0x333:
		{
			p333 *packet = (p333*)(Header);

			INT32 index = packet->Header.ClientId;

			if(Players[index].Name[0])
				AddChat("[%s]> %s", Players[index].Name, packet->eChat);
		}
		break;
	case 0x165:
		{
			pMsgSignal *packet = (pMsgSignal*)(Header);
			
			INT32 index = packet->Header.ClientId;	
			
			char tmp[256];
			sprintf(tmp, "%s (%d)", Players[index].Name, index);

			HWND hwndList = GetDlgItem(g_pWindow.hWndMain, IDC_Mobs); 
			INT32 ret = SendMessage(hwndList, LB_FINDSTRING, 0, (LPARAM)(&tmp));
			SendMessage(hwndList, LB_DELETESTRING, ret, 0); 

			if(packet->Value == 0)
			{
				RemoveFromMobGrid(index);

				Players[index].Name[0] = '\0';

				Players[index].Position.X = 0;
				Players[index].Position.Y = 0;
				memset(&Players[index].Status, 0, sizeof st_Status);

				if(index == SelectedMob)
					SelectedMob = -1;		

				else if(index == Macro.CurrentId)
					Macro.CurrentId = -1;
			}
		}
		break;
	case 0x334:
		{
			p334 *packet = (p334*)(Header);

			if(packet->eCommand[0] && packet->eValue[0] == '@')
				AddChat("%s> %s", packet->eCommand, packet->eValue);
			else
				AddChat("[PM] %s> %s", packet->eCommand, packet->eValue);
		}
		break;
	case 0x337:
		{
			p337 *p = (p337*)(Header);

			if(p->Header.ClientId == clientId)
			{
				Mob.Gold  = p->Gold;
				Mob.Learn = p->Learn;
				Mob.Exp   = p->Exp;

				Mob.MasterPoint = p->pMaster;
				Mob.SkillPoint = p->pSkills;
				Mob.StatusPoint = p->pStatus;

				RefreshStatus();
			}
		}
		break;
	case 0x363:
	case 0x364:
		{
			p3644 *packet = (p3644*)(Header);

			INT32 index = packet->Index;
			if(index < 0 || index >= 30000)
				break;

			memcpy(&Players[index].Status, &packet->Status, sizeof st_Status);
			strncpy(Players[index].Name, packet->Name, 16);

			Players[index].Position.X = packet->Current.X;
			Players[index].Position.Y = packet->Current.Y;
			Players[index].GuildIndex = packet->GuildIndex;
			
			HWND hwndList = GetDlgItem(g_pWindow.hWndMain, IDC_Mobs); 

			for(INT32 i = 0; i < 16;i++)
			{
				if(Players[index].Name[i] == ' ')
					Players[index].Name[i] = '_';
			}

			for(INT32 i = 0; i < 32; i++)
			{
				Players[index].Affect[i].Index = packet->Affect[i].Index;
				Players[index].Affect[i].Time = packet->Affect[i].Time;
			}

			char tmp[256];
			sprintf(tmp, "%s (%d)", Players[index].Name, packet->Index);

			INT32 ret = SendMessage(hwndList, LB_FINDSTRING, 0, (LPARAM)(&tmp));
			if(ret == LB_ERR)
				SendDlgItemMessage(g_pWindow.hWndMain, IDC_Mobs, LB_INSERTSTRING, -1, (LPARAM)&tmp);

			WORD multHp = packet->Item_Refine[13] & 0x0FFF;
			WORD ref    = packet->Item_Refine[13] >> 12;
			if(multHp == 786 || multHp == 1936 || multHp == 1937)
			{
				if(ref < 2)
					ref = 2;

				if(multHp == 786)
					multHp = 1 * ref;
				else if(multHp == 1936)
					multHp = 10 * ref;
				else if(multHp == 1937)
					multHp = 1000 * ref;

				Players[index].Status.maxHP *= ref;
				if(Players[index].Status.maxHP > 2000000000)
					Players[index].Status.maxHP = 2000000000;

				Players[index].Status.curHP *= ref;
				if(Players[index].Status.curHP > 2000000000)
					Players[index].Status.curHP = 2000000000;
			}

			g_pMobGrid[packet->Current.Y][packet->Current.X] = packet->Index;

			if(packet->Index == clientId && Header->PacketId == 0x363)
				ServerMessage("Loja criada com sucesso!");
		}
		break;
	case 0x367:
	case 0x39D:
	case 0x39E:
		{
			p367 *packet = (p367*)(Header);

			INT32 targetTotal = 13;
			
			if(packet->Header.Size == sizeof p367)
				targetTotal = 13;
			else if(packet->Header.Size == sizeof p39D)
				targetTotal = 1;
			else if(packet->Header.Size == sizeof p39D + sizeof st_Target)
				targetTotal = 2;
	
			for(INT32 i = 0; i < targetTotal; i++)
			{
				INT32 mobId = packet->Target[i].Index;
				if(mobId <= 0  || mobId >= MAX_SPAWN_MOB)
					continue;

				if(mobId == clientId)
				{
					Mob.Status.curHP -= packet->Target[i].Damage;
					if(Mob.Status.curHP <= 0)
						Mob.Status.curHP = 0;

					RefreshStatus();
				}
				else
				{
					Players[mobId].Status.curHP -= packet->Target[i].Damage;
					if(Players[mobId].Status.curHP <= 0)
						Players[mobId].Status.curHP = 0;

					if(mobId == Macro.CurrentId)
						RefreshMacroStatus();
					else if(mobId == Client.SelectedMob)
						RefreshMobStatus();
				}
			}
			
			if(packet->attackerId == clientId)
				Mob.Status.curMP = packet->currentMp;
		}
		break;
	case 0x1C6:
	{
		p1C6 *p = (p1C6*)(Header);
		for (INT32 i = 0; i < TotalQuiz; i++)
		{
			if (!strcmp(p->Title, Quiz[i].Question))
			{ // achou a pergunta
				for (INT32 t = 0; t < 4; t++)
				{
					if (!strncmp(p->Ans[t], Quiz[i].Answer[0], 32))
					{
						pMsgSignal packet; 
						memset(&packet, 0, sizeof packet);

						packet.Header.PacketId = 0x2C7;
						packet.Header.Size = sizeof packet;
						packet.Header.ClientId = clientId;

						packet.Value = t;

						cServer.AddMessage((char*)&packet, sizeof packet);

						ServerMessage("Pergunta: %s", p->Title);
						ServerMessage("Respondido: %s", p->Ans[t]);
						break;
					}
				}
			}
		}
	}
	break;
	case 0x182:
		{
			p182 *packet = (p182*)(Header);

			st_Item *item = GetItemPointer(packet->invType, packet->invSlot);
			if(item == NULL)
				return true;

			memcpy(item, &packet->itemData, sizeof st_Item);

			if((item->Index >= 3173 && item->Index <= 3190) || (item->Index >= 777 && item->Index <= 785))
				break;

			if(item->Index <= 0 ||item->Index >= MAX_ITEMLIST)
				ServerMessage("Excluído o item %s do invtype %d slot %d", ItemList[item->Index].Name, packet->invType, packet->invSlot);
			else if(packet->invType != EQUIP_TYPE && packet->invSlot != 13)
				ServerMessage("Atualizado o equiptype %d slot %d para item %s [%d %d %d %d %d %d]",
					packet->invType, packet->invSlot, ItemList[packet->itemData.Index].Name, item->EF1, item->EFV1, 
					item->EF2, item->EFV2, item->EF3, item->EFV3);
		}
		break;
	case 0x116:
		{
			ServerMessage("Enviado para a seleção de personagem!");

			Session = USER_SELCHAR;
			CharLogOut();
		}
		break;
	case 0x338:
		{
			p338 *packet = (p338*)(Header);

			if(packet->killed == clientId)
			{
				Mob.Status.curHP = 0;

				ServerMessage("Você morreu...");
			}
			else if(packet->killed == Macro.CurrentId && Macro.OutOfPosition)
			{
				ServerMessage("Voltando para o local de origem!");

				Macro.OutOfPosition = 0;

				//volta para o local em que estava
				node->Path(Client.Position.X, Client.Position.Y, Macro.Position.X, Macro.Position.Y);
				node->Queue = 1;
			}
			else if(packet->killed == Macro.CurrentId)
				Macro.CurrentId = -1;
			
			Players[packet->killed].Position.X		= 0;
			Players[packet->killed].Position.Y		= 0;
			Players[packet->killed].Status.curHP	= 0;

			INT64 _exp = Mob.Exp;

			if(IsDlgButtonChecked(g_pWindow.hWndMain, IDC_ShowExp))
				ServerMessage("Você ganhou %I64d experiência", packet->Exp - _exp);

			Mob.Exp = packet->Exp;
			RefreshStatus();
		}
		break;
	case 0x181:
		{
			p181 *packet = (p181*)(Header);
			
			if(packet->Header.ClientId == clientId)
			{
				Mob.Status.curHP = packet->curHP;
				Mob.Status.curMP = packet->curMP;
			
				Mob.Status.maxHP = packet->maxHP;
				Mob.Status.maxMP = packet->maxMP;
			}
			else
			{
				Players[packet->Header.ClientId].Status.curHP = packet->curHP;
				Players[packet->Header.ClientId].Status.curMP = packet->curMP;
				
				Players[packet->Header.ClientId].Status.maxHP = packet->maxHP;
				Players[packet->Header.ClientId].Status.maxMP = packet->maxMP;

				if(SelectedMob == packet->Header.ClientId)
					RefreshMobStatus();
			}

			RefreshStatus();
		}
		break;
	case 0x36C:
		{
			p36C *packet = (p36C*)(Header);

			INT32 id = packet->Header.ClientId;
			if(id == Client.clientId)
			{
				Client.Position.X = packet->Destiny.X;
				Client.Position.Y = packet->Destiny.Y;

				if(packet->MoveType == 1)
					ServerMessage("Você foi teleportado pelo servidor!");
			}
			else
			{
				Players[id].Position.X = packet->Destiny.X;
				Players[id].Position.Y = packet->Destiny.Y;

				if(SelectedMob == id)
					RefreshMobStatus();
			}

			g_pMobGrid[packet->LastPos.Y][packet->LastPos.X] = 0;
			g_pMobGrid[packet->Destiny.Y][packet->Destiny.X] = id;
		}
		break;
	case 0x37F:
		{
			p37F *packet = (p37F*)(Header);

			ServerMessage("%s (%d) enviou solicitação de grupo!", packet->nickName, packet->leaderId);
		}
		break;
	case 0x37E:
		{
			p37E *packet = (p37E*)(Header);

			if(packet->mobId == clientId)
			{
				ServerMessage("Saiu do grupo com sucesso!");

				for(INT32 i = 0; i <12; i++)
				{
					Party[i].ClientID = 0;
					Party[i].nickName[0] = 0;
				}

				LeaderId = 0;
			}
			else
			{
				for(INT32 i = 0; i < 12; i++)
				{
					if(Party[i].ClientID == packet->mobId)
					{
						ServerMessage("%s saiu do grupo", Party[i].nickName);

						Party[i].ClientID		= 0;
						Party[i].nickName[0]	= 0;

						break;
					}
				}

				if(packet->mobId == LeaderId)
					LeaderId = 0;
			}

			RefreshGroup();
		}
		break;
	case 0x3AF:
		{
			pMsgSignal *packet = (pMsgSignal*)(Header);

			Mob.Gold = packet->Value;
			RefreshStatus();
		}
		break;
	case 0x37D:
		{
			p37D *packet = (p37D*)(Header);

			for(INT32 i = 0; i < 12;i++)
			{
				if(Party[i].ClientID == packet->PartyID)
					return false;
			}

			for(INT32 i = 0; i < 12; i++)
			{
				if(Party[i].ClientID == 0)
				{
					Party[i].ClientID = packet->PartyID;
					strncpy(Party[i].nickName, packet->nickName, 12);

					break;
				}
			}

			if(packet->PartyID == packet->LiderID)
				LeaderId = packet->LiderID;

			ServerMessage("Grupo com %s (%d)", packet->nickName, packet->PartyID);
			RefreshGroup();
		}
		break;
	default:
	//	ServerMessage("PacketID: 0x%X. Size: %d", Header->PacketId, size);
		break;
	}
	return true;
}