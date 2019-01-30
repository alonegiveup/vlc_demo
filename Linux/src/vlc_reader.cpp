#include "vlc_reader.h"
#pragma comment(lib, "libvlc.lib")
#pragma comment(lib, "libvlccore.lib")
#define MAX_QUEUE 50 //最大队列空间
vlc_reader::vlc_reader(char*url)
	: inst(0), mp(0), pixel(0), w(0), h(0), rtspAddress(url)
{
	//vlc构建参数 测试--network-caching=300效果较好
	const char * const vlc_args[] = {
		"-I", "dummy", 
		"--ignore-config",
		"--network-caching=300",
		"--rtsp-tcp",
		"--no-audio",
		"--extraintf=logger", 
		"--verbose=1"
	};
	printf("%d\n", sizeof(vlc_args) / sizeof(vlc_args[0]));
	inst = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
	mp = libvlc_media_player_new(inst);
	libvlc_video_set_callbacks(mp, &cb_lock, &cb_unlock, &cb_display, this);//设置回调函数
}


vlc_reader::~vlc_reader()
{
	delete[] pixel; //释放图像数据储存内存
	std::queue<cv::Mat> empty;
	swap(empty, video_queue); //释放队列存储空间
	libvlc_media_player_stop(mp); //释放vlc句柄
	libvlc_media_player_release(mp);
	libvlc_release(inst);
}

void vlc_reader::start(int wantW, int wantH)
{
	this->w = wantW;
	this->h = wantH;
	if (pixel == nullptr)
	{
		pixel = new unsigned char[wantW*wantH * 4];
	}
	libvlc_media_player_pause(mp);
	libvlc_media_t *media = libvlc_media_new_location(inst, rtspAddress);
	libvlc_media_player_set_media(mp, media);
	libvlc_media_release(media);

	libvlc_video_set_format(mp, "RGBA", wantW, wantH, wantW * 4);//设置取流参数 以RGBA格式取流
	libvlc_media_player_play(mp);
}

void vlc_reader::pause(bool paused)
{
	if (mp){
		libvlc_media_player_set_pause(mp, paused);
	}
}

cv::Mat vlc_reader::frame() //从队列中取出一帧数据
{
	cv::Mat img;
	if (!this->video_queue.empty())
	{
		img = this->video_queue.front();
		this->video_queue.pop();
	}
	return img;
}

void* vlc_reader::cb_lock(void *opaque, void **plane)
{
	vlc_reader *p = (vlc_reader*)opaque;
	p->mtx.lock();
	*plane = p->pixel;
	return *plane;
}
void vlc_reader::cb_unlock(void *opaque, void *picture, void * const *plane)
{
	vlc_reader *p = (vlc_reader*)opaque;
	unsigned char *pix = (unsigned char*)picture;
	if (pix)
	{
		cv::Mat img = cv::Mat(cv::Size(p->w, p->h), CV_8UC4, pix);
		cv::cvtColor(img, img, CV_RGBA2BGR);
		if (p->video_queue.size() < MAX_QUEUE)
		{
			p->video_queue.push(img);
		}
		else
		{
			std::queue<cv::Mat> empty;
			swap(empty, p->video_queue);
		}
		p->mtx.unlock();
	}
}
void vlc_reader::cb_display(void *opaque, void *picture)
{

}