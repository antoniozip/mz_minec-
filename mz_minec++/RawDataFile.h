#pragma once
#include <string>
#include "Scan.h"
#include <boost/numeric/interval.hpp>

class RawDataFile
{
	std::string name;
	int numOfScan;
	int msLevels;
	int &scanNumbers;
	boost::numeric::interval<double> mzRange;
	boost::numeric::interval<double> rtRange;
	double maxBasePeakIntensity;
	double maxTotalIonCurrent;

public:
	RawDataFile(std::string);
	~RawDataFile();
	std::string getName();
	void setName(std::string);
	int getNumOfScans();
	int getNumOfScans(int);
	int & getMsLevels();
	int & getScanNumbers();
	int & getScanNumbers(int);
	int & getScanNumbers(int, boost::numeric::interval<double>);
	Scan getScan(int);
	boost::numeric::interval<double> getDataMZRange();
	boost::numeric::interval<double> getDataRTRange();
	boost::numeric::interval<double> getDataMZRange(int msLevel);
	boost::numeric::interval<double> getDataRTRange(int msLevel);
	double getDataMaxBasePeakIntensity(int msLevel);
	double getDataMaxTotalIonCurrent(int msLevel);
	void close();
};

