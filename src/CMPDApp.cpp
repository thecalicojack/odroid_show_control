/*
 * CMPDApp.cpp
 *
 *  Created on: 18.10.2015
 *      Author: michi
 */

#include "CMPDApp.h"

CMPDApp::CMPDApp()
{
	m_mpd_conn = NULL;
	m_playing_state = MPD_STATE_STOP;
	m_oTitleText = new CMovingTxt("");
	m_oAlbumText = new CMovingTxt("");
	m_oArtistText = new CMovingTxt("");
}

CMPDApp::~CMPDApp()
{
	mpd_connection_free(m_mpd_conn);
}

std::string CMPDApp::getName()
{
	return "Music Player Demon";
}

int CMPDApp::init()
{
	m_mpd_conn = mpd_connection_new("mopidysrv@192.168.0.11", 0, 0);
	if (m_mpd_conn == NULL)
	{
		return -1;
	}
	if (mpd_connection_get_error(m_mpd_conn) != MPD_ERROR_SUCCESS)
	{
		fprintf(stderr,"ERROR connection to MPD-Server: %s\n",mpd_connection_get_error_message(m_mpd_conn));
		mpd_connection_free(m_mpd_conn);
		return -1;
	}
	printf("connected successfully to MPD server\n");
	return 0;
}

std::vector<std::string> CMPDApp::getView()
{
	std::vector<std::string> buffer;
	struct mpd_status * status;
	struct mpd_song *song;
	const char *value;
	int mpd_error;
	time_t t;
	time(&t);

	if (m_mpd_conn == NULL)
	{
		m_mpd_conn = mpd_connection_new("mopidysrv@192.168.0.11", 0, 0);
	}
	mpd_error = mpd_connection_get_error(m_mpd_conn);
	if (mpd_error != MPD_ERROR_SUCCESS)
	{
		buffer.clear();
		buffer.push_back(mpd_connection_get_error_message(m_mpd_conn));
		fprintf(stderr,"MPD_ERROR1: %s\n", mpd_connection_get_error_message(m_mpd_conn));
		mpd_connection_free(m_mpd_conn);
		m_mpd_conn = NULL;
		return buffer;
	}

	std::string str = "";
	buffer.push_back("\e[H");
	buffer.push_back("\e[30m\e[41m"); // black fore, red back
	str = ctime(&t);
	buffer.push_back(str + "\eM \n\r\n\r");
	buffer.push_back("\e[32m\e[40m"); //green fore, black back

	mpd_command_list_begin(m_mpd_conn, true);
	mpd_send_status(m_mpd_conn);
	mpd_send_current_song(m_mpd_conn);
	mpd_command_list_end(m_mpd_conn);

	status = mpd_recv_status(m_mpd_conn);
	if (status == NULL)
	{
		buffer.clear();
		buffer.push_back(mpd_connection_get_error_message(m_mpd_conn));
		fprintf(stderr,"MPD_ERROR2: %s\n", mpd_connection_get_error_message(m_mpd_conn));
		mpd_connection_free(m_mpd_conn);
		m_mpd_conn = NULL;
		return buffer;
	}

	// get queue position and time
	std::stringstream queue;
	queue << "#" << mpd_status_get_song_pos(status) + 1 << "/" <<
			mpd_status_get_queue_length(status);
	char scTime[15];
	sprintf(scTime, "%02d:%02d/%02d:%02d", mpd_status_get_elapsed_time(status)/60,
			mpd_status_get_elapsed_time(status)%60,
			mpd_status_get_total_time(status)/60,
			mpd_status_get_total_time(status)%60);
	std::stringstream time;
	time << scTime;
	str = "";
	str.insert(0, MAX_LINE_SIZE - (queue.str().size() + time.str().size()),' ');
	buffer.push_back("\eE\e[32m" + fitStrToLine(queue.str() + str + time.str()));

	// Get volume and playing state
	std::stringstream ss1;
	ss1 << mpd_status_get_volume(status);
	str = "|" + ss1.str() + "%|";
	if (mpd_status_get_state(status) == MPD_STATE_PLAY)
	{
		str += " |playing|";
		m_playing_state = MPD_STATE_PLAY;
	}
	else if (mpd_status_get_state(status) == MPD_STATE_PAUSE)
	{
		str += " |paused| ";
		m_playing_state = MPD_STATE_PAUSE;
	}
	else if (mpd_status_get_state(status) == MPD_STATE_STOP)
	{
		str += " |stopped|";
		m_playing_state = MPD_STATE_STOP;
	}
	if (mpd_status_get_repeat(status) == 1)
	{
		str += " |repeat|";
	}
	else
	{
		str += " |single|";
	}
	buffer.push_back("\eE\e[34m" + fitStrToLine(str));

	if (mpd_connection_get_error(m_mpd_conn) != MPD_ERROR_SUCCESS) {
		buffer.clear();
		buffer.push_back(mpd_connection_get_error_message(m_mpd_conn));
		fprintf(stderr,"MPD_ERROR3: %s\n", mpd_connection_get_error_message(m_mpd_conn));
		mpd_connection_free(m_mpd_conn);
		m_mpd_conn = NULL;
		return buffer;
	}

	// get song infos
	mpd_response_next(m_mpd_conn);
	if ((song = mpd_recv_song(m_mpd_conn)) != NULL)
	{
		// Title
		str = "";
		if ((value = mpd_song_get_tag(song, MPD_TAG_TRACK, 0)) != NULL)
		{
			str = value;
		}
		str += " - ";
		if ((value = mpd_song_get_tag(song, MPD_TAG_TITLE, 0)) != NULL)
		{
			str = value;
		}
		m_oTitleText->setText(str);
		str = m_oTitleText->getText();
		buffer.insert(buffer.end()-2,"\eE\e[33m" + fitStrToLine(str));
		// Album
		str = "";
		if ((value = mpd_song_get_tag(song, MPD_TAG_ALBUM, 0)) != NULL)
		{
			str = value;
		}
		m_oAlbumText->setText(str);
		str = m_oAlbumText->getText();
		buffer.insert(buffer.end()-2,"\eE\e[33m" + fitStrToLine(str));
		// Artist
		str = "";
		if ((value = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0)) != NULL)
		{
			str = value;
		}
		m_oArtistText->setText(str);
		str = m_oArtistText->getText();
		buffer.insert(buffer.end()-2,"\eE\e[33m" + fitStrToLine(str) + "\n");
		str = "";
		mpd_song_free(song);
	}

	if (mpd_connection_get_error(m_mpd_conn) != MPD_ERROR_SUCCESS) {
		buffer.clear();
		buffer.push_back(mpd_connection_get_error_message(m_mpd_conn));
		fprintf(stderr,"MPD_ERROR4: %s\n", mpd_connection_get_error_message(m_mpd_conn));
		mpd_connection_free(m_mpd_conn);
		m_mpd_conn = NULL;
		return buffer;
	}

	if (!mpd_response_finish(m_mpd_conn)) {
		buffer.clear();
		buffer.push_back(mpd_connection_get_error_message(m_mpd_conn));
		fprintf(stderr,"MPD_ERROR5: %s\n", mpd_connection_get_error_message(m_mpd_conn));
		mpd_connection_free(m_mpd_conn);
		m_mpd_conn = NULL;
		return buffer;
	}

	buffer.push_back("\e[160;225H\e[39m\e[40m");
	buffer.push_back("\e[30m\e[41m < \e[39m\e[40m  ");
	buffer.push_back("\e[30m\e[41m > \e[39m\e[40m  ");
	buffer.push_back("\e[30m\e[41m P \e[39m\e[40m");

	return buffer;
}

