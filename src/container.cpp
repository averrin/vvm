#include "vvm/container.hpp"
#include "rang.hpp"
#include "format.h"
#include <utility>
#include <sstream>
#include <fstream>
#include <variant>

address address::BEGIN = address{ 0x0 };
address address::CODE = address{ CODE_OFFSET };

std::array<opSpec, 21> specs = {
	NOP_spec, MOV_mm_spec, MOV_mc_spec, ADD_mm_spec, ADD_mc_spec,
	SUB_mm_spec, SUB_mc_spec, OUT_spec, CMP_mm_spec, CMP_mc_spec,
	JNE_a_spec, JNE_r_spec, JE_spec, JMP_a_spec, JMP_r_spec,
	INT_spec, PUSH_m_spec, PUSH_c_spec, POP_spec, INC_spec, DEC_spec,
};

std::map<std::string, address> reserved_addresses = {
    {"STATE", STATE},
    {"ESP", ESP},
    {"EAX", EAX},
    {"EBX", EBX},
    {"ECX", ECX},
    {"EIP", EIP},
    {"FLAGS", FLAGS},
    {"INTERRUPTS", INTERRUPTS},
    {"OUT_PORT", OUT_PORT}
};

Container::Container(const vm_mem b, t_handler th) : _tickHandler(std::move(th)), _bytes(b)
{
};

void Container::setInterruptHandler(const std::byte interrupt, const t_handler handler)
{
	_intHandlers[interrupt] = handler;
}


void Container::saveBytes(const std::string_view name) {
	std::ofstream file(static_cast<std::string>(name), std::ios::binary);
    const size_t count = _size / sizeof(std::byte);
    file.write(reinterpret_cast<char*>(&_bytes[0]), count*sizeof(std::byte));
    file.close();
}

void Container::seek(address addr) {
	pointer = addr;
}

address Container::readAddress()
{
	return address{ readInt() };
}

unsigned int Container::readInt() {
	auto b = _bytes;
	const auto n = (static_cast<int>(b[pointer.dst]) << 24) |
		(static_cast<int>(b[pointer.dst + 1]) << 16) |
		(static_cast<int>(b[pointer.dst + 2]) << 8) |
		(static_cast<int>(b[pointer.dst + 3]));
	pointer += INT_SIZE;
	return n;
}

int Container::readSignedInt() {
	auto b = _bytes;
	const auto n = (static_cast<int>(b[pointer.dst]) << 24) |
		(static_cast<int>(b[pointer.dst + 1]) << 16) |
		(static_cast<int>(b[pointer.dst + 2]) << 8) |
		(static_cast<int>(b[pointer.dst + 3]));
	pointer += INT_SIZE;
	return n;
}

void Container::writeAddress(const address n) {
	writeInt(n.dst);
}

void Container::writeInt(const int n) {
	_bytes[pointer.dst] = static_cast<std::byte>((n >> 24) & 0xFF);
	pointer++;
	_bytes[pointer.dst] = static_cast<std::byte>((n >> 16) & 0xFF);
	pointer++;
	_bytes[pointer.dst] = static_cast<std::byte>((n >> 8) & 0xFF);
	pointer++;
	_bytes[pointer.dst] = static_cast<std::byte>(n & 0xFF);
	pointer++;
}

address Container::writeCode(std::byte opcode, address arg1, unsigned int arg2) {
	const auto local_pointer = pointer;
	writeByte(opcode);
	writeAddress(arg1);
	writeInt(arg2);
	return local_pointer;
}

address Container::writeCode(std::byte opcode, address arg1, address arg2) {
	const auto local_pointer = pointer;
	writeByte(opcode);
	writeAddress(arg1);
	writeAddress(arg2);
	return local_pointer;
}

address Container::writeCode(std::byte opcode, address arg1) {
	const auto local_pointer = pointer;
	writeByte(opcode);
	writeAddress(arg1);
	return local_pointer;
}

address Container::writeCode(std::byte opcode, address arg1, const std::byte arg2) {
	const auto local_pointer = pointer;
	writeByte(opcode);
	writeAddress(arg1);
	writeByte(arg2);
	return local_pointer;
}

