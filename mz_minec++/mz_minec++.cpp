// mz_minec++.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include "Chromatogram.h"

/*
* To change this license header, choose License Headers in Project Properties.
* To change this template file, choose Tools | Templates
* and open the template in the editor.
*/

/*
* File:   main.cpp
* Author: Antonio
*
* Created on May 6, 2016, 4:37 PM
*/
#pragma once
//#include <cstdlib>
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <boost/unordered_set.hpp>
#include <vector>
#include <climits>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
using namespace std;

typedef pair<double, double> datap;

class Chromatogram
{
	//map<int, datap> dataPointsMap;
	double mz, rt, height, area;
	double fwhm, tf, af;
	int representativeScan = -1;
	int fragmentScan = -1;
	vector<int> buildingSegment;
	datap lastMzPeak;
	int numOfCommittedSegments = 0;
	int charge = 0;
	int minScan = INT_MAX;
	int maxScan = 0;
	double minTime = 0;
	double maxTime = 0;
	double mzSum = 0;
	int mzN = 0;
	map<int, double> scan_rt;
	bool virgin;
public:
	map<int, datap> dataPointsMap;
	Chromatogram() { virgin = false; }
	Chromatogram(map<int, double> mrt) { setRTMap(mrt); virgin = 1; }
	~Chromatogram() {}
	void setRTMap(map<int, double> mrt) { scan_rt = mrt; }
	datap getLastMzPeak() { return lastMzPeak; }
	bool getVirgin() { return virgin; }
	//map<int, datap> getDataPoints() {return dataPointsMap;}
	void addMzPeak(int scan_num, datap mzz)
	{
		dataPointsMap.insert(pair<int, datap>(scan_num, mzz));
		lastMzPeak = mzz;
		mzSum += mzz.first;
		mzN++;
		mz = mzSum / mzN;
		buildingSegment.push_back(scan_num);
		if (scan_num < minScan) {
			minScan = scan_num;
			minTime = scan_rt.at(minScan);
			//minTime = dataFile.getScan(minScan).getRetentionTime();
		}
		if (scan_num > maxScan) {
			maxScan = scan_num;
			maxTime = scan_rt.at(maxScan);
			//maxTime = dataFile.getScan(maxScan).getRetentionTime();
		}
		rt = (maxTime + minTime) / 2;
	}
	double getBuildingSegmentLength() {
		if (buildingSegment.size() < 2)
			return 0;
		int firstScan = buildingSegment.at(0);
		int lastScan = buildingSegment.at(buildingSegment.size() - 1);
		//double firstRT = dataFile.getScan(firstScan).getRetentionTime();
		double firstRT = scan_rt.at(firstScan);
		//double lastRT = dataFile.getScan(lastScan).getRetentionTime();
		double lastRT = scan_rt.at(lastScan);
		return (lastRT - firstRT);
	}
	void commitBuildingSegment()
	{
		buildingSegment.clear();
		numOfCommittedSegments++;
	}
	void removeBuildingSegment()
	{
		for (vector<int>::iterator scanNumber = buildingSegment.begin(); scanNumber != buildingSegment.end(); scanNumber++)
		{
			dataPointsMap.erase(*scanNumber);
		}
		buildingSegment.clear();
	}
	int getNumberOfCommittedSegments() {
		return numOfCommittedSegments;
	}
	friend bool operator< (const Chromatogram &left, const Chromatogram &right)
	{
		return left.lastMzPeak.first < right.lastMzPeak.first;
	}
};


