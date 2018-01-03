#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <array>
#include <iostream>
#include <cstdlib>
#include "format.h"
#include "rang.hpp"
#include "vvm/container.hpp"
#include "vvm/constants.hpp"
#include "imgui_memory_editor.h"

std::vector<char> output;
int ticks = 0;

unsigned int readInt(vm_mem b, const unsigned int pointer)
{
	return (static_cast<int>(b[pointer]) << 24) |
		(static_cast<int>(b[pointer + 1]) << 16) |
		(static_cast<int>(b[pointer + 2]) << 8) |
		(static_cast<int>(b[pointer + 3]));

}

unsigned int readInt(std::array<std::byte, OP_long_length> b, const unsigned int pointer)
{
	return (static_cast<int>(b[pointer]) << 24) |
		(static_cast<int>(b[pointer + 1]) << 16) |
		(static_cast<int>(b[pointer + 2]) << 8) |
		(static_cast<int>(b[pointer + 3]));

}



void printHandler(vm_mem bytes, unsigned int pointer)
{
	auto addr = readInt(bytes, ECX.dst);
	auto ch = bytes[addr];
	std::cout << rang::fg::cyan << "  >>   " << rang::style::reset;
	while (static_cast<char>(ch) != '$')
	{
		std::cout << static_cast<char>(ch);
		addr++;
		ch = bytes[addr];
	}
	std::cout << std::endl << std::flush;
}



class Application {
	std::string VERSION;
	sf::RenderWindow *window;
	vm_mem code = { std::byte{0x0} };
	Container* mem;

	std::vector<instruction> dis_code;

	void tickHandler(vm_mem b, unsigned int pointer) {
		if (static_cast<bool>(b[FLAGS.dst] & OUTF)) {
			const auto n = readInt(b, OUT_PORT.dst);
			b[FLAGS.dst] &= ~OUTF;
			output.push_back(static_cast<char>(n));
		}
		ticks++;
		//dis_code = mem->disassemble();
	}

	int run_vm() {
		mem->init();
		mem->seek(CODE_OFFSET);
		mem->_MOV(EAX, 0x111111);
		mem->_MOV(EBX, EAX);
		mem->_ADD(EBX, EAX);
		mem->_ADD(EBX, 0x01);
		mem->_SUB(EBX, 0x05);
		mem->_SUB(EAX, EBX);
		mem->_INC(EAX);
		mem->_JMP(+OP_med_length); //next opcode
		const auto ja = mem->_JMP(address::BEGIN); //dummy jump addr
		const auto na = mem->_NOP();
		mem->seek(ja);
		mem->_JMP(na);
		mem->_PUSH(EAX);
		mem->_PUSH(0x02);
		mem->_POP(EAX);
		mem->_DEC(EAX);
		mem->_CMP(EAX, 0x0);
		mem->_JNE(-OP_long_length - OP_med_length); // pre-prev opcode
		mem->_JNE(address::CODE); //pass here

		mem->_INT(INT_END);

		dis_code = mem->disassemble();
		/*
		mem->saveBytes("init.bin");
		fmt::print("Init state: \n");
		mem->dumpState();

		mem->execCode();

		mem->saveBytes("final.bin");
		fmt::print("Final state: \n");
		mem->dumpState();

		fmt::print("Total ticks: {}\n", ticks);
		*/

		/*
		fmt::print("Output:\n");
		for (auto ch : output) {
			fmt::print("{:c}", ch);
		}
		std::cout << std::endl << std::flush;
	*/

		return EXIT_SUCCESS;
	}

public:
	Application(std::string v) : VERSION(v) {
		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;

		ImGuiIO &io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("./font.ttf", 15.0f);

		window = new sf::RenderWindow(sf::VideoMode(1600, 900), "",
			sf::Style::Default, settings);

		window->setVerticalSyncEnabled(true);
		ImGui::SFML::Init(*window);
		char windowTitle[255] = "Vortex VM";

		window->setTitle(windowTitle);
		window->resetGLStates();

		mem = new Container(code, [&](vm_mem b, unsigned int pointer)
		{
			tickHandler(b, pointer);
		});
		mem->setInterruptHandler(INT_PRINT, printHandler);

		run_vm();

	}

	void processEvent(sf::Event event) {
		sf::Vector2<float> pos =
			window->mapPixelToCoords(sf::Mouse::getPosition(*window));
		ImGui::SFML::ProcessEvent(event);

		switch (event.type) {
		case sf::Event::KeyPressed:
			switch (event.key.code) {
			case sf::Keyboard::Escape:
				window->close();
				break;
			}
			break;
		case sf::Event::Closed:
			window->close();
			break;
		}
	}


	void drawMainWindow() {
		ImGui::Begin("Main window");

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
			1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::Text("Window size: w:%d h:%d", window->getSize().x,
			window->getSize().y);

		ImGui::Button("Open vm file");

		ImGui::End();
	}