address Container::writeCode(const std::byte opcode, const std::byte arg1) {
	const auto local_pointer = pointer;
	writeByte(opcode);
	writeByte(arg1);
	return local_pointer;
}

address Container::writeCode(const std::byte opcode, const int arg1) {
	const auto local_pointer = pointer;
	writeByte(opcode);
	writeInt(arg1);
	return local_pointer;
}

address Container::writeCode(const std::byte opcode) {
	const auto local_pointer = pointer;
	writeByte(opcode);
	return local_pointer;
}

void Container::init(unsigned int size) {
	_size = size; //TODO
    _bytes.reserve(_size);
    _bytes.assign(_size, std::byte{0x0});

	pointer = address::BEGIN;
	seek(ESP);
	writeInt(_size);
	writeHeader();

	/*
	seek(EAX); writeInt(0x0);
	seek(EBX); writeInt(0x0);
	seek(ECX); writeInt(0x0);
	seek(FLAGS); writeByte(static_cast<std::byte>(0b00000000));
	seek(OUT_PORT); writeInt(0xffffffff);
	*/
}

void Container::writeByte(const std::byte ch) {
	_bytes[pointer.dst] = ch;
	pointer++;
}

void Container::writeHeader() {
	seek(address::BEGIN);
	writeByte(static_cast<std::byte>('V'));
	writeByte(static_cast<std::byte>('V'));
	writeByte(static_cast<std::byte>('M'));

	writeByte(version);
	writeByte(static_cast<std::byte>(address::CODE.dst));
}

std::byte Container::readByte() {
	const auto ch = _bytes[pointer.dst];
	pointer++;
	return ch;
}

bool Container::checkFlag(const std::byte mask)
{
	const auto local_pointer = pointer;
	seek(FLAGS);
	const auto flag = readByte();
	seek(local_pointer);
	return static_cast<bool>(flag & mask);
}

std::byte Container::getState()
{
	const auto local_pointer = pointer;
	seek(STATE);
	const auto state = readByte();
	seek(local_pointer);
	return state;
}

void Container::setState(const std::byte state)
{
	const auto local_pointer = pointer;
	seek(STATE);
	writeByte(state);
	seek(local_pointer);
}

void Container::setFlag(const std::byte flag, const bool value)
{
	const auto local_pointer = pointer;
	seek(FLAGS);
	auto flags = readByte();
	if (value) {
		flags |= flag;
	}
	else {
		flags &= ~flag;
	}
	seek(FLAGS);
	writeByte(flags);
	seek(local_pointer);
}

void Container::checkInterruption()
{
	const auto local_pointer = pointer;
	if (checkFlag(INTF))
	{
		seek(INTERRUPTS);
		const auto interrupt = readByte();
		printIRQ(interrupt);
		if (interrupt == INT_END)
		{
			setState(STATE_END);
		}
		else if (_intHandlers.count(interrupt) == 1)
		{
			_intHandlers[interrupt](_bytes, pointer.dst);
		}
		setFlag(INTF, false);
	}
	seek(local_pointer);
}

void Container::setReg(const address reg, const address value)
{
	const auto local_pointer = pointer;
	seek(reg);
	writeAddress(value);
	seek(local_pointer);
}

void Container::setReg(const address reg, const int value)
{
	const auto local_pointer = pointer;
	seek(reg);
	writeInt(value);
	seek(local_pointer);
}

address Container::readRegAddress(const address reg)
{
	const auto local_pointer = pointer;
	seek(reg);
	const auto value = readAddress();
	seek(local_pointer);
	return value;
}

int Container::readRegInt(const address reg)
{
	const auto local_pointer = pointer;
	seek(reg);
	const auto value = readInt();
	seek(local_pointer);
	return value;
}

address Container::execStart()
{
	setState(STATE_EXEC);
	seek(CO_ADDR);
	auto offset = readByte();
	const auto local_pointer = address{ static_cast<unsigned int>(offset) };
	seek(local_pointer);
	return local_pointer;
}

