#include "stdafx.h"
#include "DataPoint.h"


DataPoint::DataPoint()
{
}


DataPoint::~DataPoint()
{
}

double DataPoint::getMz() {
	return Mz;
}

double DataPoint::getIntensity() {
	return intensity;
}