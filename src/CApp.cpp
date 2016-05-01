/*
 * CApp.cpp
 *
 *  Created on: 18.10.2015
 *      Author: michi
 */

#include "CApp.h"

CApp::~CApp() {
}

std::string CApp::fitStrToLine(std::string str)
{
	std::string rval = "";
	if (str.size() <= MAX_LINE_SIZE)
	{
		rval = str;
		for (int i = rval.size(); i < MAX_LINE_SIZE; i++)
		{
			rval += " ";
		}
	}
	else
	{
		rval = str.substr(0, MAX_LINE_SIZE);
	}
	return rval;
}