void Container::execCode() {
	fmt::print("= ADDR ==|====== INSTRUCTION =====|= FLAGS ==|===== VARIABLES ====\n");
	fmt::print("         |                        |          |                    \n");
	auto local_pointer = execStart();

	while (getState() == STATE_EXEC) {
		setReg(EIP, local_pointer);

		local_pointer = execStep(local_pointer);
	}
	fmt::print("         |                        |          |                    \n");
	fmt::print("==================================================================\n\n");
}

void Container::execCode(address local_pointer) {
	while (getState() == STATE_EXEC) {
		local_pointer = execStep(local_pointer);
	}
	fmt::print("         |                        |          |                    \n");
	fmt::print("==================================================================\n\n");
}

address Container::execStep(address local_pointer)
{
	setReg(EIP, local_pointer);
	const auto opcode = readByte();

	auto spec = std::find_if(specs.begin(), specs.end(), [&](opSpec s)
	{
		return s.opcode == opcode;
	});
	if (spec != specs.end())
	{
		current_spec_type = (*spec).type;
	}
	local_pointer++;
	//TODO: make map with opcodes
	if (opcode == MOV_mm) {
		local_pointer = MOV_mm_func(local_pointer);
	}
	else if (opcode == MOV_mc) {
		local_pointer = MOV_mc_func(local_pointer);
	}
	else if (opcode == ADD_mm) {
		local_pointer = ADD_mm_func(local_pointer);
	}
	else if (opcode == ADD_mc) {
		local_pointer = ADD_mc_func(local_pointer);
	}
	else if (opcode == SUB_mm) {
		local_pointer = SUB_mm_func(local_pointer);
	}
	else if (opcode == SUB_mc) {
		local_pointer = SUB_mc_func(local_pointer);
	}
	else if (opcode == CMP_mm) {
		local_pointer = CMP_mm_func(local_pointer);
	}
	else if (opcode == CMP_mc) {
		local_pointer = CMP_mc_func(local_pointer);
	}
	else if (opcode == JNE_a) {
		local_pointer = JNE_a_func(local_pointer);
	}
	else if (opcode == JNE_r) {
		local_pointer = JNE_r_func(local_pointer);
	}
	else if (opcode == OUTPUT) {
		local_pointer = OUT_func(local_pointer);
	}
	else if (opcode == INTERRUPT) {
		local_pointer = INT_func(local_pointer);
	}
	else if (opcode == NOP) {
		local_pointer = NOP_func(local_pointer);
	}
	else if (opcode == PUSH_m) {
		local_pointer = PUSH_m_func(local_pointer);
	}
	else if (opcode == PUSH_c) {
		local_pointer = PUSH_c_func(local_pointer);
	}
	else if (opcode == POP) {
		local_pointer = POP_func(local_pointer);
	}
	else if (opcode == JMP_a) {
		local_pointer = JMP_a_func(local_pointer);
	}
	else if (opcode == JMP_r) {
		local_pointer = JMP_r_func(local_pointer);
	}
	else if (opcode == INC) {
		local_pointer = INC_func(local_pointer);
	}
	else if (opcode == DEC) {
		local_pointer = DEC_func(local_pointer);
	}
	checkInterruption();
	_tickHandler(_bytes, pointer.dst);
	if (pointer.dst >= BUF_SIZE)
	{
		//TODO: implement irq and error handler
		setState(STATE_ERROR);
	}

	const auto next_opcode = readByte();
	seek(local_pointer);
	spec = std::find_if(specs.begin(), specs.end(), [&](opSpec s)
	{
		return s.opcode == next_opcode;
	});
	if (spec != specs.end())
	{
		next_spec_type = (*spec).type;
	}
	return local_pointer;

}

std::vector<std::string> split(std::string strToSplit, char delimeter)
{
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimeter))
    {
       splittedStrings.push_back(item);
    }
    return splittedStrings;
}

bool isReservedMem(std::string arg) {
    for_each(arg.begin(), arg.end(), [](char& in){ in = ::toupper(in); });
    auto it = reserved_addresses.find(arg);
    return it != reserved_addresses.end();
}

