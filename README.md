# WYD2Bot
## About Project
The WYD2Bot is a project to simulate a WYD client without interface. This project is constructed only with C/C++ and works with Client version 759 or greater.
This code was made by any which way. 

If you want lower version you need to update all structs.

## Contents
- Read configuration files like: heightmap (movement), attributemap (movement), quiz (auto answer), itemlist, skilldata, itemeffect, autotrade (create autotrade).
- Login system with optional Encrypt Version found in some clients.
- CharList complete.
- Numeric system.
- Login in any character of account.
- Recognizes mobs and players around and gets movement or attack.
- Refresh selected mob in list of mobs with current hp or position.
- Macro system with magical and physical attack. System search the most near enemy.
- Magical macro: can set to heal your group, just set skillbar with heal id.
- Auto buff with transform select (beastmaster) respecting skill delay.
- Use some item of your inventory.
- Auto feed your pet.
- Auto pot.
- Water Macro.
- Patchfinding.

## Prerequisites
You will need just four files: `AttributeMap.dat`, `HeightMap.dat`, `ItemList.csv`, `SkillData.csv`.
The files are found on any release of WYD (see http://www.webcheats.com.br/).

To compile, you will need Visual Studio 2017 or greater.

## Before compile
1. Set in `Socket.cpp` function `Server::Connect` IP and PORT you want to connect.
2. Set in `SendFunc.cpp` function `SendRequestLogin` the client version.

## Use it
After you compile, just open the program and set your login / password / numeric password and click Enter.

In the updated list on right, select character you want to enter and click Enter, below of list. All of info around you will be on program.

## Avaible commands

`move <posX> <posY>`
Move your character to position using Pathfinding.
Disclaimer: use with positions you can move normally. This bot don't use any illegal movement or teleport.
When you stop on a teleport, you will be notified and you can use another command (see below).

`use <itemid>`
Use item of your inventory. If you don't have, you will be notified.

`party exit`
Exit current party.

`party invt <id>`
Invite player with <id> to your group. You can enter on any group with this command (use leader id).

`party kick <id>`
Kick player with <id> from your group (only if you're leader).

`party <nickname> <id>`
Accept group from <nickname> <id> player. Needs to write right nickname and ID.

`water <||0|1||> <||0|1|2||>`
Active/Desactive water macro. Use status 1 to active. Type means Normal, Mistical or Arcane.

`tele`
If you are above a teleport (notified if you stop on one) use this command to access.

`transform <||0|1|2|3|4||>`
Tell to Bot what transform you want to use. In order: werewolf, bear, astaroth, titan and eden.

## How to use Water Macro
First, you need to login normally on server to save Stellar Gem on Water Zone and buy some Scroll Teleport.
After that login with WYD2Bot and write on Command area: #use 699 to teleport.

Just set what water you want to use with: `water 1 <type>`.
`1 => means active status
<type> => 0 = Normal, M = Mystical, A = Arcane`

You will be teleported using item and will automatically go to the next room in order.

## Macro
- Macro will be refresh all time. If you need to change any configuration on skillbar for example, deactivate macro for safety.
- You can't active physical attack and magical at same time.
- Group Macro Mob show current attacked mob.
- EXP checkbox show current earned experience.

## Important thing to do
You need to update with hash system used on 756 version or above to encrypt packet correctly.
This system needs to be on `cServer::AddMessage(char *pMsg, int Size)`. 
