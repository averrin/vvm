#ifndef ADDR_HPP_
#define ADDR_HPP_
#include "ostream.hpp"

struct address
{
	static address BEGIN;
	static address CODE;
	unsigned int dst;
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
	friend std::ostream& operator<<(std::ostream& os, const address& addr)
	{
		os << fmt::format(".{:08X}", addr.dst);
		return os;
	}
};
#endif