int get_spec_length(opSpec spec) {
    auto real_length = 1;
    switch (spec.type)
    {
    case opSpec::MM:
    case opSpec::MC:
        real_length = OP_long_length;
        break;
    case opSpec::MB:
        real_length = OP_med_ex_length;
        break;
    case opSpec::M:
    case opSpec::C:
        real_length = OP_med_length;
        break;
    case opSpec::B:
        real_length = OP_short_length;
        break;
    case opSpec::Z: break;
    default:;
    }
    return real_length;
}

std::vector<instruction> Container::compile(std::string filename)
{
	std::vector<instruction> code = {};
    std::ifstream vvmc_file(filename);
    std::string line;
	const auto temp_pointer = pointer;

	seek(CO_ADDR);
	auto offset = readByte();
	auto local_pointer = address{ static_cast<unsigned int>(offset) };
	seek(address::CODE);

    std::string last_label = "";
    std::map<std::string, std::unique_ptr<instruction>> pending_jumps;
    if (vvmc_file.is_open())
    {

        auto n = 0;
        while ( getline (vvmc_file, line) )
        {
        if(line[0] == '#') {
            continue;
        }

        if(line.back() == ':') {
            line.erase(std::find_if(line.rbegin(), line.rend(), [](int ch) {
                return ch != ':';
            }).base(), line.end());
            last_label = line;
            continue;
        }
        auto tokens = split(line, ' ');
        auto op = tokens.front();
        std::string arg1, arg2;
        auto specType = opSpec::Z;
        op_arg parsed_arg1;
        op_arg parsed_arg2 ;
        bool pending = false;

        if (tokens.size() > 1) {
            arg1 = tokens[1];
            // parsed_arg1 = parse_arg(arg1);
            if (isReservedMem(arg1)) {
                specType = opSpec::M;
                parsed_arg1 = address{reserved_addresses[arg1]};
            } else {
                specType = opSpec::C;
                bool parsed = false;
                try {
                    parsed_arg1 = std::stoul(arg1, nullptr, 16);
                    parsed = true;
                } catch (std::invalid_argument e) {
                    try {
                        parsed_arg1 = std::stoul(arg1, nullptr, 10);
                        parsed = true;
                    } catch (std::invalid_argument e) {
                        parsed = false;
                    }
                }
                if (parsed && std::get<unsigned int>(parsed_arg1) < 256) {
                    specType = opSpec::B;
                }
                if (op == "JMP") {
                    specType = opSpec::M;
                    parsed_arg1 = address{0x0};
                    pending = true;
                }
            }
        }
        if (tokens.size() > 2) {
            arg2 = tokens[2];
            if (isReservedMem(arg2) && specType == opSpec::M) {
                specType = opSpec::MM;
                parsed_arg2 = address{reserved_addresses[arg2]};
            } else {
                specType = opSpec::MC;

                bool parsed = false;
                try {
                    parsed_arg2 = std::stoul(arg2, nullptr, 16);
                    parsed = true;
                } catch (std::invalid_argument e) {
                    try {
                        parsed_arg2 = std::stoul(arg2, nullptr, 10);
                        parsed = true;
                    } catch (std::invalid_argument e) {
                        parsed = false;
                    }
                    if (parsed && std::get<unsigned int>(parsed_arg2) < 256) {
                        specType = opSpec::MB;
                    }
                }
            }
        }

		const auto spec = std::find_if(specs.begin(), specs.end(), [&](opSpec s) {
			return s.name == op && s.type == specType;
		});

        if (spec != specs.end()) {
            std::array<std::byte, OP_long_length> mem{ std::byte{0x0} };
            instruction i{ local_pointer, *spec, mem };
            auto real_length = get_spec_length(*spec);
            local_pointer += real_length;

            i.arg1 = parsed_arg1;
            i.arg2 = parsed_arg2;
            i.aliases.push_back(fmt::format("{}", n));
            if (last_label != "") {
                i.aliases.push_back(last_label);
                last_label = "";
            }

            code.push_back(i);
            if (pending) {
                pending_jumps.insert(std::pair(arg1, std::make_unique<instruction>(i)));
            }

            switch (spec->type)
            {
            case opSpec::MM:
                writeCode((*spec).opcode, std::get<address>(parsed_arg1), std::get<address>(parsed_arg2));
                break;
            case opSpec::MC:
                writeCode((*spec).opcode, std::get<address>(parsed_arg1), std::get<unsigned int>(parsed_arg2));
                break;
            case opSpec::MB:
                writeCode((*spec).opcode, std::get<address>(parsed_arg1), std::get<std::byte>(parsed_arg2));
                break;
            case opSpec::M:
                writeCode((*spec).opcode, std::get<address>(parsed_arg1));
                break;
            case opSpec::C:
                writeCode((*spec).opcode, std::get<unsigned int>(parsed_arg1));
                break;
            case opSpec::B:
                writeCode((*spec).opcode, std::get<std::byte>(parsed_arg1));
                break;
            case opSpec::Z:
                writeCode((*spec).opcode);
                break;
            default:;
            }

        } else {
            std::cout << "Unable to parse line: " << line << std::endl;;
            std::cout << op << " " << specType << std::endl;;
        }
        n++;
        }

        instruction i{ local_pointer, INT_spec };
        auto real_length = get_spec_length(INT_spec);

        i.arg1 = INT_END;
        i.aliases.push_back(fmt::format("{}", n));

        code.push_back(i);
        _INT(INT_END);
        vvmc_file.close();
    }
    else std::cout << "Unable to open file";

    fmt::print("\n");
    for(auto const &jump : pending_jumps) {
        auto label = jump.first;
        auto i = *(jump.second.get());
        fmt::print("{} >> {} >> ", i, label);
		const auto dst = std::find_if(code.begin(), code.end(), [&](instruction ins) {
			return std::find(ins.aliases.begin(), ins.aliases.end(), label) != ins.aliases.end();
		});
        if (dst != code.end()) {
            fmt::print("{}\n", *dst);
            i.arg1 = (*dst).offset;
            fmt::print("{}\n", i);
            seek(i.offset + 1);
            writeAddress((*dst).offset);
        } else fmt::print("not found");
    }

    for(auto i : code) {
        std::cout << i << std::endl;
    }

    return code;
}

