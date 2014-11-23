#include <Ice/Ice.h>
#include <IGFaceDetectorIceI.h>
#include <IGFaceDetection.h>
#include <time.h>

#define SEMAPHORE_NAME	L"ImageniusSemaphore"
#ifdef DEBUG
#define SEMAPHORE_MAX_NBPEBBLES	3
#define CONFIG_CLIENT_PATH	"C:\\Imagenius\\Testing\\IGServer\\IGFaceDetector\\config.client"
#define FACEDETECTOR_WAIT_TIMEOUT	30000	// 30s time-out
#else
#define SEMAPHORE_MAX_NBPEBBLES	12
#define CONFIG_CLIENT_PATH	"C:\\Imagenius\\IGServer\\IGFaceDetector\\config.client"
#define FACEDETECTOR_WAIT_TIMEOUT	10000	// 10s time-out
#endif
#define FACEDETECTOR_NBTASKS	7

using namespace std;
using namespace Ice;
using namespace IGFaceDetector;

IGFaceDetectorIceI::IGFaceDetectorIceI(const string& serviceName) : _serviceName(serviceName)
																	, _nCurRoundRobin(0)
																	, _hThread (NULL)
{
	PropertiesPtr pProp = createProperties();
	pProp->load (CONFIG_CLIENT_PATH);
	InitializationData initData;
	initData.properties = pProp;
	CommunicatorPtr pCom = initialize (initData);

	// initialize critical section
	::InitializeCriticalSection (&_csProxy);

	// initialize ice proxies
	_tProxies[0] = IGFaceDetectorIcePrx::uncheckedCast(pCom->propertyToProxy("IGFaceDetectorIce.Proxy0"));
	_tProxies[1] = IGFaceDetectorIcePrx::uncheckedCast(pCom->propertyToProxy("IGFaceDetectorIce.Proxy1"));
	_tProxies[2] = IGFaceDetectorIcePrx::uncheckedCast(pCom->propertyToProxy("IGFaceDetectorIce.Proxy2"));
	_tProxies[3] = IGFaceDetectorIcePrx::uncheckedCast(pCom->propertyToProxy("IGFaceDetectorIce.Proxy3"));
	_tProxies[4] = IGFaceDetectorIcePrx::uncheckedCast(pCom->propertyToProxy("IGFaceDetectorIce.Proxy4"));
	_tProxies[5] = IGFaceDetectorIcePrx::uncheckedCast(pCom->propertyToProxy("IGFaceDetectorIce.Proxy5"));
	_tProxies[6] = IGFaceDetectorIcePrx::uncheckedCast(pCom->propertyToProxy("IGFaceDetectorIce.Proxy6"));
	_tProxies[7] = IGFaceDetectorIcePrx::uncheckedCast(pCom->propertyToProxy("IGFaceDetectorIce.Proxy7"));
	_tProxies[8] = IGFaceDetectorIcePrx::uncheckedCast(pCom->propertyToProxy("IGFaceDetectorIce.Proxy8"));
	_tProxies[9] = IGFaceDetectorIcePrx::uncheckedCast(pCom->propertyToProxy("IGFaceDetectorIce.Proxy9"));
	_tProxies[10] = IGFaceDetectorIcePrx::uncheckedCast(pCom->propertyToProxy("IGFaceDetectorIce.Proxy10"));
	_tProxies[11] = IGFaceDetectorIcePrx::uncheckedCast(pCom->propertyToProxy("IGFaceDetectorIce.Proxy11"));
	_tProxies[12] = IGFaceDetectorIcePrx::uncheckedCast(pCom->propertyToProxy("IGFaceDetectorIce.Proxy12"));
	for (int idxProxy = 1; idxProxy <= SEMAPHORE_MAX_NBPEBBLES; idxProxy++)
		releaseProxy (_tProxies[idxProxy]);

	// create synchronization object
	_hStartEvent = ::CreateEventW(0, TRUE, FALSE, L"");
}

IGFaceDetectorIceI::~IGFaceDetectorIceI()
{
	::CloseHandle (_hStartEvent);
	if (_hThread != NULL)
		::CloseHandle (_hThread);
	::DeleteCriticalSection (&_csProxy);
}

DWORD IGFaceDetectorIceI::ThreadProc (LPVOID lpParam)
{
	wchar_t **pParams = (wchar_t **)lpParam;	

	::SetEvent (_hStartEvent);
	{
		SmartLock lock (&_csProxy);		

		int angle = ::_wtoi(pParams[2]);
		int nProxyId;
		::sscanf_s (_serviceName.c_str(), "IGFaceDetectorIceBox%d", &nProxyId);

		int nbFaces = 0;
		int score = 0;
		bool isCancelled = false;
		if (_tProxies[0]->faceDetectionStarted (pParams[0], nProxyId, angle))			
			detectFaces (pParams[0], pParams[1], angle, &nbFaces, &score);
		else
			isCancelled = true;	
		_tProxies[0]->faceDetectionFinished (pParams[0], nProxyId, nbFaces, angle, score, isCancelled);
	}
	delete [] pParams[0];
	delete [] pParams[1];
	delete [] pParams[2];
    return TRUE;
}

DWORD WINAPI IGFaceDetectorIceI::stcThreadProc (LPVOID lpParam)
{
	wchar_t **pParams = (wchar_t **)lpParam;
	IGFaceDetectorIceI *pThis = (IGFaceDetectorIceI *)pParams[3];
	return pThis->ThreadProc (lpParam);
}

