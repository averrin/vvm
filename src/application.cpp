#include <SDL.h>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <experimental/filesystem>
#include <fstream>
#include <imgui/imgui.h>
#include <imgui/imgui/examples/imgui_impl_opengl3.h>
#include <imgui/imgui/examples/imgui_impl_sdl.h>
#include <imgui/imgui/examples/libs/gl3w/GL/gl3w.h> // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <iostream>
#include <iterator>
#include <map>
#include <ostream>
#include <sstream>
#include <stdio.h>
#include <thread>
#include <utility>

#include "yaml-cpp/yaml.h"

#include "format.h"
#include "rang.hpp"

#include "imgui_memory_editor.h"
#include "vvm/application.hpp"
#include "vvm/logger.hpp"

// using namespace Zep;
namespace fs = std::experimental::filesystem;

static Logger logger;

std::string get_selfpath() {
#ifdef _WIN32
  char buff[MAX_PATH];
  GetModuleFileName(NULL, buff, sizeof(buff));
#else
#include <linux/limits.h>
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

void printHandler(MemoryContainer bytes, unsigned int pointer) {
  // auto addr = Core::readInt(bytes, ECX.dst);
  // auto ch = bytes[addr];
  // std::cout << rang::fg::cyan << "  >>   " << rang::style::reset;
  // while (static_cast<char>(ch) != '$') {
  //   std::cout << static_cast<char>(ch);
  //   addr++;
  //   ch = bytes[addr];
  // }
  // std::cout << std::endl << std::flush;
}

void App::tickHandler(MemoryContainer b, unsigned int pointer) {
  // if (static_cast<bool>(b[FLAGS.dst] & OUTF)) {
  //   const auto n = Core::readInt(b, OUT_PORT.dst);
  //   b[FLAGS.dst] &= ~OUTF;
  //   output.push_back(static_cast<char>(n));
  // }
  ticks++;
  // dis_code = analyzer.disassemble(core);
}

void App::updateCode() { dis_code = analyzer.disassemble(core); }

App::App(std::string v, std::string s, std::string f)
    : VERSION(std::move(v)), spec_file(std::move(s)), input_file(std::move(f)) {

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    printf("Error: %s\n", SDL_GetError());
  }

  std::string window_title = fmt::format("Vortex VM [v{}]", VERSION);

  ImGui::CreateContext();

  // Setup window
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_DisplayMode current;
  SDL_GetCurrentDisplayMode(0, &current);
  window = SDL_CreateWindow(window_title.c_str(), SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, 1024, 768,
                            SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL |
                                SDL_WINDOW_RESIZABLE);
  glcontext = SDL_GL_CreateContext(window);
  gl3wInit();

  SDL_GL_SetSwapInterval(1);

  // Setup ImGui binding
  ImGui_ImplSDL2_InitForOpenGL(window, glcontext);
  ImGui_ImplOpenGL3_Init("#version 130");

  auto &io = ImGui::GetIO();
  path = get_selfpath();
  io.Fonts->AddFontFromFileTTF(fmt::format("{}/font.ttf", path).c_str(), 15.0f);

  // io.KeyMap[ImGuiKey_Space] = sf::Keyboard::Return;
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  // spEditor = std::make_unique<Zep::ZepEditor_ImGui>();
  YAML::Node config =
      YAML::LoadFile(fs::absolute(fs::path(spec_file)).string());

  core =
      std::make_shared<Core>(Core([&](MemoryContainer b, unsigned int pointer) {
        tickHandler(b, pointer);
      }));
  core->setInterruptHandler(INT_PRINT, printHandler);
  analyzer = analyzer::Analyzer();

  auto filename = fs::absolute(fs::path(input_file)).string();
  std::cout << "Load file: " << rang::fg::green << filename
            << rang::style::reset << std::endl;
  auto vm_filename =
      fmt::format("{}/{}.vvm", path, fs::path(filename).stem().string());
  loadFileText(filename);

  statusMsg = "VVM started.";
  dis_code = analyzer.parseFile(filename);

  core->compile(dis_code);

  //TODO: construct coreConfig and pass it into Core
  // & move device creation logic into some deviceManager inside core
  for (auto device : config["devices"]) {
    auto type = device["type"].as<std::string>();
    auto id = std::byte{device["id"].as<int>()};
    if (type == "memory") {
      std::shared_ptr<MemoryDevice> memory;
      auto size = 0;
      if (device["size"].as<int>() > 0) {
        size = device["size"].as<int>();
        memory = std::make_shared<MemoryDevice>(id, size);
      } else if (device["path"].as<std::string>() != "") {
        auto filename =
            fs::absolute(fs::path(device["path"].as<std::string>())).string();
        std::cout << "mem filename: " << filename << std::endl << std::flush;
        std::ifstream file(filename);
        if (file) {
          file.seekg(0, std::ios::end);
          std::streampos length = file.tellg();
          file.seekg(0, std::ios::beg);

          std::vector<std::byte> file_content(length);
          file.read((char *)file_content.data(), length);

          memory = std::make_shared<MemoryDevice>(id, length);
          memory->memory = std::make_shared<MemoryContainer>(file_content);
        }

      }
      core->addDevice(memory);
    } else if (type == "rndgen") {
      auto size = device["size"].as<int>();
      auto d = std::make_shared<RngDevice>(id, size, device["min"].as<int>(),
                                           device["max"].as<int>());
      core->addDevice(d);
    } else if (type == "video") {
      auto d = std::make_shared<VideoDevice>(id, device["width"].as<int>(),
                                             device["height"].as<int>());
      core->addDevice(d);
    }
  }

  for (auto d : core->devices) {
    if (d->deviceId == std::byte{0x12}) {
      video = std::dynamic_pointer_cast<VideoDevice>(d);
    }
  }
  core->saveBytes(vm_filename);
  statusMsg = "VVM inited.";
  logger.AddLog("VVM inited.");
}

