#pragma once
#include "RawDataFile.h"
#include <boost/numeric/interval.hpp>

class ScanUtils
{
public:
	ScanUtils();
	~ScanUtils();
	int findBestFragmentScan(RawDataFile &, boost::numeric::interval<double>, boost::numeric::interval<double>);
};

