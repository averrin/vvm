#ifndef __RNG_H_
#define __RNG_H_

#include "vvm/device.hpp"

class RngDevice : public Device {
public:
    RngDevice(unsigned int s) : Device(std::make_shared<MemoryContainer>(s)) {}
    std::string deviceName = "Random generator";
	void tickHandler();
};


#endif // __RNG_H_
