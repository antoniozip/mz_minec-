#pragma once
class DataPoint
{
	double Mz;
	double intensity;
public:
	DataPoint();
	~DataPoint();
	double getMz();
	double getIntensity();
};

