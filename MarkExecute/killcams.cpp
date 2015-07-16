#include "killcams.h"

KillCam::KillCam(Ped* playerPed, Ped* targetPed, Any* camera)
{
	this->playerPed = playerPed;
	this->targetPed = targetPed;
	this->camera = camera;
}

bool KillCam::play()
{
	return false;
}

Any KillCam::getCustomCam()
{
	return this->customCam;
}

// Simple Kill Cam
bool SimpleKillCam::play()
{
	if(firstRun)
	{
		this->customCam = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
		CAM::ATTACH_CAM_TO_ENTITY(this->customCam, *playerPed, 0.95f, -3.14f, 0.8626f, 1);
		CAM::SET_CAM_FOV(this->customCam, 30.0);
		CAM::POINT_CAM_AT_ENTITY(this->customCam, *targetPed, 0.0, 0.0, 0.2f, 1);
		CAM::SET_CAM_ACTIVE_WITH_INTERP(this->customCam, *camera, 500, true, false);
		
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
		GAMEPLAY::SET_TIME_SCALE(1.0);
		return false;
	}

	return true;
}

// Random Kill Cam
bool RandomKillCam::play()
{
	return false;
}


// CloseUp Kill Cam
bool CloseUpKillCam::play()
{
	if (firstRun)
	{
		this->customCam = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
		CAM::ATTACH_CAM_TO_ENTITY(this->customCam, *targetPed, -1.5f, -5.14f, 0.2626f, 1);
		CAM::SET_CAM_FOV(this->customCam, 50.0);
		CAM::POINT_CAM_AT_ENTITY(this->customCam, *playerPed, 0.0, 0.0, 0.2f, 1);
		CAM::SET_CAM_ACTIVE_WITH_INTERP(this->customCam, *camera, 500, true, false);

		GAMEPLAY::SET_TIME_SCALE(0.0f);

		this->time = GetTickCount();
		this->firstRun = false;
	}

	if (!this->isDead && !CAM::IS_CAM_INTERPOLATING(this->customCam))
	{
		GAMEPLAY::SET_TIME_SCALE(1.0f);
	}

	if (!this->isDead && PED::_IS_PED_DEAD(*targetPed, 1))
	{
		Vector3 cameraPos = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(*targetPed, 1.5f, -5.0f, 4.0f);
		Vector3 cameraPoint = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(*targetPed, 0.0f, 0.0f, 0.5f);

		this->secondCam = CAM::CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", cameraPos.x, cameraPos.y, cameraPos.z, 0, 0, 0, 30.0f, 1, 1);
		CAM::SET_CAM_FOV(this->secondCam, 30.0);
		CAM::POINT_CAM_AT_COORD(this->secondCam, cameraPoint.x, cameraPoint.y, cameraPoint.z);
		//CAM::POINT_CAM_AT_ENTITY(this->secondCam, *targetPed, 0.0, 0.0, 0.6f, 1);
		CAM::SET_CAM_ACTIVE_WITH_INTERP(this->secondCam, this->customCam, 500, true, false);

		
		GAMEPLAY::SET_TIME_SCALE(0.4f);

		this->isDead = true;
		this->time = GetTickCount();
	}

	if (this->isDead && this->time + 1500 < GetTickCount())
	{
		GAMEPLAY::SET_TIME_SCALE(1.0);
		return false;
	}

	return true;
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
		CAM::ATTACH_CAM_TO_ENTITY(this->customCam, *playerPed, 2.0f, -2.0f, 28.0f, 1);
		CAM::SET_CAM_FOV(this->customCam, 30.0);
		CAM::POINT_CAM_AT_ENTITY(this->customCam, *playerPed, 0.0, 0.0, 0.2f, 1);
		CAM::SET_CAM_ACTIVE_WITH_INTERP(this->customCam, *camera, 500, true, false);

		this->time = GetTickCount();
		this->firstRun = false;
	}


	if (!this->isDead && PED::_IS_PED_DEAD(*targetPed, 1))
	{
		//GAMEPLAY::SET_TIME_SCALE(0.2f);

		this->isDead = true;
		this->time = GetTickCount();
	}

	if (this->isDead && this->time + 1500 < GetTickCount())
	{
		GAMEPLAY::SET_TIME_SCALE(1.0);
		return false;
	}

	return true;
}