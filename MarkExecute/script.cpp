/*
  Author: MakG (http://www.gta-mods.pl)
*/

#include <string>
#include <ctime>
#include <Windows.h>
#include <map>
#include <vector>
#include <boost/lexical_cast.hpp>

#include "script.h"
#include "keyboard.h"

#include "menuv.h"
#include "menuv/template/gtamodspl.h"
#include "settings.h"

#include "killcams.h"

using namespace std;

#pragma warning(disable : 4244 4305) // double <-> float conversions
#pragma warning( disable : 4996 )

#define MAX_TARGETS_LIMIT 30

Settings settings;
MenuV menu;
MenuVTemplateGtaModsPl menuTemplate;

vector<Ped> pedTargets;
int targetsLimit = MAX_TARGETS_LIMIT;

void MarkAndExecuteMain()
{
	srand(GetTickCount());
	main();
}

void main()
{
	settings.load("MarkExecute.ini");
	targetsLimit = parseTargetsLimit(settings.getOption("targetsLimit"));
	
	createMenu();
	
	Ped playerPed = PLAYER::GET_PLAYER_PED(0);
	GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT("GolfPutting", true);

	Vector3 markerPosition;

	bool menuState = false;
	DWORD inputWait = 0;
	DWORD holdTime = 0;
	while (true)
	{
		if (menuState) menu.draw();

		if (IsKeyCombinationDown(settings.getOption("menuKey")) && inputWait < GetTickCount())
		{
			menuState = !menuState;
			inputWait = GetTickCount() + 500;
		}

		if (PLAYER::IS_PLAYER_FREE_AIMING(0))
		{
			if (IsKeyCombinationDown(settings.getOption("markKey")) && inputWait < GetTickCount())
			{

				inputWait = GetTickCount() + 100;

				Entity pedTarget;

				if (PLAYER::GET_PLAYER_TARGET_ENTITY(PLAYER::PLAYER_ID(), &pedTarget) || PLAYER::_GET_AIMED_ENTITY(PLAYER::PLAYER_ID(), &pedTarget))
				{
					markTargetPed(pedTarget);
				}
			}
		} else {
			if(settings.getOptionBool("markAllOnHold"))
			{
				if (IsKeyCombinationDown(settings.getOption("markKey")))
				{
					if(holdTime == 0) holdTime = GetTickCount();
					
					if(holdTime + 2000 < GetTickCount())
					{
						const int arrSize = MAX_TARGETS_LIMIT * 2 + 2;
						int foundPeds[arrSize];
						foundPeds[0] = targetsLimit;

						int count = PED::GET_PED_NEARBY_PEDS(playerPed, foundPeds, -1);
						if (count > MAX_TARGETS_LIMIT) count = MAX_TARGETS_LIMIT;
						for (int i = 0; i < count; ++i)
						{
							int offsettedID = i * 2 + 2;
							
							if (ENTITY::DOES_ENTITY_EXIST(foundPeds[offsettedID]))
							{
								Ped targetPed = foundPeds[offsettedID];
								markTargetPed(targetPed);
							}
						}
						
						holdTime = 0;
					}
				} else {
					holdTime = 0;
				}
			} else {
				holdTime = 0;
			}
		}

		if (IsKeyCombinationDown(settings.getOption("executeKey")) && pedTargets.size() > 0 && inputWait < GetTickCount())
		{
			inputWait = GetTickCount() + 500;
			
			// Wlaczenie wlasnych kamer, ustawienie punku startowego do przejsc kamery
			
			//Any camera = CAM::GET_RENDERING_CAM();
			Any camera = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
			CAM::ATTACH_CAM_TO_ENTITY(camera, playerPed, -1.5f, -3.14f, 0.8626f, 1);
			CAM::SET_CAM_FOV(camera, 30.0);
			CAM::SET_CAM_ACTIVE(camera, 1);

			CAM::RENDER_SCRIPT_CAMS(1, 0, 3000, 1, 0);
			
			// Potrzebne informacje
			Vector3 playerPosition = ENTITY::GET_ENTITY_COORDS(playerPed, 1);
			Hash playerWeapon;
			WEAPON::GET_CURRENT_PED_WEAPON(playerPed, &playerWeapon, true);

			// Tymczasowe odebranie kontroli
			PLAYER::SET_PLAYER_CONTROL(0, FALSE, 0);

			int i = 0;
			DWORD time = GetTickCount();
			for (auto &ped : pedTargets)
			{
				TopDownKillCam killCam = TopDownKillCam(&playerPed, &ped, &camera);

				AI::TASK_SHOOT_AT_ENTITY(playerPed, ped, 700, 5);

				//Vector3 pedPosition = ENTITY::GET_ENTITY_COORDS(ped, true);

				//AI::TASK_TURN_PED_TO_FACE_ENTITY(playerPed, ped, 1500);
				//AI::TASK_SHOOT_AT_COORD(playerPed, pedPosition.x, pedPosition.y, pedPosition.z, 100, 6);

				bool camStillPlaying = false;
				time = GetTickCount();
				while (true)
				{
					camStillPlaying = killCam.play();

					//if (time + 700 < GetTickCount())
					if (PED::IS_PED_SHOOTING(playerPed))
					{
						PED::EXPLODE_PED_HEAD(ped, playerWeapon);
					}

					if (time + 1000 < GetTickCount() && (!camStillPlaying || time + 6000 < GetTickCount()))
					{
						ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&ped);
						reloadWeapon();

						break;
					}
					
					WAIT(0);
				}
				
				GAMEPLAY::SET_TIME_SCALE(1.0);
				camera = killCam.getCustomCam();
				
				if(i < pedTargets.size() - 1) WAIT(getReloadDelay(playerWeapon));

				i++;
			}

			CAM::SET_CAM_ACTIVE(camera, 0);
			CAM::RENDER_SCRIPT_CAMS(0, 0, 3000, 1, 0);
			
			CAM::DESTROY_CAM(camera, true);

			PLAYER::SET_PLAYER_CONTROL(0, TRUE, 0);
			pedTargets.clear();
		}

		if (GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED("GolfPutting"))
		{
			for (auto &ped : pedTargets)
			{
				markerPosition = ENTITY::GET_ENTITY_COORDS(ped, true);
				GRAPHICS::DRAW_MARKER(3, markerPosition.x, markerPosition.y, markerPosition.z + 1.1, 0.0, 0.0, 0.0, 0.0, 90.0, 0.0, 0.5, 0.5, 0.5, 153, 0, 0, 200, true, true, 2, false, "GolfPutting", "PuttingMarker", 0);
			}
		}

		WAIT(0);
	}
}

