/*
 * CRadioApp.cpp
 *
 *  Created on: Feb 2, 2016
 *      Author: michi
 */

#include "CRadioApp.h"

CRadioApp::CRadioApp()
{
	m_currentSel = 0;
	m_curStationIdx = -1;
	m_data = new UserData;
	m_data->pipeline = NULL;
	m_data->title = "";
	m_bus = NULL;
}

CRadioApp::~CRadioApp()
{
	// stop and clean up old pipeline
	gst_object_unref (m_bus);
	gst_element_set_state (m_data->pipeline, GST_STATE_NULL);
	gst_object_unref (m_data->pipeline);
}

std::string CRadioApp::getName()
{
	return "Radio Streams";
}

int CRadioApp::init()
{
	Station *station = new Station;
	station->name = "Radio Kaernten";
	//station->uri = "playbin2 uri=mms://apasf.apa.at/radio_kaernten";
	station->uri = "playbin2 uri=http://194.232.200.145:8000/";
	m_station_list.push_back(station);

	station = new Station;
	station->name = "FM4";
	station->uri = "playbin2 uri=http://mp3stream1.apasf.apa.at:8000/";
	m_station_list.push_back(station);

	station = new Station;
	station->name = "Skaworld";
	station->uri = "playbin2 uri=http://stream.laut.fm/skaworld";
	m_station_list.push_back(station);

	station = new Station;
	station->name = "Skafari";
	station->uri = "playbin2 uri=http://stream.laut.fm/skafari";
	m_station_list.push_back(station);

	station = new Station;
	station->name = "Papas Skakiste";
	station->uri = "playbin2 uri=http://stream.laut.fm/papas_skakiste007";
	m_station_list.push_back(station);

	station = new Station;
	station->name = "Test";
	station->uri = "playbin2 uri=http://docs.gstreamer.com/media/sintel_trailer-480p.webm";
	m_station_list.push_back(station);

	return 0;
}

std::vector<std::string> CRadioApp::getView()
{
	std::vector<std::string> buffer;
	time_t t;
	time(&t);

	std::string str = "";
	buffer.push_back("\e[H");
	buffer.push_back("\e[30m\e[41m"); // black fore, red back
	str = ctime(&t);
	buffer.push_back(str + "\eM \n\r\n\r");
	str = "\e[32m\e[40m" + m_data->title;
	buffer.push_back(fitStrToLine(str)+"\n\n\r"); // size 3, green fore, black back
	for (unsigned int i = 0; i < m_station_list.size(); i++)
	{
		str = "\e[33m\e[40m"; //yellow fore, black back
		if (i == m_currentSel)
		{
			str += "-> ";
		}
		else
		{
			str += "   ";
		}
		if (i == m_curStationIdx)
		{
			str += "\e[30m\e[43m"; // black fore, yellow back
		}
		str += m_station_list[i]->name + "\n\r";
		buffer.push_back(str);
	}

	buffer.push_back("\e[160;225H\e[39m\e[40m");
	buffer.push_back("\e[30m\e[41m < \e[39m\e[40m  ");
	buffer.push_back("\e[30m\e[41m > \e[39m\e[40m  ");
	buffer.push_back("\e[30m\e[41m P \e[39m\e[40m");

	return buffer;
}

void CRadioApp::buttonChanged(char buttons[3])
{
	//printf("CRadioApp::buttonChanged: %c%c%c\n",buttons[2],buttons[1],buttons[0]);
	if (strncmp(buttons,"100",3) == 0)
	{
		m_currentSel--;
		if (m_currentSel < 0 )
		{
			m_currentSel = m_station_list.size()-1;
		}
	}
	if (strncmp(buttons,"010",3) == 0)
	{
		m_currentSel++;
		if (m_currentSel == m_station_list.size())
		{
			m_currentSel = 0;
		}
	}
	if (strncmp(buttons,"001",3) == 0)
	{
		if (m_data->pipeline != NULL && m_curStationIdx >= 0)
		{
			// stop and clean up old pipeline
			printf("Stop %s\n",m_station_list[m_curStationIdx]->name.c_str());
			gst_object_unref (m_bus);
			gst_element_set_state (m_data->pipeline, GST_STATE_NULL);
			gst_object_unref (m_data->pipeline);
		}
		else
		{
			m_curStationIdx = -1;
		}

		// if selection changed start new stream
    if (m_curStationIdx != m_currentSel)
    {
    	m_curStationIdx = m_currentSel;
    	m_data->pipeline = gst_parse_launch (m_station_list[m_currentSel]->uri.c_str(), NULL);
    	m_bus = gst_element_get_bus (m_data->pipeline);
		GstStateChangeReturn ret = gst_element_set_state (m_data->pipeline, GST_STATE_PLAYING);
		if (ret == GST_STATE_CHANGE_FAILURE) {
			printf ("ERROR: Unable to set the pipeline to the playing state.\n");
		}
		else
		{
			gst_bus_add_signal_watch (m_bus);
			g_signal_connect (m_bus, "message", G_CALLBACK (cb_message), m_data);
			printf("Start %s\n",m_station_list[m_curStationIdx]->name.c_str());
		}
    }
    else
		{
			m_curStationIdx = -1;
		}
	}
}
