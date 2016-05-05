#pragma once
#include <string>
#include <boost/numeric/interval.hpp>

class RawDataFile
{
	std::string name;
	int numOfScan;
	int msLevels;
	int &scanNumbers;
	boost::numberic::interval<double> mzRange;
	boost::numberic::interval<double> rtRange;
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
	int & getScanNumbers(int, boost::numberic::interval<double>);
	Scan getScan(int);
	boost::numberic::interval<double> getDataMZRange();
	boost::numberic::interval<double> getDataRTRange();
	boost::numberic::interval<double> getDataMZRange(int msLevel);
	boost::numberic::interval<double> getDataRTRange(int msLevel);
	double getDataMaxBasePeakIntensity(int msLevel);
	double getDataMaxTotalIonCurrent(int msLevel);
	void close();
};

