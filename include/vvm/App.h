#pragma once
#include <thread>
#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui_memory_editor.h>
#include <SFML/Window/Keyboard.hpp>

// typedef std::vector<std::string> sequence;
struct sequence
{
	std::vector<std::string> keys;
	std::function<void()> func;
};

class App
{
	std::string VERSION;
	sf::RenderWindow *window;
	vm_mem code = { std::byte{0x0} };
	Container* mem;

	std::vector<instruction> dis_code;
	address current_pointer = address::CODE;

	std::vector<char> output;
	int ticks = 0;

	std::string keySeq = "";
	std::thread st = std::thread([&]() {});
	std::vector<std::string> pressed_seq{};
	bool wait_for_key = true;
	const int leader_key_delay = 300;
	const std::string leader = "Space";

public:
	
	void tickHandler(vm_mem b, unsigned int pointer);
	int run_vm();
	App(std::string v);
	void resetSeqWait();
	void processKey(sf::Event event);
	void processEvent(sf::Event event);
	void drawMainWindow();
	void drawCodeWindow();
	void drawKeysWindow();
	void drawControlWindow();
	void drawRegWindow();
	void writeFn(MemoryEditor::u8* data, size_t off, MemoryEditor::u8 d);
	void serve();
	std::string getKeyName(const sf::Keyboard::Key key, bool alt, bool control, bool shift);
	std::string getKeyName(const sf::Keyboard::Key key);
};
