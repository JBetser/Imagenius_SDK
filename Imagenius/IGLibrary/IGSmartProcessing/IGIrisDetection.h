// added by TQ
#pragma once
#include "IGSmartLayer.h"
#include <vector>
#include <list>
#include "cv.h"


class IGIrisDetection
{
public:
	IGIrisDetection(void);
	~IGIrisDetection(void);
	
	cv::Point getIrisCoor(const cv::Mat& inputEyeImg);
	bool detect_iris (const CxImage& faceImg, RECT eye1, RECT eye2 );
};

