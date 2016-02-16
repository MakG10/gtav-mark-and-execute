#include <windows.h>
#include <math.h>
#include "utils.h"

TX_CONTEXTHANDLE eyeXContext = TX_EMPTY_HANDLE;
static const TX_STRING InteractorId = "MarkAndExecute";
static TX_HANDLE g_hGlobalInteractorSnapshot = TX_EMPTY_HANDLE;
TX_TICKET hConnectionStateChangedTicket = TX_INVALID_TICKET;
TX_TICKET hEventHandlerTicket = TX_INVALID_TICKET;
TX_GAZEPOINTDATAEVENTPARAMS lastGazeData;
bool gazeAvailable = false;


BOOL InitEyeX()
{
	BOOL success;

	success = txInitializeEyeX(TX_EYEXCOMPONENTOVERRIDEFLAG_NONE, NULL, NULL, NULL, NULL) == TX_RESULT_OK;
	success &= txCreateContext(&eyeXContext, TX_FALSE) == TX_RESULT_OK;
	success &= InitializeGlobalInteractorSnapshot(eyeXContext);
	success &= txRegisterConnectionStateChangedHandler(eyeXContext, &hConnectionStateChangedTicket, OnEngineConnectionStateChanged, NULL) == TX_RESULT_OK;
	success &= txRegisterEventHandler(eyeXContext, &hEventHandlerTicket, HandleEvent, NULL) == TX_RESULT_OK;
	success &= txEnableConnection(eyeXContext) == TX_RESULT_OK;

	return success;
}

BOOL ShutdownEyeX()
{
	BOOL success;

	txDisableConnection(eyeXContext);
	txReleaseObject(&g_hGlobalInteractorSnapshot);
	success = txShutdownContext(eyeXContext, TX_CLEANUPTIMEOUT_FORCEIMMEDIATE, TX_FALSE) == TX_RESULT_OK;
	success &= txReleaseContext(&eyeXContext) == TX_RESULT_OK;
	success &= txUninitializeEyeX() == TX_RESULT_OK;

	return success;
}


bool getGazePoint(Vector3* result)
{
	if (gazeAvailable)
	{
		result->x = (float)lastGazeData.X;
		result->y = (float)lastGazeData.Y;
		result->z = 0;

		return true;
	} else
	{
		return false;
	}
}

float getDistanceBetweenPoints(Vector3 v1, Vector3 v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;

	return sqrt(v1.x * v1.x + v1.y * v1.y + v1.z *v1.z);
}

BOOL InitializeGlobalInteractorSnapshot(TX_CONTEXTHANDLE hContext)
{
	TX_HANDLE hInteractor = TX_EMPTY_HANDLE;
	TX_GAZEPOINTDATAPARAMS params = { TX_GAZEPOINTDATAMODE_LIGHTLYFILTERED };
	BOOL success;

	success = txCreateGlobalInteractorSnapshot(hContext, InteractorId, &g_hGlobalInteractorSnapshot, &hInteractor) == TX_RESULT_OK;
	success &= txCreateGazePointDataBehavior(hInteractor, &params) == TX_RESULT_OK;

	txReleaseObject(&hInteractor);

	return success;
}

void TX_CALLCONVENTION OnSnapshotCommitted(TX_CONSTHANDLE hAsyncData, TX_USERPARAM param)
{
	//TX_RESULT result = TX_RESULT_UNKNOWN;
	//txGetAsyncDataResultCode(hAsyncData, &result);
}

void TX_CALLCONVENTION OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState, TX_USERPARAM userParam)
{
	switch (connectionState)
	{
		case TX_CONNECTIONSTATE_CONNECTED: {
			BOOL success;
			success = txCommitSnapshotAsync(g_hGlobalInteractorSnapshot, OnSnapshotCommitted, NULL) == TX_RESULT_OK;

			break;
		}

		case TX_CONNECTIONSTATE_DISCONNECTED:
			break;

		case TX_CONNECTIONSTATE_TRYINGTOCONNECT:
			break;

		case TX_CONNECTIONSTATE_SERVERVERSIONTOOLOW:
			break;

		case TX_CONNECTIONSTATE_SERVERVERSIONTOOHIGH:
			break;
	}
}


void OnGazeDataEvent(TX_HANDLE hGazeDataBehavior)
{
	if (txGetGazePointDataEventParams(hGazeDataBehavior, &lastGazeData) == TX_RESULT_OK)
	{
		gazeAvailable = true;
	} else
	{
		gazeAvailable = false;
	}
}


void TX_CALLCONVENTION HandleEvent(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam)
{
	TX_HANDLE hEvent = TX_EMPTY_HANDLE;
	TX_HANDLE hBehavior = TX_EMPTY_HANDLE;

	txGetAsyncDataContent(hAsyncData, &hEvent);

	if (txGetEventBehavior(hEvent, &hBehavior, TX_BEHAVIORTYPE_GAZEPOINTDATA) == TX_RESULT_OK)
	{
		OnGazeDataEvent(hBehavior);
		txReleaseObject(&hBehavior);
	}

	txReleaseObject(&hEvent);
}
