#include "stdafx.h"
#include "area.h"

//  Public-domain function by Darel Rex Finley, 2006.
double polygonArea(double *X, double *Y, int points) {
	double  area=0.;
	int     i, j=points-1;
	for (i=0; i<points; i++) {
	  area+=(X[j]+X[i])*(Y[j]-Y[i]); j=i; 
	}
	return area*.5;
}