#include "stdafx.h"
#include "IGRandomGenerator.h"
#include "IGException.h"

using namespace std;
using namespace IGLibrary;

IGException RandomGeneratorException ("IGSmartLayer");

IGRandomGenerator::IGRandomGenerator(int nSize) : m_nSize (nSize)
{
	m_pNoise = new double[m_nSize * m_nSize];
    for (int y = 0; y < m_nSize; y++){
		for (int x = 0; x < m_nSize; x++){
			m_pNoise[y * m_nSize + x] = (rand() % 32768) / 32768.0;
		}
	}
}

IGRandomGenerator::~IGRandomGenerator()
{
	delete [] m_pNoise;
}

double IGRandomGenerator::smoothNoise(double x, double y)
{  
   //get fractional part of x and y
   double fractX = x - int(x);
   double fractY = y - int(y);
   
   //wrap around
   int x1 = (int(x) + m_nSize) % m_nSize;
   int y1 = (int(y) + m_nSize) % m_nSize;
   
   //neighbor values
   int x2 = (x1 + m_nSize - 1) % m_nSize;
   int y2 = (y1 + m_nSize - 1) % m_nSize;

   //smooth the noise with bilinear interpolation
   double value = 0.0;
   value += fractX       * fractY       * m_pNoise[y1 * m_nSize + x1];
   value += fractX       * (1 - fractY) * m_pNoise[y2 * m_nSize + x1];
   value += (1 - fractX) * fractY       * m_pNoise[y1 * m_nSize + x2];
   value += (1 - fractX) * (1 - fractY) * m_pNoise[y2 * m_nSize + x2];

   return value;
}

double IGRandomGenerator::turbulence(double x, double y, int initSize)
{
    double value = 0.0, size = initSize;
    
    while (size >= 1)
    {
        value += smoothNoise(x / size, y / size) * size;
        size /= 2.0;
    }
    
    return(128.0 * value / initSize);
}