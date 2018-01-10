#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <imgui_memory_editor.h>
#include <thread>

// typedef std::vector<std::string> sequence;
struct sequence {
  std::string funcName;
  std::vector<std::string> keys;
  std::function<void()> func;
};

class App {
  void drawHelpWindow();
  void reset();
  void clearStatusMessage();
  void setStatusMessage(std::string_view msg);
  void run();
  void rerun();
  void step();
  void updateCode();
  void ShowStatusbar();

  std::string VERSION;
  sf::RenderWindow *window;
  vm_mem code = {std::byte{0x0}};
  Container *mem;

  std::vector<instruction> dis_code;
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
  std::vector<sequence> actions{
      sequence{
          "comment",
          {leader, "c", "L"},
            [&] { setStatusMessage("Comment line! What line?! It isnt emacs!!"); }},
      sequence{"quit", {"Escape"}, [&] { window->close(); }},
      sequence{"quit", {"shift+q"}, [&] { window->close(); }},
      sequence{"step", {"j"}, [&] { step(); }},
      sequence{"rerun", {"shift+r"}, [&] { rerun(); }},
      sequence{"run", {"r"}, [&] { run(); }},
      sequence{"reset", {"control+r"}, [&] { reset(); }},
        sequence{"disassemble", {"control+d"}, [&] { updateCode(); }},
  };

  void tickHandler(vm_mem b, unsigned int pointer);
  int run_vm();
  App(std::string v);
  void resetSeqWait(bool success);
  void processKey(sf::Event event);
  void processEvent(sf::Event event);
  void drawMainWindow();
  void drawCodeWindow();
  void drawKeysWindow();
  void drawControlWindow();
  void drawRegWindow();
  void writeFn(MemoryEditor::u8 *data, size_t off, MemoryEditor::u8 d);
  void serve();
  std::string getKeyName(const sf::Keyboard::Key key, bool alt, bool control,
                         bool shift);
  std::string getKeyName(const sf::Keyboard::Key key);
};
