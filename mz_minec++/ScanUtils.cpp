#include "stdafx.h"
#include "ScanUtils.h"
#include "RawDataFile.h"
#include <boost/numeric/interval.hpp>

ScanUtils::ScanUtils()
{
}


ScanUtils::~ScanUtils()
{
}

int ScanUtils::findBestFragmentScan(RawDataFile dataFile, boost::numeric::interval<double> rtRange, boost::numeric::interval<double> mzRange) {

	int bestFragmentScan = -1;
	double topBasePeak = 0;

	std::vector<int> fragmentScanNumbers = dataFile.getScanNumbers(2, rtRange);

	for (int number : fragmentScanNumbers) {

		Scan scan = dataFile.getScan(number);
		double lb = mzRange.lower, up = mzRange.upper, ppp = scan.getPrecursorMz;
		if (lb < ppp && up > ppp)
		{
		//if (mzRange.lower() < scan.getPrecursorMz() && mzRange.upper() > scan.getPrecursorMz) {

			DataPoint basePeak = scan.getHighestDataPoint();

			// If there is no peak in the scan, basePeak can be null
			if (DataPoint* v = dynamic_cast<DataPoint*>(&basePeak))
				continue;

			if (basePeak.getIntensity() > topBasePeak) {
				bestFragmentScan = scan.getScanNumber();
				topBasePeak = basePeak.getIntensity();
			}
		}

	}

	return bestFragmentScan;

}