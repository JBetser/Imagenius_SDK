#pragma once
#include <IGFaceDetectorIce.h>

#include <set>
#include <map>

class IGFaceDetectorIceI : public IGFaceDetector::IGFaceDetectorIce
{
public:

    IGFaceDetectorIceI(const std::string&);
	virtual ~IGFaceDetectorIceI();

	virtual void faceDetection (const std::wstring& pictureId, const std::wstring& pictureName, int rotationIndex, bool bSemaphore, const Ice::Current&);	
	virtual bool faceDetectionStarted (const std::wstring& pictureId, int proxyId, int angle, const Ice::Current&);
	virtual void faceDetectionFinished (const std::wstring& pictureId, int proxyId, int nbFaces, int angle, int score, bool isCancelled, const Ice::Current&);

private:
	void releaseProxy (IGFaceDetector::IGFaceDetectorIcePrx prx);
	IGFaceDetector::IGFaceDetectorIcePrx getProxy();
	DWORD ThreadProc (LPVOID lpParam);
	static DWORD WINAPI stcThreadProc (LPVOID lpParam);
	
    const std::string _serviceName;
	IGFaceDetector::IGFaceDetectorIcePrx _tProxies[13];
	std::set <IGFaceDetector::IGFaceDetectorIcePrx> _setFreeProxies;
	std::set <IGFaceDetector::IGFaceDetectorIcePrx> _setBusyProxies;
	std::map <std::wstring, std::pair <int, int>> _mapDetectionProgress;
	CRITICAL_SECTION _csProxy;
	HANDLE _hStartEvent;
	HANDLE _hThread;
	int _nCurRoundRobin;
};

class SmartLock
{
public:

	SmartLock(LPCRITICAL_SECTION sect)
	:m_pLock(sect)
	{
		::EnterCriticalSection(m_pLock);
	};

	~SmartLock()
	{
		::LeaveCriticalSection( m_pLock );
	};

private:
	LPCRITICAL_SECTION m_pLock;
};