std::vector<instruction> Container::disassemble()
{
	const auto temp_pointer = pointer;
	std::vector<instruction> code = {};
	seek(CO_ADDR);
	auto offset = readByte();
	auto local_pointer = address{ static_cast<unsigned int>(offset) };
    auto n = 0;
	while (local_pointer.dst < BUF_SIZE)
	{
		seek(local_pointer);
		const auto opcode = readByte();
		const auto spec = std::find_if(specs.begin(), specs.end(), [&](opSpec s) {
			return s.opcode == opcode;
		});
		if (spec == specs.end()) {
			fmt::print("Unknown opcode");
			break;
		}
		std::array<std::byte, OP_long_length> mem{ std::byte{0x0} };
		instruction i{ local_pointer, *spec, mem };
		local_pointer++;
        auto real_length = get_spec_length(*spec);
		local_pointer--;
		for (auto n = 0; n < real_length; n++)
		{
			mem[n] = _bytes[local_pointer.dst];
			local_pointer++;
		}
		i.mem = mem;
        local_pointer -= real_length - 1;
        seek(local_pointer);

		switch (i.spec.type) {
		case opSpec::MM:
            i.arg1 = readAddress();
            i.arg2 = readAddress();
			break;
		case opSpec::MB:
            i.arg1 = readAddress();
            i.arg2 = readByte();
			break;
		case opSpec::MC:
            i.arg1 = readAddress();
            i.arg2 = readInt();
			break;
		case opSpec::M:
            i.arg1 = readAddress();
			break;
		case opSpec::C:
            i.arg1 = readInt();
			break;
		case opSpec::B:
            i.arg1 = readByte();
			break;
		case opSpec::Z:
			break;
		default:;
		}
        local_pointer = pointer;

        i.aliases.push_back(fmt::format("{}", n));
        n++;
		code.push_back(i);

		if (opcode == INT_spec.opcode && i.mem[1] == INT_END)
		{
			break;
		}
        // local_pointer += real_length + 1;
        // seek(local_pointer);

	}
	seek(temp_pointer);
	return code;
}
