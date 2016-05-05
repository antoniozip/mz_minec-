#include "stdafx.h"
#include "Chromatogram.h"
#include "RawDataFile.h"
#include "ScanUtils.h"
#include <iostream>
#include <string>
#include <array>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>
#include <boost/numeric/interval.hpp>

Chromatogram::Chromatogram(std::string st)
{
	dataFile = RawDataFile(st);
	dataPointsMap = std::unordered_map<int,DataPoint>(1024);
	buildingSegment = std::vector<int>(128);
}


Chromatogram::~Chromatogram()
{
}

void Chromatogram::addMzPeak(int scanNumber, DataPoint mzValue)
{
	dataPointsMap.emplace(scanNumber, mzValue);
	lastMzPeak = mzValue;
	mzSum += mzValue.getMz();
	mzN++;
	mz = mzSum / mzN;
	buildingSegment.push_back(scanNumber);

	if (scanNumber < minScan) {
		minScan = scanNumber;
		minTime = dataFile.getScan(minScan).getRetentionTime();
	}
	if (scanNumber > maxScan) {
		maxScan = scanNumber;
		maxTime = dataFile.getScan(maxScan).getRetentionTime();
	}
	rt = (maxTime + minTime) / 2;
}

DataPoint Chromatogram::getDataPoint(int scanNumber) {
	return dataPointsMap[scanNumber];
}

DataPoint Chromatogram::getLastMzPeak() {
	return lastMzPeak;
}

double Chromatogram::getMz( ) {
	return mz;
}

double Chromatogram::getArea() {
	return area;
}

double Chromatogram::getHeight() {
	return height;
}

int Chromatogram::getMostIntenseFragmentScanNumber() {
	return fragmentScan;
}

double Chromatogram::getRT() {
	return rt;
}

boost::numeric::interval<double> Chromatogram::getRawDataPointsIntensityRange() {
	return rawDataPointsIntensityRange;
}

boost::numeric::interval<double> Chromatogram::getRawDataPointsMZRange() {
	return rawDataPointsMZRange;
}

boost::numeric::interval<double> Chromatogram::getRawDataPointsRTRange() {
	return rawDataPointsRTRange;
}

int Chromatogram::getRepresentativeScanNumber() {
	return representativeScan;
}

int & Chromatogram::getScanNumbers() {
	return dataFile.getScanNumbers(1);
}

RawDataFile Chromatogram::getDataFile() {
	return dataFile;
}

double Chromatogram::calcMedian(std::vector<double> scores)
{
	double median;
	std::size_t size = scores.size();

	if (size % 2 == 0)
	{
		median = (scores[size / 2 - 1] + scores[size / 2]) / 2;
	}
	else
	{
		median = scores[size / 2];
	}

	return median;
}