void CMPDApp::buttonChanged(char buttons[3])
{
	//printf("CMPDApp::buttonChanged: %c%c%c\n",buttons[2],buttons[1],buttons[0]);
	if (m_mpd_conn != NULL)
	{
		if (strncmp(buttons,"100",3) == 0)
		{
			mpd_command_list_begin(m_mpd_conn, true);
			mpd_send_previous(m_mpd_conn);
			mpd_command_list_end(m_mpd_conn);
		}
		if (strncmp(buttons,"010",3) == 0)
		{
			mpd_command_list_begin(m_mpd_conn, true);
			mpd_send_next(m_mpd_conn);
			mpd_command_list_end(m_mpd_conn);
		}
		if (strncmp(buttons,"001",3) == 0)
		{
			mpd_command_list_begin(m_mpd_conn, true);
			if (m_playing_state == MPD_STATE_STOP)
			{
				mpd_send_play(m_mpd_conn);
			}
			else
			{
				mpd_send_toggle_pause(m_mpd_conn);
			}
			mpd_command_list_end(m_mpd_conn);
		}
		if (mpd_connection_get_error(m_mpd_conn) != MPD_ERROR_SUCCESS) {
			fprintf(stderr,"MPD_ERROR6: %s\n", mpd_connection_get_error_message(m_mpd_conn));
			mpd_connection_free(m_mpd_conn);
			m_mpd_conn = NULL;
			return;
		}

		if (!mpd_response_finish(m_mpd_conn)) {
			fprintf(stderr,"MPD_ERROR7: %s\n", mpd_connection_get_error_message(m_mpd_conn));
			mpd_connection_free(m_mpd_conn);
			m_mpd_conn = NULL;
			return;
		}
	}
}
