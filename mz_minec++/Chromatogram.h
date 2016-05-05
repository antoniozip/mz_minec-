#pragma once
#include <unordered_map>
#include <vector>
#include "DataPoint.h"
#include "RawDataFile.h"
#include <climits>
#include <fstream>
#include <vector>
#include <boost/numeric/interval.hpp>

class Chromatogram
{
	RawDataFile dataFile;
	std::unordered_map<int, DataPoint> dataPointsMap;
	double mz, rt, height, area;
	double fwhm, tf, af;
	int representativeScan = -1;
	int fragmentScan = -1;
	std::vector<int> buildingSegment;
	DataPoint lastMzPeak;
	int numOfCommittedSegments = 0;
	int charge = 0;
	int minScan = INT_MAX;
	int maxScan = 0;
	double minTime = 0;
	double maxTime = 0;
	double mzSum = 0;
	int mzN = 0;
public:
	Chromatogram(std::string);
	~Chromatogram();
	void addMzPeak(int , DataPoint );
	DataPoint getDataPoint(int );
	DataPoint getLastMzPeak();
	double getMz();
	double getArea();
	double getHeight();
	int Chromatogram::getMostIntenseFragmentScanNumber();
	double getRT();
	boost::numeric::interval<double> getRawDataPointsIntensityRange();
	boost::numeric::interval<double> getRawDataPointsMZRange();
	boost::numeric::interval<double> getRawDataPointsRTRange();
	int getRepresentativeScanNumber();
	int & getScanNumbers();
	RawDataFile getDataFile();
	double calcMedian(std::vector<int>);
	void finishChromatogram();
};

