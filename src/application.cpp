#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <iostream>
#include <cstdlib>
#include "format.h"
#include "rang.hpp"
#include "vvm/container.hpp"
#include "vvm/constants.hpp"
#include "imgui_memory_editor.h"

std::vector<char> output;
int ticks = 0;

unsigned int readInt(const std::byte* bytes, const unsigned int pointer)
{
	return static_cast<int>(
		(bytes[pointer] << 24) |
		(bytes[pointer + 1] << 16) |
		(bytes[pointer + 2] << 8) |
		(bytes[pointer + 3]));

}

void tickHandler(std::byte* bytes, unsigned int pointer) {
	if (static_cast<bool>(bytes[FLAGS.dst] & OUTF)) {
		const auto n = readInt(bytes, OUT_PORT.dst);
		bytes[FLAGS.dst] &= ~OUTF;
		output.push_back(static_cast<char>(n));
	}
	ticks++;
}


void printHandler(const std::byte* bytes, unsigned int pointer)
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
	std::byte code[BUF_SIZE] = { std::byte{0x0} };
	Container* mem;


int run_vm() {
	mem->_MOV(EAX, 0x11111111);
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
	mem->_JNE(-OP_long_length-OP_med_length); // pre-prev opcode
	mem->_JNE(address::CODE); //pass here
	
	mem->_INT(INT_END);
	
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

	mem = new Container(code, tickHandler);
	mem->init();
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


	void serve() {
		sf::Clock deltaClock;
		MemoryEditor::u8 cc[BUF_SIZE] = { 0x0 };
		for (auto n=0; n< BUF_SIZE; n++)
		{
			cc[n] = (char)code[n];
		}
		static MemoryEditor mem_edit;

		ImGui::StyleColorsDark();
		while (window->isOpen()) {
			sf::Event event;
			while (window->pollEvent(event)) {
				processEvent(event);
			}

			window->clear(sf::Color(100,100,100));
			ImGui::SFML::Update(*window, deltaClock.restart());
			mem_edit.DrawWindow("VM State", cc, BUF_SIZE);
			drawMainWindow();


			ImGui::SFML::Render(*window);
			window->display();

		}
		ImGui::SFML::Shutdown();
	}
};