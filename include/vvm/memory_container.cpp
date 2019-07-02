#include "vvm/memory_container.hpp"

MemoryContainer::MemoryContainer(unsigned int s): size(s) {
    data = vm_mem{};
    data.reserve(size);
    data.assign(size, std::byte{0x0});
}

MemoryContainer::MemoryContainer(vm_mem d): data(d) {
    size = data.size();
}

unsigned int MemoryContainer::readInt(address pointer) {
    unsigned int n = 0;
    for (auto i = 0; i < INT_SIZE; i++) {
        n |= static_cast<int>(data[pointer.dst + i]) << 8*(INT_SIZE - 1 - i);
    }
  return n;
}

int MemoryContainer::readSignedInt(address pointer) {
    int n = 0;
    for (auto i = 0; i < INT_SIZE; i++) {
        n |= static_cast<int>(data[pointer.dst + i]) << 8*(INT_SIZE - 1 - i);
    }
  return n;
}


void MemoryContainer::writeInt(address pointer, const int n) {
    for (auto i = 0; i < INT_SIZE; i++) {
        data[pointer.dst] = static_cast<std::byte>((n >> 8*(INT_SIZE - 1 - i)) & 0xFF);
        pointer++;
    }
}

void MemoryContainer::writeByte(address pointer, const std::byte ch) {
  data[pointer.dst] = ch;
}

std::byte MemoryContainer::readByte(address pointer) {
  return data[pointer.dst];
}

void MemoryContainer::resize(unsigned int new_size) {
    size = new_size;
    data.resize(size);
}

void MemoryContainer::clear() {
    data.assign(size, std::byte{0x0});
}
