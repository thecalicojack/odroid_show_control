/*
 * CStatusApp.h
 *
 *  Created on: 18.10.2015
 *      Author: michi
 */

#ifndef CSTATUSAPP_H_
#define CSTATUSAPP_H_

#include "CApp.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <sys/utsname.h>
#include <sys/statvfs.h>
#include <math.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#define String_startsWith(s, match) (strstr((s), (match)) == (s))

struct CPUData_ {
	unsigned long long int totalTime;
	unsigned long long int userTime;

	unsigned long long int totalPeriod;
	unsigned long long int userPeriod;
};

struct diskData_ {
	unsigned long long disk_size;
	unsigned long long used;
	unsigned long long free;
};

class CStatusApp: public CApp
{
public:
	virtual ~CStatusApp();
	CStatusApp();
	std::string getName();
	int init();
	std::vector<std::string> getView();
	void buttonChanged(char buttons[3]);
private:
	char m_mountPath[2];
	int  m_GB; // Giga byte
	char m_procstat[11];
	char m_temppath[38];

	std::vector<std::string> m_buffer;
	int m_cpus;
	struct CPUData_ *m_cpuData;

	void cpuUsageDisplay();
	void cpuUsageInit();
	void cpuCount();
	void systemInfo();
	void diskSpace();
	void cpuTemp();
};

#endif /* CSTATUSAPP_H_ */
