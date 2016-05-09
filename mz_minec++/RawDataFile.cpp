#include "stdafx.h"
#include "RawDataFile.h"


RawDataFile::RawDataFile()
{
	name = "";
	
}

RawDataFile::RawDataFile(std::string str)
{
	name = str;

}


RawDataFile::~RawDataFile()
{
}

std::vector<int> RawDataFile::getScanNumbers(int, boost::numeric::interval<double>)
{
	return scanNumbers;
}

Scan RawDataFile::getScan(int scanNum)
{

}

boost::numeric::interval<double> RawDataFile::getDataMZRange()
{
	return mzRange;
}

boost::numeric::interval<double> RawDataFile::getDataRTRange()
{
	return rtRange;
}