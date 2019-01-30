#include "vlc_reader.h"

int main()
{
	vlc_reader r("rtsp://admin:hik123456@192.168.0.15:554/cam/realmonitor?channel=1&subtype=0");
	int rtsp_w = 640, rtsp_h = 480;
	r.start(rtsp_w, rtsp_h);
	while (1)
	{
		cv::Mat frame = r.frame();
		if (!frame.empty())
		{
			imshow("test", frame);
			int ch = cv::waitKey(1);
			if (ch > 0)
				break;
		}
	}
	return 0;
}
