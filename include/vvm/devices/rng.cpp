#include "vvm/devices/rng.hpp"

void RngDevice::tickHandler() {
    memory->writeByte(address{0x00}, std::byte{rand()%7+1});
    memory->writeByte(address{0x01}, std::byte{rand()%7+1});
    memory->writeByte(address{0x02}, std::byte{rand()%7+1});
    memory->writeByte(address{0x03}, std::byte{rand()%7+1});
}
