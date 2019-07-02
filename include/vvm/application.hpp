#pragma once
#include <SDL.h>
#include <imgui_memory_editor.h>
#include <thread>
#include "vvm/analyzer/code_instruction.hpp"
#include "vvm/analyzer.hpp"
// #define ZEP_SINGLE_HEADER_BUILD
// #include <zep.h>


#include "zep/mcommon/animation/timer.h"

#undef max

#include "zep/imgui/display_imgui.h"
#include "zep/imgui/editor_imgui.h"
#include "zep/mode_standard.h"
#include "zep/mode_vim.h"
#include "zep/theme.h"
#include "zep/tab_window.h"
#include "zep/window.h"

#include "vvm/devices/video.hpp"
#include "vvm/devices/rng.hpp"

struct sequence {
	std::string funcName;
	std::vector<std::string> keys;
	std::function<void()> func;
};

class App {
	void drawPicWindow(std::shared_ptr<VideoDevice> video);
	void showStatusbar();
	void reset();
	void clearStatusMessage();
	void setStatusMessage(std::string_view msg);
	void run();
	void compile();
	void rerun();
	void step();
	void updateCode();

	std::shared_ptr<VideoDevice> video;
	std::shared_ptr<RngDevice> rng;
	MemoryEditor mem_edit;
	MemoryEditor pic_edit;
	MemoryEditor rnd_edit;
	std::string VERSION;
	SDL_Window *window;
    SDL_GLContext glcontext;
	std::shared_ptr<Core> core;
    analyzer::Analyzer analyzer;
    // std::unique_ptr<Zep::ZepEditor_ImGui> spEditor;

	analyzer::script dis_code;
	address current_pointer = address::CODE;

	std::vector<char> output;
	int ticks = 0;

	std::string statusMsg = "";
	std::string keySeq = "";
	std::thread st = std::thread([&]() {});
	std::thread sm_t = std::thread([&]() {});
	std::vector<std::string> pressed_seq{};
	sequence *lastFiredAction = nullptr;
	bool wait_for_key = true;
	bool show_status = true;
	const int leader_key_delay = 300;
	const int status_message_dalay = 2000;
	const std::string leader = "Space";

public:
    std::string path;
	std::string input_file;
	// std::vector<sequence> actions{
	// 	sequence{
	// 		"comment",
	// 		{leader, "c", "L"},
	// 		  [&] { setStatusMessage("Comment line! What line?! It isnt emacs!!"); }},
	// 	// sequence{"quit", {"Escape"}, [&] { window->close(); }},
	// 	sequence{"quit", {"shift+q"}, [&] { window->close(); }},
	// 	// sequence{"step", {"j"}, [&] { step(); }},
	// 	sequence{"rerun", {"shift+r"}, [&] { rerun(); }},
	// 	// sequence{"run", {"r"}, [&] { run(); }},
	// 	sequence{"reset", {"control+r"}, [&] { reset(); }},
	// 	  sequence{"disassemble", {"control+d"}, [&] { updateCode(); }},
	// };

	void tickHandler(MemoryContainer b, unsigned int pointer);
	App(std::string v, std::string input_file);
	void resetSeqWait(bool success);
	// void processKey(sf::Event event);
	// void processEvent(sf::Event event);
	void drawMainWindow();
	void drawCodeWindow();
	void drawKeysWindow();
	void drawControlWindow();
	void drawRegWindow();
	// void writeFn(MemoryEditor::u8 *data, size_t off, MemoryEditor::u8 d);
	void serve();
    void loadFileText(std::string filename);
	// std::string getKeyName(const sf::Keyboard::Key key, bool alt, bool control,
		// bool shift);
	// std::string getKeyName(const sf::Keyboard::Key key);
};
