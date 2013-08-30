#include "StdAfx.h"
#include "IGIrisDetection.h"
#include <vector>
#include <list>
#include "cv.h"

IGIrisDetection::IGIrisDetection(void)
{
}


IGIrisDetection::~IGIrisDetection(void)
{
}

cv::Point IGIrisDetection::getIrisCoor(const cv::Mat& inputEyeImg)
{
	cv::Mat srcGray;
	cv::Point center;
	cv::cvtColor(inputEyeImg, srcGray, CV_BGR2GRAY );
	cv::Mat tmp = cv::Mat(srcGray).clone();
	
	equalizeHist( srcGray, srcGray );
	
	// Initialization
    double minVal = 0;
    minMaxLoc(srcGray, &minVal, NULL, NULL, NULL);
	minVal =0;
	
	for (int i=0; i<=10; i=i+2)
	{	
		threshold(srcGray, tmp, minVal + i, 255, cv::THRESH_BINARY_INV);
 
		/*Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3), Point(1, 1));
		erode(tmp, tmp, element);
		dilate(tmp, tmp, element);*/

		//imshow("Source", tmp);
		vector<cv::Vec4i> hierarchy;
		vector<vector<cv::Point2i> > contours;
		findContours(tmp, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
 
		//find contour with max area
		int maxArea = 0;
		cv::Rect maxContourRect;
		for (int i=0; i<contours.size(); i++)
		{
			int area = cv::contourArea(contours[i]);
			cv::Rect rect = cv::boundingRect(contours[i]);
			double squareKoef = ((double) rect.width)/rect.height;
 
			//check if contour is like square (shape)
			#define SQUARE_KOEF 1.5
			if (area>maxArea && squareKoef < SQUARE_KOEF && squareKoef > 1.0/SQUARE_KOEF)
			{
				maxArea = area;
				//std::cout<<"area = "<<maxContourRect.width * maxContourRect.height<<endl;
				maxContourRect = rect;
				area =0;
			}
		}
 
		if (maxArea == 0)
		{
			//std::cout << "Iris not found!" << std::endl;
		}
		else
		{	
			//std::cout<<maxContourRect.width*4<<endl<<inputImg.rows<<endl<<inputImg.cols<<endl;
			if (maxContourRect.width*4 < inputEyeImg.rows && maxContourRect.width*4 > ( inputEyeImg.rows/3 ))
			{
				cv::Point center(maxContourRect.x+maxContourRect.width, maxContourRect.y+maxContourRect.height);			
				//cv::circle(inputEyeImg, center, maxContourRect.width*2, cv::Scalar( 0, 0, 255 ), -4, 8, 0);
				break;
			}
		}
		
	}
	return center;
}

bool IGIrisDetection::detect_iris (const CxImage& faceImg, RECT eye1, RECT eye2 )
{
	bool temp;
	cv::Rect eyeR1 (eye1.left, eye1.top, eye1.right-eye1.left, eye1.bottom-eye1.bottom); 
	cv::Rect eyeR2 (eye2.left, eye2.top, eye2.right-eye2.left, eye2.bottom-eye2.bottom); 
	// CxImage to IplImage
	CxImage imgRotated (faceImg);
    imgRotated.Rotate180();
	IplImage *faceImage = cvCreateImageHeader (cvSize (faceImg.GetWidth(), faceImg.GetHeight()), 8, faceImg.GetBpp() / 8);
    faceImage->imageDataOrigin = (char*)imgRotated.GetDIB();
    faceImage->imageData = (char*)imgRotated.GetBits();

    // IplImage to Mat
	cv::Mat matFaceImg=cv::cvarrToMat(faceImage);
    cv::Mat eyeRegion1, eyeRegion2;

	if (!matFaceImg.data)
	{
		temp =1;
		// Eye Region 1
		eyeRegion1 = matFaceImg(eyeR1);

		// Eye Region 2
		eyeRegion2 = matFaceImg(eyeR2);

		// iris detection
		cv::Point eyeCoor1 = getIrisCoor(eyeRegion1);
		cv::Point eyeCoor2 = getIrisCoor(eyeRegion2);
	}
	else
	{
		temp =0;
	}
    
    
    return temp;
}