/*
*
*/
int main()
{
	ifstream in;
	ofstream out;
	string line;
	in.open("C:\\Users\\antonio\\Documents\\generalParameters.txt");
	out.open("C:\\Users\\antonio\\Documents\\chromatogram_lists.txt");
	int scan_id = 0;
	double minimumTimeSpan = 0.0;
	double retentionTime;
	cout << "Start Acquisition " << endl;
	multimap<int, datap> raw;
	map<int, double> scan_rt;
	int n_line = 1;
	double intensity = 0;
	double mzPeak = 0;
	while (getline(in, line))
	{
		int pos = line.find(",");
		//cout << "n_line: " << n_line++ <<endl;
		n_line++;
		try
		{
			if (pos == -1)
			{
				int pos2 = line.find(";");
				string tmp = line.substr(5, pos2 - 1);
				scan_id = boost::lexical_cast<int>(tmp);
				//sscanf(tmp.c_str(), "%d", &scan_id);
				size_t idx = line.find("\r");
				//cout <<"line length " <<line.length() <<endl; 
				string tmp2 = line.substr(pos2 + 1, idx - 1);
				boost::trim(tmp2);
				retentionTime = boost::lexical_cast<double>(tmp2);
				scan_rt.insert(pair<int, double>(scan_id, retentionTime));
				//cout <<"Scan number: " << scan_id <<endl;
			}
			else
			{
				string tmp = line.substr(0, pos - 1);
				intensity = boost::lexical_cast<double>(tmp);
				//cout <<"Intensity:" << tmp << '\n';
				//cout <<"Intensity:" << intensity << '\n';
				tmp = line.substr(pos + 1, line.length() - 1);
				//cout <<"M/Z:" << tmp << '\n';
				int idx = tmp.find_first_of("\r");
				tmp = tmp.substr(0, idx - 1);
				mzPeak = boost::lexical_cast<double>(tmp);
				//cout <<"M/Z:" << mzPeak << '\n';
				datap dp(mzPeak, intensity);
				raw.insert(pair<int, datap>(scan_id, dp));

			}
		}
		catch (boost::bad_lexical_cast const&)
		{
			intensity = 0;
			mzPeak = 0;
			cout << "Error! n_line: " << n_line << endl;
		}
	}
	in.close();

	cout << "Acquisition ends!" << endl;
	multimap<int, datap>::iterator mit = raw.begin();
	set<Chromatogram> buildingChromatograms;
	double ppmTolerance = 80.0;
	double mzTolerance = 0.001;
	double million = 1000000.0;
	for (int j = 1; j <= scan_id; j++)
	{

		cout << "Scan n. " << j << endl;
		set<Chromatogram> connectedChromatograms;
		int len = raw.count(j);
		//cout <<"DataPoint for this scan: " <<len <<endl;
		for (int k = 1; k <= len; k++)
		{
			cout << "DataPoint n. " << k << endl;
			datap mzPeak = (*mit).second;
			mit++;
			Chromatogram bestChromatogram;
			BOOST_FOREACH(Chromatogram test, buildingChromatograms)
			{
				//for (set<Chromatogram>::iterator ic = buildingChromatograms.begin(); ic != buildingChromatograms.end(); ++ic)
				//{
				//Chromatogram test = *ic;
				datap lastMzPeak = test.getLastMzPeak();
				double testMz = lastMzPeak.second;
				double absolute_tolerance = max(mzTolerance, testMz / million * ppmTolerance);
				double tolerance_upper = testMz + absolute_tolerance;
				double tolerance_lower = testMz - absolute_tolerance;
				if (tolerance_upper > mzPeak.second && tolerance_lower < mzPeak.second)
				{
					if (bestChromatogram.getVirgin() || lastMzPeak.first > bestChromatogram.getLastMzPeak().first)
						bestChromatogram = test;
				}
			}

			if (!bestChromatogram.getVirgin())
			{
				if (connectedChromatograms.count(bestChromatogram) != 0)
				{
					continue;
				}
				else {
					bestChromatogram = Chromatogram(scan_rt);
				}
			}

			bestChromatogram.addMzPeak(scan_id, mzPeak);
			connectedChromatograms.insert(bestChromatogram);
		}
		int count = 1;
		BOOST_FOREACH(Chromatogram test, buildingChromatograms)
		{
			//Chromatogram test = *ic;
			cout << "Inspection of Chromatogram n " << count << endl;
			count++;
			cout << "Check 1\n";
			if (connectedChromatograms.count(test) > 0)
			{
				continue;
			}
			cout << "Check 2\n";
			//if (count == 88)
			//        cout <<"Hi\n"; 
			if (test.getBuildingSegmentLength() >= minimumTimeSpan)
			{
				test.commitBuildingSegment();
				connectedChromatograms.insert(test);
				continue;
			}

			cout << "Check 3\n";
			// Check if we have any committed segments in the chromatogram
			if (test.getNumberOfCommittedSegments() > 0)
			{
				test.removeBuildingSegment();
				connectedChromatograms.insert(test);
				continue;
			}
		}
		buildingChromatograms = connectedChromatograms;
		cout << "Built n. " << buildingChromatograms.size() << " Chromatograms!" << endl;
	}
	cout << "Final Built n. " << buildingChromatograms.size() << " Chromatograms!" << endl;
	int id_chromatogram = 1;
	for (set<Chromatogram>::iterator ci = buildingChromatograms.begin(); ci != buildingChromatograms.end(); ci++)
	{
		map<int, datap> dpm = ci->dataPointsMap;
		out << "Chromatogram " << id_chromatogram << endl;
		id_chromatogram++;
		for (map<int, datap>::iterator mip = dpm.begin(); mip != dpm.end(); mip++)
		{
			out << (*mip).second.first << ",";
			out << (*mip).second.second << endl;
		}
	}
	out.close();
	cout << "Fine!" << endl;
	return 0;
}


