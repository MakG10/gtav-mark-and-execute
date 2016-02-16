/*
  Author: MakG (http://www.gta-mods.pl)
*/

#include <string>
#include <ctime>
#include <windows.h>
#include <Xinput.h>
#include <map>
#include <vector>
#include <boost/lexical_cast.hpp>

#include "script.h"
#include "keyboard.h"
#include "utils.h"

#include <menuv.h>
#include <menuv/template/gtamodspl.h>
#include "settings.h"

#include "killcams.h"

using namespace std;

#pragma warning(disable : 4244 4305) // double <-> float conversions
#pragma warning( disable : 4996 )

#define MAX_TARGETS_LIMIT 30
#define CAMERA_MARK_PEDS_LIMIT 100
#define CAMERA_MARK_MARGIN 30
#define MARK_TIMEOUT 800

Settings settings;
MenuV menu;
MenuVTemplateGtaModsPl menuTemplate;

vector<Ped> pedTargets;
int targetsLimit = MAX_TARGETS_LIMIT;
bool markButtonUp = true;

void MarkAndExecuteMain()
{
	srand(GetTickCount());
	main();
}

void main()
{
	/*settings.setOption("MarkExecute", "menuKey", "F3");
	settings.setOption("MarkExecute", "markKey", "X");
	settings.setOption("MarkExecute", "executeKey", "E");
	settings.setOption("MarkExecute", "markAllOnHold", "true");
	settings.setOption("MarkExecute", "targetsLimit", "5");
	settings.setOption("MarkExecute", "killCam", "Random");*/

	settings.load("MarkExecute.ini");
	if (settings.getOptions().size() == 0)
	{
		settings.setOption("MarkExecute", "menuKey", "F3");
		settings.setOption("MarkExecute", "markKey", "X");
		settings.setOption("MarkExecute", "executeKey", "E");
		settings.setOption("MarkExecute", "markAllOnHold", "true");
		settings.setOption("MarkExecute", "targetsLimit", "5");
		settings.setOption("MarkExecute", "killCam", "Random");

		settings.save();
	}

	targetsLimit = parseTargetsLimit(settings.getOption("targetsLimit"));
	
	createMenu();

	InitEyeX();
	
	Ped playerPed;
	GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT("GolfPutting", true);

	Vector3 markerPosition;

	bool doublePressState = false;
	bool menuState = false;
	DWORD inputWait = 0;
	DWORD holdTime = 0;
	DWORD doublePressTime1 = 0;
	DWORD doublePressTime2 = 0;
	while (true)
	{
		playerPed = PLAYER::GET_PLAYER_PED(0);

		if (menuState) menu.draw();

		if (IsKeyCombinationDown(settings.getOption("menuKey")) && inputWait < GetTickCount())
		{
			menuState = !menuState;
			inputWait = GetTickCount() + 500;
		}

		if (PLAYER::IS_PLAYER_FREE_AIMING(0))
		{
			if (
				markButtonUp &&
				(IsKeyCombinationDown(settings.getOption("markKey")) || IsControllerButtonPressed(XINPUT_GAMEPAD_A)) &&
				inputWait < GetTickCount()
			)
			{
				markButtonUp = false;
				inputWait = GetTickCount() + 100;

				Entity pedTarget;

				if (PLAYER::GET_PLAYER_TARGET_ENTITY(PLAYER::PLAYER_ID(), &pedTarget) || PLAYER::_GET_AIMED_ENTITY(PLAYER::PLAYER_ID(), &pedTarget))
				{
					if (!isPedMarked(pedTarget))
					{
						markTargetPed(pedTarget);
					} else {
						unmarkTargetPed(pedTarget);
					}
				}
			}
		} else {
			if(settings.getOptionBool("markAllOnHold"))
			{
				if (IsKeyCombinationDown(settings.getOption("markKey")) || IsControllerButtonPressed(XINPUT_GAMEPAD_B))
				{
					if(holdTime == 0) holdTime = GetTickCount();
					
					if(holdTime + 1000 < GetTickCount())
					{
						markButtonUp = false;

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

			if (markButtonUp && (IsKeyCombinationDown(settings.getOption("markKey")) || IsControllerButtonPressed(XINPUT_GAMEPAD_A)))
			{
				markButtonUp = false;

				Vector3 lookScreenPos;

				if (getScreenCoords(&lookScreenPos))
				{
					const int arrSize = CAMERA_MARK_PEDS_LIMIT * 2 + 2;
					int foundPeds[arrSize];
					foundPeds[0] = CAMERA_MARK_PEDS_LIMIT;

					int count = PED::GET_PED_NEARBY_PEDS(playerPed, foundPeds, 100);
					if (count > CAMERA_MARK_PEDS_LIMIT) count = CAMERA_MARK_PEDS_LIMIT;
					for (int i = 0; i < count; ++i)
					{
						int offsettedID = i * 2 + 2;

						if (ENTITY::DOES_ENTITY_EXIST(foundPeds[offsettedID]))
						{
							Ped targetPed = foundPeds[offsettedID];
							Vector3 targetPedPos = ENTITY::GET_ENTITY_COORDS(targetPed, true);

							float targetPedScreenPosX, targetPedScreenPosY;
							GRAPHICS::_WORLD3D_TO_SCREEN2D(targetPedPos.x, targetPedPos.y, targetPedPos.z, &targetPedScreenPosX, &targetPedScreenPosY);

							Vector3 targetPedScreenPos;
							targetPedScreenPos.x = targetPedScreenPosX;
							targetPedScreenPos.y = targetPedScreenPosY;

							/*DWORD test = GetTickCount();
							while (true)
							{
							if (test + 2000 < GetTickCount()) break;
							showMessage(to_string(lookScreenPos.x) + ", " + to_string(lookScreenPos.y) + " - " + to_string(targetPedScreenPos.x) + ", " + to_string(targetPedScreenPos.y));
							WAIT(0);
							}*/

							if (getDistanceBetweenPoints(lookScreenPos, targetPedScreenPos) < 0.1)
							{
								if (!isPedMarked(targetPed))
								{
									markTargetPed(targetPed);
								} else
								{
									unmarkTargetPed(targetPed);
								}
								break;
							}
						}
					}
				}
			}
		}

		if (IsKeyCombinationJustUp(settings.getOption("markKey")) || IsControllerButtonJustPressed(XINPUT_GAMEPAD_A))
		{
			markButtonUp = true;
		}

		if (
			(IsKeyCombinationDown(settings.getOption("executeKey")) || doublePressState) &&
			(pedTargets.size() > 0 && !PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0) && inputWait < GetTickCount())
		)
		{
			inputWait = GetTickCount() + 500;
			
			// Wlaczenie wlasnych kamer, ustawienie punku startowego do przejsc kamery
			
			//Any camera = CAM::GET_RENDERING_CAM();
			Vector3 gameplayCamPos = CAM::GET_GAMEPLAY_CAM_COORD();
			Vector3 gameplayCamRot = CAM::GET_GAMEPLAY_CAM_ROT(2);
			float gameplayCamFov = CAM::GET_GAMEPLAY_CAM_FOV();

			
			Any camera = CAM::CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPT_CAMERA", gameplayCamPos.x, gameplayCamPos.y, gameplayCamPos.z, gameplayCamRot.x, gameplayCamRot.y, gameplayCamRot.z, gameplayCamFov, 1, 0);
			CAM::SET_CAM_ACTIVE(camera, 1);

			CAM::RENDER_SCRIPT_CAMS(1, 0, 3000, 1, 0);
			
			// Potrzebne informacje
			Vector3 playerPosition = ENTITY::GET_ENTITY_COORDS(playerPed, 1);
			Hash playerWeapon;
			WEAPON::GET_CURRENT_PED_WEAPON(playerPed, &playerWeapon, true);

			// Tymczasowe odebranie kontroli
			PLAYER::SET_PLAYER_CONTROL(0, FALSE, 0);
			AI::CLEAR_PED_TASKS(playerPed);

			DWORD maxCheckTime = GetTickCount() + 500;
			while (true)
			{
				if (maxCheckTime < GetTickCount() || (!AI::IS_PED_RUNNING(playerPed) && !AI::IS_PED_SPRINTING(playerPed)))
				{
					break;
				}

				WAIT(0);
			}

			int i = 0;
			DWORD time = GetTickCount();
			for (auto &ped : pedTargets)
			{
				// Wybranie kamery zabojstwa
				string killCamName = settings.getOption("killCam");
				KillCam* killCam = new KillCam();

				if (killCamName == "Random")
				{
					int randomCam = rand() % 4 + 1;
					
					if (randomCam == 1) killCamName = "CloseUp";
					else if (randomCam == 2) killCamName = "Simple";
					else if (randomCam == 3) killCamName = "SimpleFast";
					else killCamName = "CloseUp";
				}

				if (killCamName == "CloseUp") killCam = new CloseUpKillCam(&playerPed, &ped, &camera);
				else if (killCamName == "Simple") killCam = new SimpleKillCam(&playerPed, &ped, &camera);
				else if (killCamName == "SimpleFast") killCam = new SimpleFastKillCam(&playerPed, &ped, &camera);
				else if (killCamName == "TopDown") killCam = new TopDownKillCam(&playerPed, &ped, &camera);
				//

				AI::TASK_SHOOT_AT_ENTITY(playerPed, ped, 700, 5);

				//Vector3 pedPosition = ENTITY::GET_ENTITY_COORDS(ped, true);

				//AI::TASK_TURN_PED_TO_FACE_ENTITY(playerPed, ped, 1500);
				//AI::TASK_SHOOT_AT_COORD(playerPed, pedPosition.x, pedPosition.y, pedPosition.z, 100, 6);

				Vector3 targetPedPos = ENTITY::GET_ENTITY_COORDS(ped, 1);
				DWORD timeToKill = 0;
				bool camStillPlaying = false;
				time = GetTickCount();
				while (true)
				{
					if (timeToKill == 0)
					{
						if (playerWeapon == GAMEPLAY::GET_HASH_KEY("WEAPON_RPG") || playerWeapon == GAMEPLAY::GET_HASH_KEY("WEAPON_HOMINGLAUNCHER") || playerWeapon == GAMEPLAY::GET_HASH_KEY("WEAPON_GRENADELAUNCHER"))
						{
							if (GAMEPLAY::IS_PROJECTILE_IN_AREA(targetPedPos.x - 9.0f, targetPedPos.y - 9.0f, targetPedPos.z - 9.0f, targetPedPos.x + 9.0f, targetPedPos.y + 9.0f, targetPedPos.z + 9.0f, 1))
							{
								killCam->killHeadsUp();
								timeToKill = GetTickCount();
							}
						}

						else if (PED::IS_PED_SHOOTING(playerPed))
						{
							killCam->killHeadsUp();
							timeToKill = GetTickCount();
						}
					}

					if (timeToKill != 0 && timeToKill != -1 && timeToKill + 150 < GetTickCount())
					{
						PED::EXPLODE_PED_HEAD(ped, playerWeapon);
						timeToKill = -1;
					}

					camStillPlaying = killCam->play();

					if (time + 1000 < GetTickCount() && (!camStillPlaying || time + 4000 < GetTickCount()))
					{
						ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&ped);
						reloadWeapon();

						break;
					}
					
					WAIT(0);
				}
				
				GAMEPLAY::SET_TIME_SCALE(1.0);

				CAM::DESTROY_CAM(camera, 0);
				camera = killCam->getCustomCam();

				//delete &killCam;
				
				if(i < pedTargets.size() - 1) WAIT(getReloadDelay(playerWeapon));

				i++;
			}

			//CAM::SET_CAM_ACTIVE(camera, 0);
			CAM::RENDER_SCRIPT_CAMS(0, 0, 3000, 1, 0);
			
			CAM::DESTROY_CAM(camera, 0);

			PLAYER::SET_PLAYER_CONTROL(0, TRUE, 0);
			pedTargets.clear();
			
			doublePressState = false;
		}
		
		// Controller double-press
		if (IsControllerButtonPressed(XINPUT_GAMEPAD_A))
		{
			if (doublePressTime2 != 0 && doublePressTime2 - doublePressTime1 < 150)
			{
				doublePressState = true;
			} else
			{
				doublePressTime1 = GetTickCount();
				doublePressTime2 = 0;
				doublePressState = false;
			}
		}

		if (!IsControllerButtonPressed(XINPUT_GAMEPAD_A) && doublePressTime1 != 0)
		{
			doublePressTime2 = GetTickCount();
		}
		//

		if (GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED("GolfPutting"))
		{
			for (auto &ped : pedTargets)
			{
				if (!PED::_IS_PED_DEAD(ped, 1))
				{
					markerPosition = ENTITY::GET_ENTITY_COORDS(ped, true);
					GRAPHICS::DRAW_MARKER(3, markerPosition.x, markerPosition.y, markerPosition.z + 1.1, 0.0, 0.0, 0.0, 0.0, 90.0, 0.0, 0.5, 0.5, 0.5, 153, 0, 0, 200, true, true, 2, false, "GolfPutting", "PuttingMarker", 0);
				} else
				{
					unmarkTargetPed(ped);
				}
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
	MenuVItem killCamItem = MenuVItem("Kill Cam", OPTION_SET, MenuCallbacks::killCam);
	killCamItem.optionValue = to_string(targetsLimit);
	killCamItem.setOptions({ "Random", "Simple", "SimpleFast", "CloseUp", "TopDown" });
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
		/*Vector3 pedPos = ENTITY::GET_ENTITY_COORDS(target, 1);
		Vector3 playerPos = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED(0), 1);

		if (GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(pedPos.x, pedPos.y, pedPos.z, playerPos.x, playerPos.y, playerPos.z, 1) < 70.0f)
		{
			pedTargets.push_back(target);
		}*/

		if (pedTargets.size() < targetsLimit)
		{
			ENTITY::SET_ENTITY_AS_MISSION_ENTITY(target, 1, 1);
			pedTargets.push_back(target);
		}
	}
}

void unmarkTargetPed(Entity target)
{
	pedTargets.erase(remove(pedTargets.begin(), pedTargets.end(), target), pedTargets.end());
}

bool isPedMarked(Entity ped)
{
	return find(pedTargets.begin(), pedTargets.end(), ped) != pedTargets.end();
}

bool getScreenCoords(Vector3* coords)
{
	int width, height;
	GRAPHICS::_GET_SCREEN_ACTIVE_RESOLUTION(&width, &height);

	if (getGazePoint(coords))
	{
		coords->x /= (float)width;
		coords->y /= (float)height;

		return true;
	} else {
		coords->x = (float)width / 2.0f / (float)width;
		coords->y = (float)height / 2.0f / (float)height;
		coords->z = 0;
		
		return true;
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
