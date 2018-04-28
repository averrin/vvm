#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include <ostream>
#include <fstream>
#include <iterator>
#include <sstream>
#include <thread>
#include <utility>
#include <experimental/filesystem>

#include "format.h"
#include "rang.hpp"

#include "imgui_memory_editor.h"
#include "vvm/application.hpp"

#include "zep/src/imgui/editor_imgui.h"
#include "zep/src/imgui/display_imgui.h"

using namespace Zep;
namespace fs = std::experimental::filesystem;

std::string get_selfpath() {
    int bl;
#ifdef _WIN32
    char buff[MAX_PATH];
    GetModuleFileName(NULL, buff, sizeof(buff));
#else
    char buff[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);
    if (len != -1) {
      buff[len] = '\0';
    }
#endif
    auto path = fs::path(buff);
    std::cout << path << std::endl;
    return path.parent_path().string();
}

void printHandler(vm_mem bytes, unsigned int pointer) {
	auto addr = Core::readInt(bytes, ECX.dst);
	auto ch = bytes[addr];
	std::cout << rang::fg::cyan << "  >>   " << rang::style::reset;
	while (static_cast<char>(ch) != '$') {
		std::cout << static_cast<char>(ch);
		addr++;
		ch = bytes[addr];
	}
	std::cout << std::endl << std::flush;
}

void App::tickHandler(vm_mem b, unsigned int pointer) {
	if (static_cast<bool>(b[FLAGS.dst] & OUTF)) {
		const auto n = Core::readInt(b, OUT_PORT.dst);
		b[FLAGS.dst] &= ~OUTF;
		output.push_back(static_cast<char>(n));
	}
	ticks++;
	dis_code = analyzer.disassemble(core.get());
}

void App::updateCode() {
	dis_code = analyzer.disassemble(core.get());
}

App::App(std::string v, std::string f) : VERSION(std::move(v)),input_file(std::move(f)) {
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

    ImGui::CreateContext();
	auto& io = ImGui::GetIO();
    path = get_selfpath();
	io.Fonts->AddFontFromFileTTF(fmt::format("{}/font.ttf", path).c_str(), 15.0f);

    io.KeyMap[ImGuiKey_Space] = sf::Keyboard::Return;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	window = new sf::RenderWindow(sf::VideoMode(1600, 900), "",
		sf::Style::Default, settings);

    spEditor = std::make_unique<ZepEditor_ImGui>();

	window->setVerticalSyncEnabled(true);
	ImGui::SFML::Init(*window);
	char window_title[255] = "Vortex VM";

	window->setTitle(window_title);
	window->resetGLStates();

	core = std::make_unique<Core>(Core(
		{ std::byte{0x0} }, [&](vm_mem b, unsigned int pointer) { tickHandler(b, pointer); }));
	core->setInterruptHandler(INT_PRINT, printHandler);
    analyzer = analyzer::Analyzer();

    auto filename = fs::absolute(fs::path(input_file)).string();
    std::cout << "Load file: " << rang::fg::green << filename << rang::style::reset << std::endl;
    auto vm_filename = fmt::format("{}/{}.vvm", path, fs::path(filename).stem().string());
    loadFileText(filename);

	statusMsg = "VVM started.";
	core->init(256);
    dis_code = analyzer.parseFile(filename);
    core->compile(dis_code);
	core->saveBytes(vm_filename);
	statusMsg = "VVM inited.";
}

void App::loadFileText(std::string filename) {
    ZepBuffer* pBuffer = spEditor->AddBuffer(fs::path(filename).filename());

    std::string code_str;
    std::string line;
    std::ifstream vvmc_file(filename);
    if (vvmc_file.is_open())
    {
        while ( getline (vvmc_file, line) )
        {
            line.append("\n");
            code_str.append(line);
        }
        vvmc_file.close();
    }

    else std::cout << "Unable to open file";

    pBuffer->SetText(code_str.c_str());
}

void App::processEvent(sf::Event event) {
	ImGui::SFML::ProcessEvent(event);

	switch (event.type) {
	case sf::Event::KeyPressed:
		processKey(event);
		break;
	case sf::Event::Closed:
		window->close();
		break;
	}
}

void App::drawMainWindow() {
	ImGui::Begin("Main window");

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
		1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::Text("Window size: w:%d h:%d", window->getSize().x,
		window->getSize().y);

	ImGui::Button("Open vm file");

	ImGui::End();
}

