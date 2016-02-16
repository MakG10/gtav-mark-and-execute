#include <math.h>
#include <string>
#include <vector>
#include "killcams.h"

KillCam::KillCam(Ped* playerPed, Ped* targetPed, Any* camera)
{
	this->playerPed = playerPed;
	this->targetPed = targetPed;
	this->camera = camera;
}

KillCam::KillCam()
{

}

KillCam::~KillCam()
{
	//CAM::SET_CAM_ACTIVE(this->customCam, 0);
	//CAM::RENDER_SCRIPT_CAMS(0, 0, 3000, 1, 0);
}

bool KillCam::play()
{
	return false;
}

Any KillCam::getCustomCam()
{
	return this->customCam;
}

void KillCam::killHeadsUp()
{
	this->isAboutToKill = true;
}

// Simple Kill Cam
bool SimpleKillCam::play()
{
	if(firstRun)
	{
		this->customCam = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
		CAM::ATTACH_CAM_TO_ENTITY(this->customCam, *playerPed, 0.75f, -3.54f, 0.8626f, 1);
		CAM::SET_CAM_FOV(this->customCam, 15.0);
		CAM::POINT_CAM_AT_ENTITY(this->customCam, *targetPed, 0.0, 0.0, 0.2f, 1);
		CAM::SET_CAM_ACTIVE_WITH_INTERP(this->customCam, *camera, 500, true, false);

		CAM::RENDER_SCRIPT_CAMS(1, 0, 3000, 1, 0);

		// Efekt, jezeli jest przejscie z innej, dalszej kamery
		/*Vector3 oldCamPos = CAM::GET_CAM_COORD(*camera);
		Vector3 newCamPos = CAM::GET_CAM_COORD(this->customCam);

		if (GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(oldCamPos.x, oldCamPos.y, oldCamPos.z, newCamPos.x, newCamPos.y, newCamPos.z, 1) > 30.0f)
		{
			GRAPHICS::_START_SCREEN_EFFECT("SwitchShortMichaelIn", 0, 0);
		}*/
		//
		
		this->time = GetTickCount();
		this->firstRun = false;
	}
	
	
	if(!this->isDead && PED::_IS_PED_DEAD(*targetPed, 1))
	{
		GAMEPLAY::SET_TIME_SCALE(0.2f);
		
		this->isDead = true;
		this->time = GetTickCount();
	}
	
	if(this->isDead && this->time + 1500 < GetTickCount())
	{
		GRAPHICS::_STOP_SCREEN_EFFECT("SwitchShortMichaelIn");

		GAMEPLAY::SET_TIME_SCALE(1.0);
		return false;
	}

	return true;
}


// Simple Fast Kill Cam
bool SimpleFastKillCam::play()
{
	if (firstRun)
	{
		this->customCam = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
		CAM::ATTACH_CAM_TO_ENTITY(this->customCam, *playerPed, 0.75f, -3.54f, 0.8626f, 1);
		CAM::SET_CAM_FOV(this->customCam, 15.0);
		CAM::POINT_CAM_AT_ENTITY(this->customCam, *targetPed, 0.0, 0.0, 0.2f, 1);
		CAM::SET_CAM_ACTIVE_WITH_INTERP(this->customCam, *camera, 500, true, false);

		CAM::RENDER_SCRIPT_CAMS(1, 0, 3000, 1, 0);

		// Efekt, jezeli jest przejscie z innej, dalszej kamery
		/*Vector3 oldCamPos = CAM::GET_CAM_COORD(*camera);
		Vector3 newCamPos = CAM::GET_CAM_COORD(this->customCam);

		if (GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(oldCamPos.x, oldCamPos.y, oldCamPos.z, newCamPos.x, newCamPos.y, newCamPos.z, 1) > 30.0f)
		{
		GRAPHICS::_START_SCREEN_EFFECT("SwitchShortMichaelIn", 0, 0);
		}*/
		//

		this->time = GetTickCount();
		this->firstRun = false;
	}


	if (!this->isDead && PED::_IS_PED_DEAD(*targetPed, 1))
	{
		this->isDead = true;
		this->time = GetTickCount();
	}

	if (this->isDead && this->time + 300 < GetTickCount())
	{
		GRAPHICS::_STOP_SCREEN_EFFECT("SwitchShortMichaelIn");

		GAMEPLAY::SET_TIME_SCALE(1.0);
		return false;
	}

	return true;
}


