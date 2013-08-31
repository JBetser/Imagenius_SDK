// added by TQ
#pragma once
#include "ximage.h"
#include <vector>
#include <list>

namespace IGLibrary
{
	class IGIrisDetection
	{
	public:
		IGIrisDetection(void);
		~IGIrisDetection(void);
		
		bool detect_iris (const CxImage& faceImg, RECT eye1, RECT eye2);
	};
}