void createMenu()
{
	menu = MenuV("Mark & Execute by MakG", MenuCallbacks::other);
	menu.setTemplate(&menuTemplate);
	
	// Skrot klawiszowy do menu
	MenuVItem shortcutItem = MenuVItem("Menu Key", KEYINPUT, MenuCallbacks::menuKey);
	shortcutItem.setKeyValue(str2key(settings.getOption("menuKey", "MarkExecute")));
	shortcutItem.setBgColor(COLOR_PRIMARY);
	menu.addItem(shortcutItem);
	
	// Skrot klawiszowy do oznaczania
	MenuVItem markItem = MenuVItem("Mark Key", KEYINPUT, MenuCallbacks::markKey);
	markItem.setKeyValue(str2key(settings.getOption("markKey", "MarkExecute")));
	markItem.setBgColor(COLOR_PRIMARY);
	menu.addItem(markItem);
	
	// Skrot klawiszowy do egzekucji
	MenuVItem executeItem = MenuVItem("Execute Key", KEYINPUT, MenuCallbacks::executeKey);
	executeItem.setKeyValue(str2key(settings.getOption("executeKey", "MarkExecute")));
	executeItem.setBgColor(COLOR_PRIMARY);
	menu.addItem(executeItem);
	
	// Zaznaczenie wszystkich ON/OFF
	MenuVItem markAllItem = MenuVItem("Mark All on Hold", TOGGLE, MenuCallbacks::markAll);
	markAllItem.toggleValue = settings.getOptionBool("markAllOnHold");
	markAllItem.setBgColor(COLOR_PRIMARY);
	menu.addItem(markAllItem);
	
	// Limit celi do zaznaczenia, 0 - bez limitu, to samo jezeli zostanie wprowadzony tekst, zamiast liczby
	MenuVItem targetsLimitItem = MenuVItem("Targets Limit", OPTION, MenuCallbacks::limit);
	targetsLimitItem.optionValue = to_string(targetsLimit);
	targetsLimitItem.setBgColor(COLOR_PRIMARY);
	menu.addItem(targetsLimitItem);
	
	// Kamery egzekucji
	MenuVItem killCamItem = MenuVItem("Kill Cam", OPTION, MenuCallbacks::killCam);
	killCamItem.optionValue = to_string(targetsLimit);
	killCamItem.setOptions({ "Random", "Simple" });
	killCamItem.optionValue = settings.getOption("killCam", "MarkExecute");
	killCamItem.setBgColor(COLOR_PRIMARY);
	menu.addItem(killCamItem);
}