// CloseUp Kill Cam
bool CloseUpKillCam::play()
{
	if (firstRun)
	{
		this->customCam = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
		
		if (PED::IS_PED_IN_ANY_VEHICLE(*targetPed, 0))
		{
			CAM::ATTACH_CAM_TO_ENTITY(this->customCam, PED::GET_VEHICLE_PED_IS_IN(*targetPed, 0), 0.0, 5.0f, 0.4626f, 1);
		} else
		{
			CAM::ATTACH_CAM_TO_ENTITY(this->customCam, *targetPed, -1.5f, -5.14f, 0.4626f, 1);
		}

		CAM::SET_CAM_FOV(this->customCam, 50.0);
		CAM::POINT_CAM_AT_ENTITY(this->customCam, *playerPed, 0.0, 0.0, 0.2f, 1);
		CAM::SET_CAM_ACTIVE_WITH_INTERP(this->customCam, *camera, 400, true, true);

		CAM::RENDER_SCRIPT_CAMS(1, 0, 3000, 1, 0);

		GRAPHICS::_START_SCREEN_EFFECT("SwitchShortMichaelIn", 0, 0);

		GAMEPLAY::SET_TIME_SCALE(0.0f);

		this->time = GetTickCount();
		this->firstRun = false;
	}

	if (!this->isDead && !CAM::IS_CAM_INTERPOLATING(this->customCam))
	{
		GAMEPLAY::SET_TIME_SCALE(0.7f);
	}

	if (this->isAboutToKill)
	{
		if (PED::IS_PED_IN_ANY_VEHICLE(*targetPed, 0))
		{
			this->secondCam = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
			CAM::ATTACH_CAM_TO_ENTITY(this->secondCam, PED::GET_VEHICLE_PED_IS_IN(*targetPed, 0), 2.0f, 5.0f, 0.7626f, 1);
			CAM::SET_CAM_FOV(this->secondCam, 25.0);
			CAM::POINT_CAM_AT_ENTITY(this->secondCam, *targetPed, 0.0, 0.0, 0.2f, 1);
		} else {
			Vector3 headPosition = PED::GET_PED_BONE_COORDS(*targetPed, 31086, 0.0, 0.0, 0.0);
			Vector3 headPositionOffsetted = PED::GET_PED_BONE_COORDS(*targetPed, 31086, 1.0f, 1.0f, 0.3f);

			Vector3 camPosition = headPositionOffsetted;
			camPosition.x -= 2.0f;

			this->secondCam = CAM::CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", camPosition.x, camPosition.y, camPosition.z, 0, 0, 0, 30.0f, 1, 1);
			CAM::SET_CAM_FOV(this->secondCam, 50.0);
			CAM::SET_CAM_ROT(this->secondCam, 0.0, 30.0f, 0.0, 2);

			Vector3 velocity = ENTITY::GET_ENTITY_VELOCITY(*targetPed);
			if (sqrt(pow(velocity.x, 2) + pow(velocity.y, 2) + pow(velocity.z, 2)) > 2.0f)
			{
				CAM::POINT_CAM_AT_ENTITY(this->secondCam, *targetPed, 0.0, 0.0, 0.6f, 1);
			} else
			{
				CAM::POINT_CAM_AT_COORD(this->secondCam, headPosition.x, headPosition.y, headPosition.z);
			}

			// Proba znalezienia pozycji kamery niekolidujacej z innymi obiektami
			BOOL occupied = GAMEPLAY::IS_POSITION_OCCUPIED(camPosition.x, camPosition.y, camPosition.z, 0.2f, 0, 1, 0, 0, 0, 0, 0);

			if (occupied)
			{
				std::vector<float> tryOffsetsX = { -2.0f, -1.0f, 1.0f, 2.0f };
				std::vector<float> tryOffsetsY = { -2.0f, -1.0f, 1.0f, 2.0f };

				bool foundGoodPosition = false;
				Vector3 newCamPosition = camPosition;
				for (auto &offsetX : tryOffsetsX)
				{
					for (auto &offsetY : tryOffsetsY)
					{
						foundGoodPosition = !GAMEPLAY::IS_POSITION_OCCUPIED(camPosition.x + offsetX, headPositionOffsetted.y + offsetY, headPositionOffsetted.z, 0.2f, 0, 1, 0, 0, 0, 0, 0);

						if (foundGoodPosition)
						{
							newCamPosition.x = camPosition.x + offsetX;
							newCamPosition.y = camPosition.y + offsetY;
							newCamPosition.z = camPosition.z;

							break;
						}
					}
				}

				if (!foundGoodPosition)
				{
					newCamPosition.x += 2.0f;
					newCamPosition.z += 4.0f;
				}

				CAM::SET_CAM_COORD(this->secondCam, newCamPosition.x, newCamPosition.y, newCamPosition.z);
			}
		}
		//

		CAM::SET_CAM_ACTIVE_WITH_INTERP(this->secondCam, this->customCam, 200, true, true);
		CAM::RENDER_SCRIPT_CAMS(1, 0, 3000, 1, 0);

		//GRAPHICS::_START_SCREEN_EFFECT("SwitchShortMichaelIn", 0, 0);

		this->isAboutToKill = false;
	}

	if (!this->isDead && PED::_IS_PED_DEAD(*targetPed, 1))
	{
		GAMEPLAY::SET_TIME_SCALE(0.2f);

		/*Vector3 cameraPos = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(*targetPed, 1.5f, -5.0f, 4.0f);
		Vector3 cameraPoint = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(*targetPed, 0.0f, 0.0f, 0.5f);

		this->secondCam = CAM::CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", cameraPos.x, cameraPos.y, cameraPos.z, 0, 0, 0, 30.0f, 1, 1);
		CAM::SET_CAM_FOV(this->secondCam, 30.0);
		CAM::POINT_CAM_AT_COORD(this->secondCam, cameraPoint.x, cameraPoint.y, cameraPoint.z);
		//CAM::POINT_CAM_AT_ENTITY(this->secondCam, *targetPed, 0.0, 0.0, 0.6f, 1);
		CAM::SET_CAM_ACTIVE_WITH_INTERP(this->secondCam, this->customCam, 500, true, false);*/

		this->isDead = true;
		this->time = GetTickCount();
	}

	if (this->isDead && this->time + 1500 < GetTickCount())
	{
		GRAPHICS::_STOP_SCREEN_EFFECT("SwitchShortMichaelIn");
		CAM::DESTROY_CAM(this->customCam, 0);

		GAMEPLAY::SET_TIME_SCALE(1.0);
		return false;
	}

	return true;
}

