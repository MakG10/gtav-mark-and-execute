#pragma once

#include <eyex/EyeX.h>
#include "types.h"

//Tobii.EyeX.Client.lib
BOOL InitEyeX();
BOOL ShutdownEyeX();
BOOL InitializeGlobalInteractorSnapshot(TX_CONTEXTHANDLE hContext);
void TX_CALLCONVENTION OnSnapshotCommitted(TX_CONSTHANDLE hAsyncData, TX_USERPARAM param);
void TX_CALLCONVENTION OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState, TX_USERPARAM userParam);
void OnGazeDataEvent(TX_HANDLE hGazeDataBehavior);
void TX_CALLCONVENTION HandleEvent(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam);


bool getGazePoint(Vector3* vector);
float getVectorLength(Vector3 vector);
float getDistanceBetweenPoints(Vector3 v1, Vector3 v2);