void App::drawCodeWindow() {
	ImGui::Begin("Parsed code");

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

	for (auto i : dis_code) {
		std::string ind = args::detail::Join(i.aliases, ", ");
		if (current_pointer == i.offset) {
			ind = ">";
		}
		ImGui::PushItemWidth(20);
		ImGui::TextColored(ImVec4(0.1f, 1.0f, 0.1f, 1.0f), "%s", ind.c_str());
		ImGui::PopItemWidth();
		ImGui::NextColumn();
		if (current_pointer == i.offset) {
			ImGui::TextColored(ImVec4(0.1f, 1.0f, 0.1f, 1.0f),
				"%s", fmt::format("{}", i.offset).c_str());
		}
		else {
			ImGui::Text("%s", fmt::format("{}", i.offset).c_str());
		}
		ImGui::NextColumn();
		ImGui::Text("%s", fmt::format("{} [{:02X}]", i.spec.name, static_cast<unsigned char>(i.spec.opcode)).c_str());
		ImGui::NextColumn();
		std::string arg;
		std::string arg2;

		switch (i.spec.type) {
		case opSpec::MM:
			arg = fmt::format("{}", std::get<address>(i.arg1));
			arg2 = fmt::format("{}", std::get<address>(i.arg2));
			break;
		case opSpec::MB:
			arg = fmt::format("{}", std::get<address>(i.arg1));
			arg2 = fmt::format(" {:02X}", static_cast<char>(std::get<std::byte>(i.arg2)));
			break;
		case opSpec::MC:
			arg = fmt::format("{}", std::get<address>(i.arg1));
			arg2 = fmt::format(" {:08X}", std::get<unsigned int>(i.arg2));
			break;
		case opSpec::M:
			arg = fmt::format("{}", std::get<address>(i.arg1));
			break;
		case opSpec::C:
			arg = fmt::format(" {:08X}", std::get<unsigned int>(i.arg1));
			break;
		case opSpec::B:
			arg = fmt::format(" {:02X}", static_cast<unsigned int>(std::get<std::byte>(i.arg1)));
			break;
		case opSpec::Z:
			break;
		default:;
		}

		ImGui::Text("%s", arg.c_str());
		ImGui::NextColumn();
		ImGui::Text("%s", arg2.c_str());
		ImGui::NextColumn();
	}
	ImGui::Columns(1);

	ImGui::End();
}

void App::step() {
	setStatusMessage("Step");
	if (current_pointer == address::CODE) {
		core->execStart();
	}
	current_pointer = core->execStep(current_pointer);
	if (core->getState() == STATE_END) {
		current_pointer = address::CODE;
	}
}

void App::rerun() {
	reset();
	setStatusMessage("Rerun");
	core->execCode();
}

void App::reset() {
	setStatusMessage("Reset");
	std::fill(core->_bytes.begin(), core->_bytes.end(), std::byte{ 0x0 });
	core->init(256);
	core->seek(CODE_OFFSET);
    auto filename = "example.vvmc";
    dis_code = analyzer.parseFile(filename);
	current_pointer = address::CODE;
}

void App::run() {
	setStatusMessage("Run");
	if (current_pointer == address::CODE) {
		core->execCode();
	}
	else {
		core->execCode(current_pointer);
	}
	current_pointer = address::CODE;
}

void App::clearStatusMessage() {
	show_status = false;
	if (sm_t.joinable()) sm_t.join();
	show_status = true;
	statusMsg = "";
}

void App::setStatusMessage(const std::string_view msg) {
	show_status = false;
	if (sm_t.joinable()) sm_t.join();
	show_status = true;
	statusMsg = msg;
	sm_t = std::thread(
		[&](int delay) {
		auto n = 0;
		while (n < delay && show_status) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			n++;
		}
		statusMsg = "";
	},
		status_message_dalay);
}

void App::drawControlWindow() {
	ImGui::Begin("Controls");
	ImGui::Text("Instructions: %zu", dis_code.size());
	if (ImGui::Button("run")) {
		run();
	}
	ImGui::SameLine(40);
	if (ImGui::Button("step")) {
		step();
	}
	ImGui::SameLine(80);
	if (ImGui::Button("reset")) {
		reset();
	}
	ImGui::SameLine(130);
	if (ImGui::Button("rerun")) {
		rerun();
	}
	ImGui::End();
}

