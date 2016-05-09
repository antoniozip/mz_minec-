#include "stdafx.h"
#include "Scan.h"


Scan::Scan(std::string s)
{
	dataFile = RawDataFile(s);
}


Scan::~Scan()
{
}

double Scan::getRetentionTime()
{
	return retentionTime;
}

double Scan::getPrecursorMz()
{
	return precursorMz;
}

int Scan::getScanNumber()
{
	return scanNumber;
}

DataPoint Scan::getHighestDataPoint()
{
	return highestDataPoint;
}

int Scan::getPrecursorCharge()
{
	
}