CloseUpKillCam::~CloseUpKillCam()
{
	CAM::SET_CAM_ACTIVE(this->customCam, 0);
	CAM::DESTROY_CAM(this->customCam, 0);
}

Any CloseUpKillCam::getCustomCam()
{
	return this->secondCam;
}


// Top Down Kill Cam
bool TopDownKillCam::play()
{
	if (firstRun)
	{
		this->customCam = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
		CAM::ATTACH_CAM_TO_ENTITY(this->customCam, *targetPed, 0.0f, 0.0f, 15.0f, 1);
		CAM::SET_CAM_FOV(this->customCam, 25.0);
		CAM::POINT_CAM_AT_ENTITY(this->customCam, *targetPed, 0.0, 0.0, 0.2f, 1);
		CAM::SET_CAM_ACTIVE_WITH_INTERP(this->customCam, *camera, 200, true, false);

		CAM::RENDER_SCRIPT_CAMS(1, 0, 3000, 1, 1);

		GRAPHICS::_START_SCREEN_EFFECT("SwitchShortMichaelIn", 0, 0);

		GAMEPLAY::SET_TIME_SCALE(0.0f);

		this->time = GetTickCount();
		this->firstRun = false;
	}

	if (!this->isDead && !CAM::IS_CAM_INTERPOLATING(this->customCam))
	{
		GAMEPLAY::SET_TIME_SCALE(1.0f);
	}

	if (this->isAboutToKill)
	{
		Vector3 camPosition = CAM::GET_CAM_COORD(this->customCam);

		CAM::ATTACH_CAM_TO_ENTITY(this->customCam, *targetPed, 0.0f, 0.0f, 15.0f, 0);
		CAM::SET_CAM_COORD(this->customCam, camPosition.x, camPosition.y, camPosition.z);
		CAM::POINT_CAM_AT_COORD(this->customCam, camPosition.x + 0.1f, camPosition.y, camPosition.z - 10.0f);

		this->isAboutToKill = false;
	}

	if (!this->isDead && PED::_IS_PED_DEAD(*targetPed, 1))
	{
		GAMEPLAY::SET_TIME_SCALE(0.2f);

		this->isDead = true;
		this->time = GetTickCount();
	}

	if (this->isDead && this->time + 1500 < GetTickCount())
	{
		GRAPHICS::_STOP_SCREEN_EFFECT("SwitchShortMichaelIn");

		GAMEPLAY::SET_TIME_SCALE(1.0);
		return false;
	}

	return true;
}