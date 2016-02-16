#pragma once

#include <string>

#include "natives.h"
#include "types.h"
#include "enums.h"

#include "main.h"

#include <menuv/item.h>

void MarkAndExecuteMain();
void main();
void createMenu();

int parseTargetsLimit(std::string limit);
void markTargetPed(Entity target);
void unmarkTargetPed(Entity target);
bool isPedMarked(Entity ped);
bool getScreenCoords(Vector3* coords);

int getReloadDelay(Hash weaponHash);
void reloadWeapon();
void showMessage(std::string msg);

class MenuCallbacks
{
public:
	static void menuKey(MenuVItem* item);
	static void markKey(MenuVItem* item);
	static void executeKey(MenuVItem* item);
	static void markAll(MenuVItem* item);
	static void limit(MenuVItem* item);
	static void killCam(MenuVItem* item);

	static void other(int index, MenuVItem* item);
};