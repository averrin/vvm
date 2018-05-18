#pragma once
#include <cstddef>
#include <vector>
#include "vvm/address.hpp"
#include "vvm/constants.hpp"
#include "vvm/sizes.hpp"

class MemoryContainer {
private:
        typedef std::vector<std::byte> vm_mem;

public:
        vm_mem data;
        unsigned int size;
        address offset;

        MemoryContainer(unsigned int size);
        MemoryContainer(vm_mem data);

        void writeByte(address pointer, std::byte ch);
        void writeAddress(address pointer, const address n);
        void writeInt(address pointer, int n);

        std::byte readByte(address pointer);
        address readAddress(address pointer);
        unsigned int readInt(address pointer);
        int readSignedInt(address pointer);

        void clear();
        void resize(unsigned int size);

	friend std::ostream& operator<<(std::ostream& os, const MemoryContainer& mem)
	{
		os << "MEM: " << mem.offset << "SIZE: " << mem.size;
		return os;
	}
};
