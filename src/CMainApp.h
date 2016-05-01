/*
 * CMainApp.h
 *
 *  Created on: 18.10.2015
 *      Author: michi
 */

#ifndef CMAINAPP_H_
#define CMAINAPP_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <string>
#include "CApp.h"
#include "CMPDApp.h"
#include "CStatusApp.h"
#include "CRadioApp.h"

#define baudrate	B500000
#define MAX_BUFFER_SIZE 64

class CMainApp : public CApp{
public:
	CMainApp();
	CMainApp(char *serialPort);
	virtual ~CMainApp();
	std::string getName();
	int init();
	void mainLoop();
	std::vector<std::string> getView();
	void buttonChanged(char buttons[3]);
	static void backToMenu();
private:
	void  initSerial();
	int  checkButtonChanged(char *buttons);
	void writeData(std::vector<std::string> strings);
	bool isDeviceAvailable();

	char m_serialPort[15];
	int  m_usbdev;
	int  m_currentAppIdx;
	int  m_currentSel;
	int  m_AppChanged;
	std::vector<CApp*> m_app_list;
};

#endif /* CMAINAPP_H_ */
