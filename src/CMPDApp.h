/*
 * CMPDApp.h
 *
 *  Created on: 18.10.2015
 *      Author: michi
 */

#ifndef CMPDAPP_H_
#define CMPDAPP_H_

#include "CApp.h"
#include "CMovingTxt.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <string.h>
#include <sstream>
#include <mpd/connection.h>
#include <mpd/client.h>
#include <mpd/status.h>

class CMPDApp: public CApp
{
public:
	CMPDApp();
	virtual ~CMPDApp();
	std::string getName();
	int init();
	std::vector<std::string> getView();
	void buttonChanged(char buttons[3]);
private:
	struct mpd_connection *m_mpd_conn;
	int m_playing_state;
	CMovingTxt *m_oTitleText;
	CMovingTxt *m_oAlbumText;
	CMovingTxt *m_oArtistText;
};

#endif /* CMPDAPP_H_ */