void IGFaceDetectorIceI::faceDetection(const wstring& pictureId, const wstring& pictureName, int rotationIndex, bool bSemaphore, const Ice::Current&)
{
	if (bSemaphore)
	{
		getProxy()->faceDetection (pictureId.c_str(), pictureName.c_str(), rotationIndex, false);		
	}
	else
	{
		DWORD dwThreadID;
		wchar_t **pParams = new wchar_t*[4];
		pParams[0] = new wchar_t[MAX_PATH];
		pParams[1] = new wchar_t[MAX_PATH];
		pParams[2] = new wchar_t[32];
		pParams[3] = (wchar_t*)this;
		::wcscpy_s (pParams[0], MAX_PATH, pictureId.c_str());
		::wcscpy_s (pParams[1], MAX_PATH, pictureName.c_str());
		::_itow_s (rotationIndex, pParams[2], 32, 10);
		if (_hThread != NULL)
			::CloseHandle (_hThread);
		{
			SmartLock lock (&_csProxy);
			_hThread = ::CreateThread (NULL,       // default security attributes
											 0,          // default stack size
											 (LPTHREAD_START_ROUTINE) IGFaceDetectorIceI::stcThreadProc, 
											 pParams,       // no thread function arguments
											 0,          // default creation flags
											 &dwThreadID); // receive thread identifier	

			if (::WaitForSingleObject (_hStartEvent, FACEDETECTOR_WAIT_TIMEOUT) != WAIT_OBJECT_0)
				printf("IGFaceDetectorIce synchronization error: %d\n", GetLastError());
			::ResetEvent (_hStartEvent);
		}
	}
}

bool IGFaceDetectorIceI::faceDetectionStarted (const wstring& pictureId, int proxyId, int angle, const Ice::Current&)
{
	{
		SmartLock lock (&_csProxy);
		int nCurProgess = _mapDetectionProgress.find(pictureId) == _mapDetectionProgress.end() ? 1 : _mapDetectionProgress[pictureId].first + 1;
		int nCurScore = nCurProgess > 1 ? _mapDetectionProgress[pictureId].second : 0;
		if (nCurScore > 90 && abs (angle) > 75)
			return false;
		if (_setFreeProxies.find (_tProxies [proxyId]) != _setFreeProxies.end()){
			_setFreeProxies.erase (_tProxies [proxyId]);
			_setBusyProxies.insert (_tProxies [proxyId]);
		}
		return true;
	}
}

void IGFaceDetectorIceI::faceDetectionFinished(const wstring& pictureId, int proxyId, int nbFaces, int angle, int score, bool isCancelled, const Ice::Current&)
{
	{
		SmartLock lock (&_csProxy);
		releaseProxy (_tProxies [proxyId]);

		int nCurProgess = _mapDetectionProgress.find(pictureId) == _mapDetectionProgress.end() ? 1 : _mapDetectionProgress[pictureId].first + 1;
		int nCurScore = nCurProgess > 1 ? _mapDetectionProgress[pictureId].second : 0;
		_mapDetectionProgress[pictureId] = pair <int, int> (nCurProgess, nCurScore + score);

		char tcPicId [64];
		::WideCharToMultiByte (CP_ACP, 0, pictureId.c_str(), -1, tcPicId, pictureId.length() + 1, NULL, NULL);
		time_t mytime = time(0);
		tm rTime;
		localtime_s (&rTime, &mytime);
		char tcTimeBuf[32];			
		asctime_s (tcTimeBuf, &rTime);
		if (isCancelled)
			::printf("=========================\n%sProxy no%d, angle: %d, cancelled, progress: %d/%d\non picture id: %s\n", tcTimeBuf, proxyId, angle, nCurProgess, FACEDETECTOR_NBTASKS, tcPicId);
		else			
			::printf("=========================\n%sProxy no%d detected %d faces, angle: %d, score: %d, progress: %d/%d\non picture id: %s\n", tcTimeBuf, proxyId, nbFaces, angle, score, nCurProgess, FACEDETECTOR_NBTASKS, tcPicId);

		if (nCurProgess == FACEDETECTOR_NBTASKS){
			_mapDetectionProgress.erase (pictureId);
			wstring wsEvent (L"Global\\");
			wsEvent += pictureId;
			HANDLE hEndProcessingEvent = ::OpenEventW (EVENT_MODIFY_STATE, FALSE, wsEvent.c_str());
			if (hEndProcessingEvent) {
				::SetEvent (hEndProcessingEvent);
				::CloseHandle (hEndProcessingEvent);
			}
			else
				::printf("Error in method faceDetectionFinished: OpenEvent failed\nPicture id: %s, Error: %d\n", tcPicId, ::GetLastError());
		}
	}
}

void IGFaceDetectorIceI::releaseProxy (IGFaceDetector::IGFaceDetectorIcePrx prx)
{	
	{
		SmartLock lock (&_csProxy);
		if (_setBusyProxies.find (prx) != _setBusyProxies.end())
			_setBusyProxies.erase (prx);
		_setFreeProxies.insert (prx);
	}
}

IGFaceDetectorIcePrx IGFaceDetectorIceI::getProxy()
{
	{
		SmartLock lock (&_csProxy);
		if (_setFreeProxies.empty()){
			set<IGFaceDetectorIcePrx>::iterator itPrx = _setBusyProxies.begin();
			for (int idxProxy = 0; idxProxy < _nCurRoundRobin; idxProxy++)
				++itPrx;
			if (++_nCurRoundRobin >= SEMAPHORE_MAX_NBPEBBLES)
				_nCurRoundRobin = 0;
			return *itPrx;
		}
		else
		{
			IGFaceDetectorIcePrx freeProxy = *_setFreeProxies.begin();
			_setFreeProxies.erase (freeProxy);
			_setBusyProxies.insert (freeProxy);
			_nCurRoundRobin = 0;
			return freeProxy;
		}		
	}
}