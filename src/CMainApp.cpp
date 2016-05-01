/*
 * CMainApp.cpp
 *
 *  Created on: 18.10.2015
 *      Author: michi
 */

#include "CMainApp.h"

CMainApp::CMainApp()
{
	sprintf(m_serialPort,"/dev/ttyUSB0");
	m_usbdev = 0;
	m_currentAppIdx = 0;
	m_currentSel = 0;
	m_AppChanged = 0;
}

CMainApp::CMainApp(char *serialPort)
{
	sprintf(m_serialPort,"%s",serialPort);
	m_usbdev = 0;
	m_currentAppIdx = 0;
	m_currentSel = 0;
	m_AppChanged = 0;
}

CMainApp::~CMainApp()
{
}

int CMainApp::init(void)
{
	int rval = 0;
	initSerial();

	//clear screen
	std::vector<std::string> buffer;
	buffer.push_back("\ec\e[2s\e[1r");
	writeData(buffer);

	m_app_list.push_back(this);

	CStatusApp *statusApp = new CStatusApp();
	rval += statusApp->init();
	m_app_list.push_back(statusApp);

	CMPDApp *mpdApp = new CMPDApp();
	rval += mpdApp->init();
	m_app_list.push_back(mpdApp);
	
	CRadioApp *radioApp = new CRadioApp();
	rval += radioApp->init();
	m_app_list.push_back(radioApp);

	return rval;
}


std::string CMainApp::getName()
{
	std::string buffer = "Main Menu";
	return buffer;
}

std::vector<std::string> CMainApp::getView()
{
	std::vector<std::string> buffer;
	std::string str = "";
	time_t t;
	time(&t);

	buffer.push_back("\e[H"); //cursor to top
	buffer.push_back("\e[30m\e[41m"); // black fore, red back
	str = ctime(&t);
	buffer.push_back(str + "\eM \n\r\n\r");
	buffer.push_back("\e[32m\e[40mMain Menu\n\r"); // green fore, black back
	buffer.push_back("\e[33m\n\r");
	for (unsigned int i = 1; i < m_app_list.size(); i++)
	{
		if (i-1 == m_currentSel)
		{
			str = "-> ";
		}
		else
		{
			str = "   ";
		}
		str += m_app_list[i]->getName() + "\n\r";
		buffer.push_back(str);
	}
	buffer.push_back("\e[160;225H\e[39m\e[40m");
	buffer.push_back("\e[30m\e[41m < \e[39m\e[40m  ");
	buffer.push_back("\e[30m\e[41m > \e[39m\e[40m  ");
	buffer.push_back("\e[30m\e[41m S \e[39m\e[40m");
	return buffer;
}

void CMainApp::buttonChanged(char buttons[3])
{
	//printf("CMainApp::buttonChanged: %c%c%c\n",buttons[2],buttons[1],buttons[0]);
	if (strncmp(buttons,"100",3) == 0)
	{
		m_currentSel--;
		if (m_currentSel < 0 )
		{
			m_currentSel = m_app_list.size()-2;
		}
	}
	if (strncmp(buttons,"010",3) == 0)
	{
		m_currentSel++;
		if (m_currentSel == m_app_list.size()-1)
		{
			m_currentSel = 0;
		}
	}
	if (strncmp(buttons,"001",3) == 0)
	{
		m_currentAppIdx = m_currentSel+1;
		m_AppChanged = 1;
	}
}

void CMainApp::mainLoop()
{
	char state = 0;
	char buttonStates[3];

	while(1)
	{
		if (isDeviceAvailable())
		{
			if (m_usbdev == -1)
			{
				initSerial();
			}
			read(m_usbdev, &state, 1);
			if (state == '4')
			{
				printf("Reset pressed (mainLoop)\n");
				m_currentAppIdx = 0;
				m_AppChanged = 1;
			}
			else if (state == '5')
			{
				if(checkButtonChanged(buttonStates))
				{
					m_app_list[m_currentAppIdx]->buttonChanged(buttonStates);
				}
			}
			if (m_AppChanged == 1)
			{
				//clear screen
				std::vector<std::string> buffer;
				buffer.push_back("\ec\e[2s\e[1r");
				writeData(buffer);
				m_AppChanged = 0;
			}
			this->writeData(m_app_list[m_currentAppIdx]->getView());
		}
		else
		{
			if (m_usbdev != -1)
			{
				printf("Serial connection closed\n");
				close(m_usbdev);
				m_usbdev = -1;
			}
			usleep(50000);
		}
	}
}

bool CMainApp::isDeviceAvailable()
{
	FILE *fSerialPort;
	if ((fSerialPort = fopen(m_serialPort, "r")) > 0)
	{
		fclose(fSerialPort);
		return true;
	}
	return false;
}

void CMainApp::initSerial()
{
	int usbdev;
	struct termios options;

	printf("Open serial port: %s\n",m_serialPort);
	usbdev = open(m_serialPort, O_RDWR | O_NOCTTY | O_NDELAY);

	if (usbdev == -1)
	  perror("open_port : Unable to open:");

	tcgetattr(usbdev, &options);

	cfsetispeed(&options, baudrate);
	cfsetospeed(&options, baudrate);

	options.c_cflag |= CS8;
	options.c_iflag |= IGNBRK;
	options.c_iflag &= ~( BRKINT | ICRNL | IMAXBEL | IXON);
	options.c_oflag &= ~( OPOST | ONLCR );
	options.c_lflag &= ~( ISIG | ICANON | IEXTEN | ECHO | ECHOE | ECHOK |
						  ECHOCTL | ECHOKE);
	options.c_lflag |= NOFLSH;
	options.c_cflag &= ~CRTSCTS;

	tcsetattr(usbdev, TCSANOW, &options);
	m_usbdev = usbdev;
}

void CMainApp::writeData(std::vector<std::string> strings)
{
	char isbusy = 0;
	int length = 0;
	char buttonStates[3];
	std::string str;

	for (unsigned int i = 0; i < strings.size(); i++)
	{
		isbusy = 0;
		str = strings[i];
		write(m_usbdev, "\006", 1);
		length = str.length() + 48;
		write(m_usbdev, &length, 1);
		while (isbusy != '6' && isDeviceAvailable()) {
			read(m_usbdev, &isbusy, 1);
			if (isbusy == '4')
			{
				printf("Reset pressed (writeData)\n");
				m_currentAppIdx = 0;
				m_AppChanged = 1;
				break;
			}
			else if (isbusy == '5' && checkButtonChanged(buttonStates))
			{
				m_app_list[m_currentAppIdx]->buttonChanged(buttonStates);
			}
			usleep(10000);
		}
		if (isbusy == '6')
		{
			write(m_usbdev, str.c_str(), length - 48);
		}
		else
		{
			break;
		}
	}
}

int CMainApp::checkButtonChanged(char *buttons)
{
	char btn_state[3];
	char fin;
	read(m_usbdev, &btn_state[0], 1);
	read(m_usbdev, &btn_state[1], 1);
	read(m_usbdev, &btn_state[2], 1);
	read(m_usbdev, &fin, 1);
	if (btn_state[0] == '0' || btn_state[0] == '1')
	{
		for (int i=0; i<3; i++)
		{
			buttons[i] = btn_state[i];
		}
		return 1;
	}
	return 0;
}

