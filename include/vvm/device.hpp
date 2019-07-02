#ifndef __DEVICE_H_
#define __DEVICE_H_

#include <map>
#include <memory>
#include <functional>
#include "vvm/memory_container.hpp"

typedef std::function<void()> dt_handler;

class Device {
public:
    Device() {}
    Device(std::shared_ptr<MemoryContainer> m) : memory(m) {}
    virtual ~Device() = default;
    std::shared_ptr<MemoryContainer> memory;
	virtual void tickHandler() = 0;
	std::map<const std::byte, dt_handler> intHandlers;
    std::string deviceName;
};

#endif // __DEVICE_H_
