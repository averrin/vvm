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
  const auto n = (static_cast<int>(data[pointer.dst]) << 24) |
                 (static_cast<int>(data[pointer.dst + 1]) << 16) |
                 (static_cast<int>(data[pointer.dst + 2]) << 8) |
                 (static_cast<int>(data[pointer.dst + 3]));
  return n;
}

int MemoryContainer::readSignedInt(address pointer) {
  const auto n = (static_cast<int>(data[pointer.dst]) << 24) |
                 (static_cast<int>(data[pointer.dst + 1]) << 16) |
                 (static_cast<int>(data[pointer.dst + 2]) << 8) |
                 (static_cast<int>(data[pointer.dst + 3]));
  return n;
}


void MemoryContainer::writeInt(address pointer, const int n) {
    data[pointer.dst] = static_cast<std::byte>((n >> 24) & 0xFF);
    pointer++;
    data[pointer.dst] = static_cast<std::byte>((n >> 16) & 0xFF);
    pointer++;
    data[pointer.dst] = static_cast<std::byte>((n >> 8) & 0xFF);
    pointer++;
    data[pointer.dst] = static_cast<std::byte>(n & 0xFF);
    pointer++;
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

MemoryContainer::vm_mem MemoryContainer::dump() {
    return data;
}

void MemoryContainer::clear() {
    data.assign(size, std::byte{0x0});
}
