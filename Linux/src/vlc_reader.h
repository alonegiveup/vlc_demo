#pragma once
#include <mutex>
#include <queue>
#include "vlc/vlc.h"
#include "opencv2/opencv.hpp"

class vlc_reader
{
public:
	vlc_reader(char* url = 0);
	~vlc_reader();
	void start(int wantW = 640, int wantH = 480);
	void pause(bool paused);
	cv::Mat frame();
	int w, h;
private:
	char* rtspAddress = "";
	std::mutex mtx;
	std::queue<cv::Mat> video_queue;
	libvlc_instance_t *inst;
	libvlc_media_player_t *mp;
	unsigned char *pixel;

	static void *cb_lock(void *opaque, void **plane);
	static void cb_unlock(void *opaque, void *picture, void * const *plane);
	static void cb_display(void *opaque, void *picture);
};

