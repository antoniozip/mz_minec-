#include "stdafx.h"
#include "RawDataFile.h"


RawDataFile::RawDataFile(std::string str)
{
	name = str;
}


RawDataFile::~RawDataFile()
{
}

int & RawDataFile::getScanNumbers(int, boost::numeric::interval<double>)
{
	int r = 0;
	return &r;
}