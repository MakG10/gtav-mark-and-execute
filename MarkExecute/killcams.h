#pragma once

#include <Windows.h>
#include "natives.h"
#include "types.h"
#include "enums.h"

class KillCam
{
public:
	Ped* playerPed;
	Ped* targetPed;
	Any* camera;
	
	Any customCam;
	DWORD time = 0;
	bool firstRun = true;
	
	explicit KillCam(Ped* playerPed, Ped* targetPed, Any* camera);
	virtual bool play();
	virtual Any getCustomCam();
};

class RandomKillCam : public KillCam
{
public:
	using KillCam::KillCam;
	bool play();
};


class SimpleKillCam : public KillCam
{
public:
	explicit SimpleKillCam(Ped* playerPed, Ped* targetPed, Any* camera) : KillCam(playerPed, targetPed, camera) { }
	bool play();
	
private:
	bool isDead = false;
};


class CloseUpKillCam : public KillCam
{
public:
	explicit CloseUpKillCam(Ped* playerPed, Ped* targetPed, Any* camera) : KillCam(playerPed, targetPed, camera) { }
	bool play();
	Any getCustomCam();

private:
	bool isDead = false;
	Any secondCam = 0;
};


class TopDownKillCam : public KillCam
{
public:
	explicit TopDownKillCam(Ped* playerPed, Ped* targetPed, Any* camera) : KillCam(playerPed, targetPed, camera) { }
	bool play();

private:
	bool isDead = false;
	Any secondCam = 0;
};