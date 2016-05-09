#pragma once
#include "RawDataFile.h"
#include "DataPoint.h"
#include <string>
#include <boost/numeric/interval.hpp>
#include <vector>


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
	std::vector<int> fragmentScanNumbers;
	std::vector<DataPoint> dataPoints;
	DataPoint highestDataPoint;
public:
	Scan(std::string);
	~Scan();
	double getRetentionTime();
	double getPrecursorMz();
	int getScanNumber();
	DataPoint getHighestDataPoint();
	int getPrecursorCharge();
};

