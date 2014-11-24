// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cv.h"
#include "highgui.h"

extern void detectEyes(IplImage *img);

int _tmain(int argc, _TCHAR* argv[])
{
	/* usage: testFaceDetection <image> */
	assert(argc == 2);

	IplImage *img;

	size_t szConv = 0;
	char tcPath[256];
	::wcstombs_s (&szConv, tcPath, 256, argv[1], ::wcslen (argv[1]) + 1);

	/* load image */
	img = cvLoadImage(tcPath, 1);

	/* always check */
	assert(img);

	cvNamedWindow(tcPath, 1);

	/* detect eyes and display image */
	detectEyes(img);
	cvShowImage(tcPath, img);

	cvWaitKey(0);
	cvDestroyWindow(tcPath);
	cvReleaseImage(&img);

	return 0;
}

