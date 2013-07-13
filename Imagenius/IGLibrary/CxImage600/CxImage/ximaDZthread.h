#pragma once

#include "ximathread.h"	// define base image thread

class CxImageDZThreadSampler : public CxImageThread
{
public:
	CxImageDZThreadSampler();
	virtual ~CxImageDZThreadSampler(void);

	virtual bool Start();
	virtual bool Exit();
	virtual void ThreadProc (IGLibrary::IGThreadMessage *pMessage);
	virtual void SetNbDZThreads (int nNbSamplingThreads, int nNbCroppingAndSavingSubThreads);

	inline int GetProgress() const {return m_nProgress;}
	inline void StepIt (int nSteps) {
		CxImageThread::StepIt (nSteps);
		if (m_nProgress == 100)
			m_nProgress = 0;
		m_nProgress += nSteps;
	}
private:
	inline bool isUpdateNeeded (const RECT& rcTile, const RECT& rcUpdate, const RECT& rcLastUpdate) const;
	int m_nProgress;
};

class CxImageDZCropAndSaveThread : public CxImageDZThreadSampler
{
public:
	CxImageDZCropAndSaveThread();
	virtual ~CxImageDZCropAndSaveThread(void);

	virtual bool Start();
	virtual bool Exit();
	virtual void ThreadProc (IGLibrary::IGThreadMessage *pMessage);	
};