	void drawCodeWindow() {
		ImGui::Begin("Code");

		ImGui::Columns(5, "registers");
		ImGui::Separator();
		ImGui::PushItemWidth(20);
		ImGui::Text("");
		ImGui::PopItemWidth();
		ImGui::NextColumn();
		ImGui::Text("Addr");
		ImGui::NextColumn();
		ImGui::Text("Opcode");
		ImGui::NextColumn();
		ImGui::Text("");
		ImGui::NextColumn();
		ImGui::Text("");
		ImGui::Separator();
		ImGui::NextColumn();

		for (auto i : dis_code)
		{
			auto ind = "";
			if (current_pointer == i.offset)
			{
				ind = ">";
			}
			ImGui::PushItemWidth(20);
			ImGui::TextColored(ImVec4(0.1f, 1.0f, 0.1f, 1.0f), ind);
			ImGui::PopItemWidth();
			ImGui::NextColumn();
			if (current_pointer == i.offset)
			{
				ImGui::TextColored(ImVec4(0.1f, 1.0f, 0.1f, 1.0f), fmt::format("{}", i.offset).c_str());
			}
			else
			{
				ImGui::Text(fmt::format("{}", i.offset).c_str());

			}
			ImGui::NextColumn();
			ImGui::Text(i.spec.name.c_str());
			ImGui::NextColumn();
			std::string arg = "";
			std::string arg2 = "";

			switch (i.spec.type)
			{
			case opSpec::MM:
			case opSpec::MC:
				arg = fmt::format("{:08X}", readInt(i.mem, 1));
				arg2 = fmt::format("{:08X}", readInt(i.mem, 1 + INT_SIZE));
				break;
			case opSpec::M:
			case opSpec::C:
				arg = fmt::format("{:08X}", readInt(i.mem, 1));
				break;
			case opSpec::B:
				arg = fmt::format("{:02X}", (unsigned char)i.mem[1]);
				break;
			case opSpec::Z: break;
			default:;
			}

			ImGui::Text(arg.c_str());
			ImGui::NextColumn();
			ImGui::Text(arg2.c_str());
			ImGui::NextColumn();

		}
		ImGui::Columns(1);

		ImGui::End();
	}

	address current_pointer = address::CODE;

	void drawControlWindow() {
		ImGui::Begin("Controls");
		ImGui::Text("Instructions: %d", dis_code.size());
		if (ImGui::Button("run"))
		{
			if (current_pointer == address::CODE)
			{
				mem->execCode();
			}
			else
			{
				mem->execCode(current_pointer);
			}
			current_pointer = address::CODE;
		}
		ImGui::SameLine(40);
		if (ImGui::Button("step"))
		{
			if (current_pointer == address::CODE)
			{
				mem->execStart();
			}
			current_pointer = mem->execStep(current_pointer);
			if (mem->getState() == STATE_END)
			{
				current_pointer = address::CODE;
			}
		}
		ImGui::SameLine(80);
		if (ImGui::Button("rerun"))
		{
			mem->_bytes.fill(std::byte{ 0x0 });
			mem->init();
			run_vm();
			mem->execCode();
		}
		ImGui::End();
	}

	void drawRegWindow() {
		ImGui::Begin("Registers");

		ImGui::Columns(3, "registers");
		ImGui::Separator();
		ImGui::Text("Name");
		ImGui::NextColumn();
		ImGui::Text("Addr");
		ImGui::NextColumn();
		ImGui::Text("Val");
		ImGui::Separator();

		std::array<address, 6> regs = { EAX, EBX, ECX, EIP, ESP, OUT_PORT };
		std::array<std::string, 6> names = { "EAX", "EBX", "ECX", "EIP", "ESP", "OUT_PORT" };

		auto n = 0;
		for (auto r : regs)
		{
			ImGui::NextColumn();
			ImGui::Text(names[n].c_str());
			ImGui::NextColumn();
			ImGui::Text(fmt::format("{}", r).c_str());
			ImGui::NextColumn();
			ImGui::Text(fmt::format("{:08X}", mem->readRegInt(r)).c_str());
			n++;
		}
		ImGui::Separator();
		ImGui::Columns(1);

		ImGui::Text("\nFlags:");
		std::array<std::byte, 3> flags = { ZF, OUTF, INTF };
		std::array<std::string, 3> flag_names = { "ZF", "OUTF", "INTF" };
		n = 0;
		for (auto f : flags)
		{
			ImGui::Text(fmt::format("{}: {}", flag_names[n], mem->checkFlag(f)).c_str());
			n++;
		}

		ImGui::End();
	}

	void writeFn(MemoryEditor::u8* data, size_t off, MemoryEditor::u8 d)
	{
		mem->_bytes[off] = std::byte{ d };
	}

	void serve() {
		sf::Clock deltaClock;
		MemoryEditor::u8 cc[BUF_SIZE] = { 0x0 };
		static MemoryEditor mem_edit;

		ImGui::StyleColorsDark();
		while (window->isOpen()) {
			sf::Event event;
			while (window->pollEvent(event)) {
				processEvent(event);
			}

			for (auto n = 0; n < BUF_SIZE; n++)
			{
				cc[n] = (char)mem->_bytes[n];
			}

			window->clear(sf::Color(100, 100, 100));
			ImGui::SFML::Update(*window, deltaClock.restart());

			//TODO: change colors for different ranges
			//TODO: highlight executed opcode (i want two colors)
			//TODO: implement write fn
			// mem_edit.WriteFn = writeFn;
			/*
			mem_edit.HighlightColor = IM_COL32(25, 255, 25, 40);
			mem_edit.HighlightMin = current_pointer.dst;
			mem_edit.HighlightMax = current_pointer.dst + OP_long_length;
*/
			mem_edit.DrawWindow(mem);
			drawMainWindow();
			drawRegWindow();
			drawControlWindow();
			drawCodeWindow();

			ImGui::SFML::Render(*window);
			window->display();

		}
		ImGui::SFML::Shutdown();
	}
};