void MenuCallbacks::menuKey(MenuVItem* item)
{
	settings.setOption("MarkExecute", "menuKey", key2str(item->getKeyValue()));
	settings.save();
}

void MenuCallbacks::markKey(MenuVItem* item)
{
	settings.setOption("MarkExecute", "markKey", key2str(item->getKeyValue()));
	settings.save();
}

void MenuCallbacks::executeKey(MenuVItem* item)
{
	settings.setOption("MarkExecute", "executeKey", key2str(item->getKeyValue()));
	settings.save();
}

void MenuCallbacks::markAll(MenuVItem* item)
{
	settings.setOption("MarkExecute", "markAllOnHold", item->toggleValue ? "true" : "false");
	settings.save();
}

void MenuCallbacks::limit(MenuVItem* item)
{
	targetsLimit = parseTargetsLimit(item->optionValue);

	item->optionValue = to_string(targetsLimit);

	settings.setOption("MarkExecute", "targetsLimit", to_string(targetsLimit));
	settings.save();
}

void MenuCallbacks::killCam(MenuVItem* item)
{
	settings.setOption("MarkExecute", "killCam", item->getOptionValue());
	settings.save();
}

void MenuCallbacks::other(int index, MenuVItem* item)
{

}

int parseTargetsLimit(string limit)
{
	try {
		int n = boost::lexical_cast<int>(limit);
		
		return n;
	} catch (boost::bad_lexical_cast) {
		int n = MAX_TARGETS_LIMIT;
		return n;
	}
}

void markTargetPed(Entity target)
{
	if (!PED::_IS_PED_DEAD(target, 1) && find(pedTargets.begin(), pedTargets.end(), target) == pedTargets.end())
	{
		pedTargets.push_back(target);
	}
}

void reloadWeapon()
{
	Hash cur;
	if (WEAPON::GET_CURRENT_PED_WEAPON(PLAYER::GET_PLAYER_PED(0), &cur, 1))
	{
		if (WEAPON::IS_WEAPON_VALID(cur))
		{
			int maxAmmo;
			if (WEAPON::GET_MAX_AMMO(PLAYER::GET_PLAYER_PED(0), cur, &maxAmmo))
			{
				WEAPON::SET_PED_AMMO(PLAYER::GET_PLAYER_PED(0), cur, maxAmmo);

				maxAmmo = WEAPON::GET_MAX_AMMO_IN_CLIP(PLAYER::GET_PLAYER_PED(0), cur, 1);
				if (maxAmmo > 0)
					WEAPON::SET_AMMO_IN_CLIP(PLAYER::GET_PLAYER_PED(0), cur, maxAmmo);
			}
		}
	}
}

int getReloadDelay(Hash weaponHash)
{
	if (weaponHash == GAMEPLAY::GET_HASH_KEY("WEAPON_RPG") || weaponHash == GAMEPLAY::GET_HASH_KEY("WEAPON_HOMINGLAUNCHER") || weaponHash == GAMEPLAY::GET_HASH_KEY("WEAPON_GRENADELAUNCHER"))
	{
		return 1600;
	}

	if (weaponHash == GAMEPLAY::GET_HASH_KEY("WEAPON_SNIPERRIFLE") || weaponHash == GAMEPLAY::GET_HASH_KEY("WEAPON_HEAVYSNIPER"))
	{
		return 300;
	}

	return 0;
}

void showMessage(std::string msg)
{
	UI::SET_TEXT_FONT(0);
	UI::SET_TEXT_SCALE(0.55, 0.55);
	UI::SET_TEXT_COLOUR(255, 255, 255, 255);
	UI::SET_TEXT_WRAP(0.0, 1.0);
	UI::SET_TEXT_CENTRE(1);
	UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
	UI::SET_TEXT_EDGE(1, 0, 0, 0, 205);
	UI::_SET_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING((char *)msg.c_str());
	UI::_DRAW_TEXT(0.5, 0.5);
}