void App::loadFileText(std::string filename) {
  // Zep::ZepBuffer *pBuffer =
  // spEditor->AddBuffer(fs::path(filename).filename());

  std::string code_str;
  std::string line;
  std::ifstream vvmc_file(filename);
  if (vvmc_file.is_open()) {
    while (getline(vvmc_file, line)) {
      line.append("\n");
      code_str.append(line);
    }
    vvmc_file.close();
  }

  else
    std::cout << "Unable to open file";

  // pBuffer->SetText(code_str.c_str());
}

// void App::processEvent(SDL_Event event) {

// switch (event.type) {
// case sf::Event::KeyPressed:
//   // processKey(event);
//   break;
// case sf::Event::Closed:
//   window->close();
//   break;
// }
// }

void App::drawMainWindow() {
  ImGui::Begin("Main window");

  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

  // ImGui::Text("Window size: w:%d h:%d", window->getSize().x,
  //             window->getSize().y);

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
    std::string ind = analyzer::join(i.aliases, ", ");
    if (current_pointer == i.offset) {
      ind = ">";
    }
    ImGui::PushItemWidth(20);
    ImGui::TextColored(ImVec4(0.1f, 1.0f, 0.1f, 1.0f), "%s", ind.c_str());
    ImGui::PopItemWidth();
    ImGui::NextColumn();
    if (current_pointer == i.offset) {
      ImGui::TextColored(ImVec4(0.1f, 1.0f, 0.1f, 1.0f), "%s",
                         fmt::format("{}", i.offset).c_str());
    } else {
      ImGui::Text("%s", fmt::format("{}", i.offset).c_str());
    }
    ImGui::NextColumn();
    ImGui::Text("%s", fmt::format("{} [{:02X}]", i.spec.name,
                                  static_cast<unsigned char>(i.spec.opcode))
                          .c_str());
    ImGui::NextColumn();
    std::string arg;
    std::string arg2;

    switch (i.spec.type) {
    case op_spec::AA:
      arg = fmt::format("{}", std::get<address>(i.arg1));
      arg2 = fmt::format("{}", std::get<address>(i.arg2));
      break;
    case op_spec::AW:
      arg = fmt::format("{}", std::get<address>(i.arg1));
      arg2 = fmt::format(
          " {:02X}", static_cast<unsigned int>(std::get<std::byte>(i.arg2)));
      break;
    case op_spec::AI:
      arg = fmt::format("{}", std::get<address>(i.arg1));
      arg2 =
          fmt::format(" {:0{}X}", std::get<unsigned int>(i.arg2), INT_SIZE * 2);
      break;
    case op_spec::A:
      arg = fmt::format("{}", std::get<address>(i.arg1));
      break;
    case op_spec::I:
      arg =
          fmt::format(" {:0{}X}", std::get<unsigned int>(i.arg1), INT_SIZE * 2);
      break;
    case op_spec::W:
      arg = fmt::format(" {:02X}",
                        static_cast<unsigned int>(std::get<std::byte>(i.arg1)));
      break;
    case op_spec::Z:
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

  auto filename = fs::absolute(fs::path(input_file)).string();
  dis_code = analyzer.parseFile(filename);
  core->compile(dis_code);
  current_pointer = address::CODE;
  // TODO: clear memory
}

void App::run() {
  setStatusMessage("Run");
  if (current_pointer == address::CODE) {
    core->execCode();
  } else {
    core->execCode(current_pointer);
  }
  current_pointer = address::CODE;
}

void App::clearStatusMessage() {
  show_status = false;
  if (sm_t.joinable())
    sm_t.join();
  show_status = true;
  statusMsg = "";
}

void App::setStatusMessage(const std::string_view msg) {
  show_status = false;
  if (sm_t.joinable())
    sm_t.join();
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
  ImGui::Text("VM size: %d", core->getSize());
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

  if (ImGui::Button("save vm")) {
    auto filename = fs::absolute(fs::path(input_file)).string();
    auto vm_filename =
        fmt::format("{}/{}.vvm", path, fs::path(filename).stem().string());
    core->saveBytes(vm_filename);
  }
  ImGui::SameLine(130);
  if (ImGui::Button("save code")) {
    auto filename = fs::absolute(fs::path(input_file)).string();
    auto vm_filename =
        fmt::format("{}/{}.code.vvm", path, fs::path(filename).stem().string());
    core->code->dump(vm_filename);
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

  // TODO: use analyzer reserves_addresses
  std::array<address, 12> regs = {ESP, EAX, EBX, ECX, EIP, EDI, AL, BL, CL, AH, BH, CH};
  std::array<std::string, 12> names = {"ESP", "EAX", "EBX", "ECX", "EIP",
                                      "EDI", "AL",  "BL",  "CL",
                                      "AH",  "BH",  "CH"};

  auto n = 0;
  for (auto r : regs) {
    ImGui::NextColumn();
    ImGui::Text("%s", names[n].c_str());
    ImGui::NextColumn();
    ImGui::Text("%s", fmt::format("{}", r).c_str());
    ImGui::NextColumn();
    if (r.storeByte) {
      ImGui::Text("%s", fmt::format("{:02X}", static_cast<unsigned int>(
                                                  core->readRegByte(r)))
                            .c_str());
    } else {
      ImGui::Text(
          "%s",
          fmt::format("{:0{}X}", core->readRegInt(r), INT_SIZE * 2).c_str());
    }
    n++;
  }
  ImGui::Separator();
  ImGui::Columns(1);

  ImGui::Text("\nFlags:");
  std::array<std::byte, 2> flags = {ZF, INTF};
  std::array<std::string, 2> flag_names = {"ZF", "INTF"};
  n = 0;
  for (auto f : flags) {
    ImGui::Text(
        "%s", fmt::format("{}: {}", flag_names[n], core->checkFlag(f)).c_str());
    n++;
  }

  ImGui::End();
}

void App::serve() {
  bool done = false;
  auto &io = ImGui::GetIO();

  auto lt = ticks;
  while (!done) {
    SDL_Event event;
    if (SDL_WaitEventTimeout(&event, 50)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        done = true;
    } else {
      // if (!spEditor->GetDisplay()->RefreshRequired()) {
      //   continue;
      // }
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    mem_edit.DrawWindow("Code", core->code.get(), true,
                        core->pointer - core->code->offset,
                        core->next_spec_type);
    stack_edit.DrawWindow("Stack", core->stack.get(), false,
                          core->pointer - core->stack->offset,
                          core->next_spec_type);
    d_table_edit.DrawWindow("Devices", core->d_table.get(), false,
                            core->pointer - core->d_table->offset,
                            core->next_spec_type);
    for (auto device : core->devices) {
      MemoryEditor editor;
      auto exec = device->deviceId == std::byte{0x10};
      editor.DrawWindow(fmt::format("Device 0x{:02X}",
                                    static_cast<unsigned int>(device->deviceId))
                            .c_str(),
                        device->memory.get(), exec, core->pointer - device->memory->offset,
                        core->next_spec_type);
    }
    drawMainWindow();
    drawRegWindow();
    drawControlWindow();
    drawCodeWindow();
    if (video != nullptr) {
      // drawPicWindow(video);
    }
    lt = ticks;
    logger.Draw("Log");

    ImGui::Begin("Code", nullptr, ImVec2(1024, 768));
    if (ImGui::Button("Compile")) {
      compile();
    }
    // spEditor->Display(Zep::toNVec2f(ImGui::GetCursorScreenPos()),
    //                   Zep::toNVec2f(ImGui::GetContentRegionAvail()));

    ImGui::End();

    showStatusbar();

    ImVec4 clear_color = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x,
               (int)ImGui::GetIO().DisplaySize.y);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);

    // uint32_t mod = 0;

    // if (io.KeyCtrl)
    // {
    //     mod |= ModifierKey::Ctrl;
    // }
    // if (io.KeyShift)
    // {
    //     mod |= ModifierKey::Shift;
    // }

    if (io.KeyCtrl and
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow))) {
      step();
    }
    if (io.KeyCtrl and
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) {
      run();
    }
  }

  wait_for_key = false;
  st.join();
  clearStatusMessage();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_GL_DeleteContext(glcontext);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void App::compile() {
  auto filename = fs::absolute(fs::path(input_file)).string();
  loadFileText(filename);
  dis_code = analyzer.parseFile(filename);
  core->compile(dis_code);
  auto vm_filename =
      fmt::format("{}/{}.vvm", path, fs::path(filename).stem().string());
  core->saveBytes(vm_filename);
}

