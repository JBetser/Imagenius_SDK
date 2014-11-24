#pragma once

class IGAutoKill
{
public:
#ifdef DEBUG
	static const int MAX_LIVE_TIME_SEC = 30;
#else
	static const int MAX_LIVE_TIME_SEC = 14400; // recycle the process every 4 hours when idle
#endif

	IGAutoKill();
	void Check();
	void Snooze();
	static void Kill();
	static bool& IsDead();

private:
	SYSTEMTIME _startTime;
};

ULARGE_INTEGER operator-(const SYSTEMTIME& pSr,const SYSTEMTIME& pSl);