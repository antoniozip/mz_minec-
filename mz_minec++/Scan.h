#pragma once
#include "RawDataFile.h"
#include "DataPoint.h"
#include <string>
#include <boost/numeric/interval.hpp>


class Scan
{
	RawDataFile dataFile;
	int scanNumber;
	std::string scanDefinition;
	int msLevel;
	double retentionTime;
	boost::numeric::interval<double> scanningMzRange;
	double precursorMz;
	char polarity;
	int precursorCharge;
	int & fragmentScanNumbers;
	DataPoint & dataPoints;
	DataPoint highestDataPoint;
public:
	Scan();
	~Scan();
	double getRetentionTime();
	double getPrecursorMz();
	int getScanNumber();
	DataPoint getHighestDataPoint();
	int getPrecursorCharge();
};