void App::drawPicWindow(std::shared_ptr<VideoDevice> video) {
  ImGui::Begin(
      fmt::format("Output 0x{:02X}", static_cast<unsigned int>(video->deviceId))
          .c_str());
  auto sq_size = 16;

  GLuint g_FontTexture = 0;
  int width = video->width * sq_size;
  int height = video->height * sq_size;

  std::vector<unsigned int> pixels;
  pixels.assign(width * height, 0x0);

  auto n = 0;
  // ABGR
  std::map<std::byte, unsigned int> colors = {
      {std::byte{0x0}, 0x00000000}, {std::byte{0x1}, 0xff0000aa},
      {std::byte{0x2}, 0xff00aa00}, {std::byte{0x3}, 0xff00aaaa},
      {std::byte{0x4}, 0xffaa0000}, {std::byte{0x5}, 0xffaa00aa},
      {std::byte{0x6}, 0xffaaaa00}, {std::byte{0x7}, 0xffffffff},
  };
  for (auto col : video->memory->data) {
    if (col == std::byte{0x0}) {
      n++;
      continue;
    }
    for (auto x = 0; x < sq_size; x++) {
      for (auto y = 0; y < sq_size; y++) {
        // pixels[n*video->width + x + y*width + (n/sq_size)*height*15] =
        // colors[col];
        pixels[n * sq_size + x + y * width +
               (n / video->width) * sq_size * sq_size * video->width] =
            colors[col];
      }
    }
    n++;
  }
  n = 0;

  auto out_pixels = reinterpret_cast<char *>(&pixels[0]);

  // Upload texture to graphics system
  GLint last_texture;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  glGenTextures(1, &g_FontTexture);
  glBindTexture(GL_TEXTURE_2D, g_FontTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, out_pixels);

  // Store our identifier
  glBindTexture(GL_TEXTURE_2D, last_texture);
  auto my_tex_id = (void *)(intptr_t)g_FontTexture;
  ImGui::Image(my_tex_id, ImVec2(width, height), ImVec2(0, 0), ImVec2(1, 1),
               ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));

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
    if (!statusMsg.empty()) {
      ImGui::Text("%s", statusMsg.c_str());
    }
    ImGui::End();
  }
  ImGui::PopStyleColor();
  ImGui::PopStyleVar();
  ImGui::PopStyleVar();
}
