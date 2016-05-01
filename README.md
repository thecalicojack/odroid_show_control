# odroid_show_control
This project offers a framework to "run" different applications on the
[ODROID_SHOW](http://odroid.com/dokuwiki/doku.php?id=en:odroidshow).
(Note: odroid_show_control runs on a linux pc and controls the ODROID_SHOW)
Three apps are already included:
* Status: shows system stati.
* Music Player Demon control: allows to control a mpd, and shows infos about
  currently running song
* Radios Streams control: allows to start/stop online radio streams.

## Getting started
1. Load the firmware to the ODROID_SHOW by generally following this
[instructions](http://odroid.com/dokuwiki/doku.php?id=en:show_setting).
But of course use firmware/show_main.ino instead of there file.
2. Install the following libraries:
  * libgstreamer0.10-dev
  * libmpdclient-dev
3. compile and run

## Add new applications
* Create a new class that implements CApp.
* In CMainApp::init add the app to the app list.
