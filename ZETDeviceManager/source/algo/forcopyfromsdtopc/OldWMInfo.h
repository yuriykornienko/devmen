// WMINFO из регистротора, нужно для корректного копирования 

#pragma once

#include <vector>
#include <string.h>

#ifndef __newwminfo
#define __newwminfo

#define NEWWMFSIZE 100

struct NewWMInfo
{
	NewWMInfo::NewWMInfo()
		:newppsSample(0)
	{
		newtimelabel.resize(NEWWMFSIZE);
		memset(newtimelabel.data(), 0, newtimelabel.size() * sizeof(long));
	}

	long newppsSample;
	std::vector<long> newtimelabel;
};

typedef struct _NEW_GPS_INFO_WM
{
	long double	ldLatitude;			// Latitude in NDEG - +/-[degree][min].[sec/60]
	long double ldLongitude;        // Longitude in NDEG - +/-[degree][min].[sec/60]
	long double ldAltitude;			// Antenna altitude above/below mean sea level (geoid) in meters
	long double ldSpeed;			// Speed over the ground in kilometers/hour
	int iInUse;						// Number of satellites in use (not those in view)
	int iInView;					// Total number of satellites in view
}NEW_GPS_INFO_WM, *NEW_PGPS_INFO_WM;

#endif