void Chromatogram::finishChromatogram() {

	std::vector<int> allScanNumbers;
	for (std::unordered_map<int, DataPoint>::iterator it = dataPointsMap.begin(); it != dataPointsMap.end(); ++it)
		allScanNumbers.push_back(it->first);
		
	std::sort(allScanNumbers.begin(), allScanNumbers.end());

	
	std::vector<double> allMzValues;
	for (int i = 0; i < allScanNumbers.size(); i++) 
	{
		allMzValues.push_back(dataPointsMap.[allScanNumbers[i]].getMZ());
	}
	mz = calcMedian(allMzValues);

	// Update raw data point ranges, height, rt and representative scan
	height = std::numeric_limits<double>::lowest();
	for (int i = 0; i < allScanNumbers.size(); i++) {

		DataPoint mzPeak = dataPointsMap[allScanNumbers[i]];

		// Replace the MzPeak instance with an instance of SimpleDataPoint,
		// to reduce the memory usage. After we finish this Chromatogram, we
		// don't need the additional data provided by the MzPeak

		dataPointsMap.emplace(allScanNumbers[i], mzPeak);

		if (i == 0) {
			rawDataPointsIntensityRange = boost::numeric::interval<double>(mzPeak.getIntensity());
			rawDataPointsMZRange = boost::numeric::interval<double>(mzPeak.getMz());
		}
		else {
			rawDataPointsIntensityRange = rawDataPointsIntensityRange.hull(rawDataPointsIntensityRange.lower(), mzPeak.getIntensity());
			rawDataPointsMZRange = rawDataPointsMZRange.hull(rawDataPointsMZRange.lower(), mzPeak.getMz());
		}

		if (height < mzPeak.getIntensity()) {
			height = mzPeak.getIntensity();
			rt = dataFile.getScan(allScanNumbers[i]).getRetentionTime();
			representativeScan = allScanNumbers[i];
		}
	}

	// Update area
	area = 0;
	for (int i = 1; i < allScanNumbers.size(); i++) {
		// For area calculation, we use retention time in seconds
		double previousRT = dataFile.getScan(allScanNumbers[i - 1]).getRetentionTime() * 60.000000;
		double currentRT = dataFile.getScan(allScanNumbers[i]).getRetentionTime() * 60.000000;
		double previousHeight = dataPointsMap[(allScanNumbers[i - 1])].getIntensity();
		double currentHeight = dataPointsMap[(allScanNumbers[i])].getIntensity();
		area += (currentRT - previousRT) * (currentHeight + previousHeight)/ 2;
	}

	// Update fragment scan
	ScanUtils su = ScanUtils();
	fragmentScan = su.findBestFragmentScan(dataFile,dataFile.getDataRTRange(1), rawDataPointsMZRange);

	if (fragmentScan > 0) {
		Scan fragmentScanObject = dataFile.getScan(fragmentScan);
		int precursorCharge = fragmentScanObject.getPrecursorCharge();
		if (precursorCharge > 0)
			charge = precursorCharge;
	}

	// Victor Treviño
	// using allScanNumbers : rawDataPointsRTRange = new
	// Range(dataFile.getScan(allScanNumbers[0]).getRetentionTime(),
	// dataFile.getScan(allScanNumbers[allScanNumbers.length-1]).getRetentionTime());
	rawDataPointsRTRange = boost::numeric::interval<double>(minTime, maxTime); // using the
														 
	//buildingSegment = NULL;
	//lastMzPeak = NULL;
}

double Chromatogram::getBuildingSegmentLength() {
	if (buildingSegment.size() < 2)
		return 0;
	int firstScan = buildingSegment.at(1);
	int lastScan = buildingSegment.at(buildingSegment.size());
	double firstRT = dataFile.getScan(firstScan).getRetentionTime();
	double lastRT = dataFile.getScan(lastScan).getRetentionTime();
	return (lastRT - firstRT);
}

double Chromatogram::getBuildingFirstTime() {
	return minTime;
}

double Chromatogram::getBuildingLastTime() {
	return maxTime;
}

int Chromatogram::getNumberOfCommittedSegments() {
	return numOfCommittedSegments;
}

void Chromatogram::removeBuildingSegment() {
	for (int scanNumber : buildingSegment)
		dataPointsMap.erase(scanNumber);
	buildingSegment.clear();
}

void Chromatogram::commitBuildingSegment() {
	buildingSegment.clear();
	numOfCommittedSegments++;
}

//TO BE REVISED
void Chromatogram::addDataPointsFromChromatogram(Chromatogram ch) {
	std::unordered_map<int, DataPoint> dPM = ch.dataPointsMap;
	for (std::unordered_map<int, DataPoint>::iterator it = dPM.begin(); it != dPM.end(); ++it)
		addMzPeak(it->first,dPM[it->first]);
}

int Chromatogram::getCharge() {
	return charge;
}

void Chromatogram::setCharge(int charge) {
	charge = charge;
}

double Chromatogram::getFWHM() {
	return fwhm;
}

void Chromatogram::setFWHM(double fwhm) {
	fwhm = fwhm;
}

double Chromatogram::getTailingFactor() {
	return tf;
}

void Chromatogram::setTailingFactor(double tf) {
	tf = tf;
}

double Chromatogram::getAsymmetryFactor() {
	return af;
}

void Chromatogram::setAsymmetryFactor(double af) {
	af = af;
}