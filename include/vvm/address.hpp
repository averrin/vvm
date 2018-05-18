#ifndef ADDR_HPP_
#define ADDR_HPP_
#include "ostream.hpp"
#include "vvm/sizes.hpp"

struct address
{
	static address BEGIN;
	static address CODE;
	unsigned int dst;
    bool redirect = false;
    bool storeByte = false;
	friend bool operator==(const address& lhs, const address&  rhs)
	{
		return lhs.dst == rhs.dst;
	}
	friend address operator+(const address& lhs, const unsigned int rhs)
	{
		return address{ lhs.dst + rhs };
	}
	friend address operator++(address& lhs, const int rhs)
	{
		lhs.dst++; //TODO: postfix must return old value
		return lhs;
	}
	friend address operator--(address& lhs, const int rhs)
	{
		lhs.dst--; //TODO: postfix must return old value
		return lhs;
	}
	friend address& operator+=(address& lhs, const unsigned int rhs)
	{
		lhs.dst += rhs;
		return lhs;
	}
	friend address& operator-=(address& lhs, const unsigned int rhs)
	{
		lhs.dst -= rhs;
		return lhs;
	}

	friend address operator-(const address& lhs, const unsigned int rhs)
	{
		return address{ lhs.dst - rhs };
	}

	friend address operator-(const address& lhs, const address& rhs)
	{
		return address{ lhs.dst - rhs.dst };
	}

	friend bool operator<(address& lhs, address& rhs)
	{
		return lhs.dst < rhs.dst;
	}

	friend std::ostream& operator<<(std::ostream& os, const address& addr)
	{
		os << fmt::format("{}{:0{}X}{}",
                          addr.redirect ? "[" : (addr.storeByte ? "*" : "."),
                          addr.dst, INT_SIZE*2,
                          addr.redirect ? "]" : " ");
		return os;
	}
};
#endif
