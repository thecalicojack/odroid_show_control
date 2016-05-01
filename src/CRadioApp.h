/*
 * CRadioApp.h
 *
 *  Created on: Feb 2, 2016
 *      Author: michi
 */

#ifndef CRADIOAPP_H_
#define CRADIOAPP_H_

#include <string.h>
#include <vector>
#include <gst/gst.h>
#include <gst/gstmessage.h>
#include "CApp.h"

struct Station {
	std::string name;
	std::string uri;
};

typedef struct _UserData {
	GstElement *pipeline;
	std::string title;
} UserData;

class CRadioApp: public CApp {
public:
	CRadioApp();
	virtual ~CRadioApp();
	std::string getName();
	int init();
	std::vector<std::string> getView();
	void buttonChanged(char buttons[3]);
private:
	std::vector<Station*> m_station_list;
	int  m_curStationIdx;
	int  m_currentSel;
	GstBus   *m_bus;
	UserData *m_data;
};

static void print_one_tag (const GstTagList * list, const gchar * tag, gpointer data)
{
	int i, num;

	num = gst_tag_list_get_tag_size (list, tag);
	for (i = 0; i < num; ++i) {
		const GValue *val;

		/* Note: when looking for specific tags, use the gst_tag_list_get_xyz() API,
		 * we only use the GValue approach here because it is more generic */
		val = gst_tag_list_get_value_index (list, tag, i);
		if (G_VALUE_HOLDS_STRING (val)) {
			g_print ("\t%20s : %s\n", tag, g_value_get_string (val));
		} else if (G_VALUE_HOLDS_UINT (val)) {
			g_print ("\t%20s : %u\n", tag, g_value_get_uint (val));
		} else if (G_VALUE_HOLDS_DOUBLE (val)) {
			g_print ("\t%20s : %g\n", tag, g_value_get_double (val));
		} else if (G_VALUE_HOLDS_BOOLEAN (val)) {
			g_print ("\t%20s : %s\n", tag,
			  (g_value_get_boolean (val)) ? "true" : "false");
		} else {
			g_print ("\t%20s : tag of type '%s'\n", tag, G_VALUE_TYPE_NAME (val));
		}
	}
}

static void cb_message (GstBus *bus, GstMessage *msg, UserData *data) {
	GstTagList *tags = NULL;
	printf("New Message:\n");
	switch (GST_MESSAGE_TYPE (msg)) {
		case GST_MESSAGE_TAG: {
		  gst_message_parse_tag (msg, &tags);
		  gst_tag_list_foreach (tags, print_one_tag, data);
		  g_print("\n");
		  //gst_tag_list_unref (tags);
		  break;
		}
		case GST_MESSAGE_ERROR: {
		  GError *err;
		  gchar *debug;

		  gst_message_parse_error (msg, &err, &debug);
		  g_print ("Error: %s\n", err->message);
		  g_error_free (err);
		  g_free (debug);

		  gst_element_set_state (data->pipeline, GST_STATE_READY);
		  break;
		}
		case GST_MESSAGE_BUFFERING: {
		  gint percent = 0;

		  gst_message_parse_buffering (msg, &percent);
		  g_print ("Buffering (%3d%%)\r", percent);
		  /* Wait until buffering is complete before start/resume playing */
		  if (percent < 100)
			gst_element_set_state (data->pipeline, GST_STATE_PAUSED);
		  else
			gst_element_set_state (data->pipeline, GST_STATE_PLAYING);
		  break;
		}
		case GST_MESSAGE_CLOCK_LOST:
		  /* Get a new clock */
		  gst_element_set_state (data->pipeline, GST_STATE_PAUSED);
		  gst_element_set_state (data->pipeline, GST_STATE_PLAYING);
		  break;
		default:
		  /* Unhandled message */
		  break;
	}
}

#endif /* CRADIOAPP_H_ */
