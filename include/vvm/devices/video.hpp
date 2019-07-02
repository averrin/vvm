#ifndef __VIDEO_H_
#define __VIDEO_H_

#include "vvm/device.hpp"

class VideoDevice : public Device {
public:
    VideoDevice(unsigned int w, unsigned int h) : Device(std::make_shared<MemoryContainer>(w*h)), width(w), height(h) {}
    std::string deviceName = "Video memory";
	void tickHandler() {};

    unsigned int width;
    unsigned int height;
};


#endif // __VIDEO_H_
