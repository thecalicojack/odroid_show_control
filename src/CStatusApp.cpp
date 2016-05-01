/*
 * CStatusApp.cpp
 *
 *  Created on: 18.10.2015
 *      Author: michi
 */

#include "CStatusApp.h"

CStatusApp::~CStatusApp()
{
}

CStatusApp::CStatusApp()
{
	strcpy(m_mountPath, "/");
	m_GB = 1024*1024*1024; // Giga byte
	strcpy(m_procstat, "/proc/stat");
	strcpy(m_temppath, "/sys/class/thermal/thermal_zone0/temp");
	m_cpus = -1;
	m_cpuData = NULL;
}

std::string CStatusApp::getName()
{
	return "Status";
}

int CStatusApp::init()
{
	cpuCount();
        cpuUsageInit();
	return 0;
}

std::vector<std::string> CStatusApp::getView()
{
	m_buffer.clear();
	m_buffer.push_back("\e[H");
	systemInfo();
	cpuUsageDisplay();
	diskSpace();
	cpuTemp();
	return m_buffer;
}

void CStatusApp::buttonChanged(char buttons[3])
{
	//printf("CStatusApp::buttonChanged: %c%c%c\n",buttons[2],buttons[1],buttons[0]);
}

void CStatusApp::diskSpace()
{
	char buffer[64];
	struct statvfs sfs;
	struct diskData_ diskData;
	if (!statvfs(m_mountPath, &sfs)) {
		diskData.disk_size = (long long)sfs.f_blocks * sfs.f_bsize;
		diskData.free = (long long)sfs.f_bfree * sfs.f_bsize;
		diskData.used = (long long)diskData.disk_size - diskData.free;
	}

	//sprintf(buffer, "\e[37mMount Path : \e[36m%s\n\r", m_mountPath);
	//m_buffer.push_back(buffer);
	sprintf(buffer, "\e[33mdisk_size : \e[32m%.2lfGB\n\r",
						(double)diskData.disk_size/m_GB);
	m_buffer.push_back(buffer);
	sprintf(buffer, "\e[35mused : \e[32m%.2lfGB\n\r",
						(double)diskData.used/m_GB);
	m_buffer.push_back(buffer);
	sprintf(buffer, "\e[36mfree : \e[32m%.2lfGB\n\r",
						(double)diskData.free/m_GB);
	m_buffer.push_back(buffer);
}

void CStatusApp::systemInfo()
{
	time_t t;
	struct utsname uts;
	char buffer[64];

	time(&t);
	sprintf(buffer, "\e[30m\e[41m%s\r", ctime(&t));
	m_buffer.push_back(buffer);
	uname(&uts);
	sprintf(buffer, "\e[37m\e[40mOSname:\e[36m%s\n\r", uts.sysname);
	m_buffer.push_back(buffer);
	sprintf(buffer, "\e[37mVersion:\e[36m%s\n\r", uts.release);
	m_buffer.push_back(buffer);
	sprintf(buffer, "\e[37mMachine:\e[36m%s\n\r", uts.machine);
	m_buffer.push_back(buffer);
}

void CStatusApp::cpuCount()
{
	char buffer[255];
	FILE *file = fopen(m_procstat, "r");
	do {
		m_cpus++;
		fgets(buffer, 255, file);
	} while (String_startsWith(buffer, "cpu"));
	fclose(file);
}

void CStatusApp::cpuUsageInit()
{
	int i;
	m_cpuData = (struct CPUData_ *) malloc(m_cpus * sizeof(struct CPUData_));

	for (i = 0; i < m_cpus; i++) {
		m_cpuData[i].totalTime = 1;
		m_cpuData[i].userTime = 0;
		m_cpuData[i].totalPeriod = 1;
		m_cpuData[i].userPeriod = 0;
	}
}

void CStatusApp::cpuUsageDisplay()
{
	unsigned long long int usertime, nicetime, systemtime, systemalltime,
				idlealltime, idletime, totaltime, virtalltime;
	unsigned long long int ioWait, irq, softIrq, steal, guest;
	struct utsname uts;
	double total = 0;
	FILE *file;
	int cpuid;
	int i;
	time_t t;
	char buffer[255];

	file = fopen(m_procstat, "r");
	ioWait = irq = softIrq = steal = guest = 0;

	for (i = 0; i < m_cpus; i++) {
		fgets(buffer, 255, file);
		if (i == 0) {
			sscanf(buffer, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
					&usertime, &nicetime, &systemtime, &idletime,
					&ioWait, &irq, &softIrq, &steal, &guest);
		} else {
			sscanf(buffer, "cpu%d %llu %llu %llu %llu %llu %llu %llu %llu %llu",
					&cpuid, &usertime, &nicetime, &systemtime, &idletime,
					&ioWait, &irq, &softIrq, &steal, &guest);
			assert(cpuid == i - 1);
		}

		idlealltime = idletime + ioWait;
		systemalltime = systemtime + irq + softIrq;
		virtalltime = steal + guest;
		totaltime = usertime + nicetime + systemalltime +
			idlealltime + virtalltime;

		assert(usertime >= m_cpuData[i].userTime);
		assert(totaltime >= m_cpuData[i].totalTime);

		m_cpuData[i].userPeriod = usertime - m_cpuData[i].userTime;
		m_cpuData[i].totalPeriod = totaltime - m_cpuData[i].totalTime;

		m_cpuData[i].totalTime = totaltime;
		m_cpuData[i].userTime = usertime;

		total = (double)m_cpuData[i].totalPeriod;

		if ((i != 0) && (i%2 == 1)) {
			sprintf(buffer, "\e[33mcpu%d:\e[32m%4.1f%% ",
					i, m_cpuData[i].userPeriod/total*100.0);
			m_buffer.push_back(buffer);
		} else if ((i != 0) && (i%2 == 0)) {
			sprintf(buffer, "\e[33mcpu%d:\e[32m%4.1f%%  \n\r",
					i, m_cpuData[i].userPeriod/total*100.0);
			m_buffer.push_back(buffer);
		}
	}

	fclose(file);
}

void CStatusApp::cpuTemp()
{
	FILE *file;
	int iTemp = 0;
	char buffer[64];

	file = fopen(m_temppath,"r");
	if (file != NULL)
	{
		fscanf(file, "%d", &iTemp);
		sprintf(buffer,"\e[33mcpu temp: \e[32m%03.2fC\n\r", ((float)iTemp)/1000);
		m_buffer.push_back(buffer);
		fclose(file);
	}
	else
	{
		perror ("Error at opening cpu_temp file");
		printf("filename: %s\n", m_temppath);
	}
}