void App::drawRegWindow() {
	ImGui::Begin("Registers");

	ImGui::Columns(3, "registers");
	ImGui::Separator();
	ImGui::Text("Name");
	ImGui::NextColumn();
	ImGui::Text("Addr");
	ImGui::NextColumn();
	ImGui::Text("Val");
	ImGui::Separator();

    //TODO: use analyzer reserves_addresses
	std::array<address, 6> regs = { EAX, EBX, ECX, EIP, ESP, OUT_PORT };
	std::array<std::string, 6> names = { "EAX", "EBX", "ECX",
										"EIP", "ESP", "OUT_PORT" };

	auto n = 0;
	for (auto r : regs) {
		ImGui::NextColumn();
		ImGui::Text("%s", names[n].c_str());
		ImGui::NextColumn();
		ImGui::Text("%s", fmt::format("{}", r).c_str());
		ImGui::NextColumn();
		ImGui::Text("%s", fmt::format("{:08X}", core->readRegInt(r)).c_str());
		n++;
	}
	ImGui::Separator();
	ImGui::Columns(1);

	ImGui::Text("\nFlags:");
	std::array<std::byte, 3> flags = { ZF, OUTF, INTF };
	std::array<std::string, 3> flag_names = { "ZF", "OUTF", "INTF" };
	n = 0;
	for (auto f : flags) {
		ImGui::Text(
			"%s", fmt::format("{}: {}", flag_names[n], core->checkFlag(f)).c_str());
		n++;
	}

	ImGui::End();
}

void App::serve() {
	sf::Clock delta_clock;

	ImGui::StyleColorsDark();
	while (window->isOpen()) {
		sf::Event event{};
		while (window->pollEvent(event)) {
			processEvent(event);
		}

		window->clear(sf::Color(40, 40, 40));
		ImGui::SFML::Update(*window, delta_clock.restart());

		mem_edit.DrawWindow(core.get());
		drawMainWindow();
		drawRegWindow();
		drawControlWindow();
		drawCodeWindow();
		drawHelpWindow();

        ImGui::Begin("Code", nullptr, ImVec2(1024, 768));
        spEditor->Display(toNVec2f(ImGui::GetCursorScreenPos()), toNVec2f(ImGui::GetContentRegionAvail()));
        ImGui::End();


		showStatusbar();

		ImGui::SFML::Render(*window);
		window->display();
	}
	ImGui::SFML::Shutdown();

	wait_for_key = false;
	st.join();
	clearStatusMessage();
}

void App::drawHelpWindow() {
	ImGui::Begin("Help");

	ImGui::Columns(2, "Controls");
	for (auto action : actions) {
		std::ostringstream oss;
		std::copy(action.keys.begin(), action.keys.end(), std::ostream_iterator<std::string>(oss, " -> "));

		if (lastFiredAction != nullptr && lastFiredAction->funcName == action.funcName) {
			ImGui::TextColored(ImVec4(0.9f, 0.8f, 0.2f, 1.0f), "%s", action.funcName.c_str());
			ImGui::NextColumn();
			ImGui::TextColored(ImVec4(0.9f, 0.8f, 0.2f, 1.0f), "%s", oss.str().substr(0, oss.str().size() - 4).c_str());
			ImGui::Separator();
			ImGui::NextColumn();
		}
		else {
			ImGui::Text("%s", action.funcName.c_str());
			ImGui::NextColumn();
			ImGui::Text("%s", oss.str().substr(0, oss.str().size() - 4).c_str());
			ImGui::Separator();
			ImGui::NextColumn();
		}
	}
	ImGui::Columns(1);

	ImGui::End();
}

void App::showStatusbar() {
	const auto h = 25.f;
	const auto window_pos = ImVec2(0.f, ImGui::GetIO().DisplaySize.y - h);
	const auto window_pos_pivot = ImVec2(1.0f, 0.0f);
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 2));
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x - 10, h));
	ImGui::PushStyleColor(
		ImGuiCol_WindowBg,
		ImVec4(0.0f, 0.0f, 0.0f, 0.3f)); // Transparent background
	if (ImGui::Begin("Statusbar", (bool *)true,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoScrollbar)) {
		ImGui::AlignTextToFramePadding();
		if (!keySeq.empty()) {
			if (wait_for_key) {
				ImGui::TextColored(ImVec4(0.9f, 0.8f, 0.2f, 1.0f), "Key: %s",
					keySeq.c_str());
			}
		}
		else {
			if (lastFiredAction != nullptr && !statusMsg.empty()) {
				ImGui::TextColored(ImVec4(0.6f, 0.8f, 0.2f, 1.0f), "[%s]: ",
					lastFiredAction->funcName.c_str());
				ImGui::SameLine(100);
			}
			ImGui::Text("%s", statusMsg.c_str());
		}
		ImGui::End();
	}
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}
