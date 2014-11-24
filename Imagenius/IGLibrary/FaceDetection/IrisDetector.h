#ifndef IrisDetector_h
#define IrisDetector_h

#include <list>
#include <iostream>

class CxImage;

void detect_iris (const CxImage& faceImg, RECT eye1, RECT eye2, POINT & ptEye1, int& ray1, POINT & ptEye2, int& ray2);

#endif IrisDetector_h

