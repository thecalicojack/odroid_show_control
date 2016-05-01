//============================================================================
// Name        : odroid_control.cpp
// Author      : Michael Moritsch
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include "CMainApp.h"
#include <gst/gst.h>

int main(int argc, char **argv) {
	char serialPort[15] = "/dev/ttyUSB0";
	if (argc == 2)
	{
		sprintf(serialPort, "%s", argv[1]);
	}

	/* Initialize GStreamer */
	gst_init (&argc, &argv);

	CMainApp mainApp(serialPort);
	if (mainApp.init() >= 0)
	{
		mainApp.mainLoop();
	}
	else
	{
		printf("Initialization failed\n");
		return -1;
	}
	return 0;
}
