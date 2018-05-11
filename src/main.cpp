#include "args.hpp"
#include "vvm/application.hpp"

std::string VERSION = "0.1.0";

int main(int argc, char **argv) {
  args::ArgumentParser parser(
      "Vortex VM. Toy virtual machine for toy assembly.", "");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::Positional<std::string> input_file(parser, "input_file",
                                           "Input code file");
  try {
    parser.ParseCLI(argc, argv);
    App app(VERSION, args::get(input_file));
    app.serve();
  } catch (args::Completion e) {
    std::cout << e.what();
    return 0;
  } catch (args::Help) {
    std::cout << parser;
    return 0;
  } catch (args::ParseError e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return 1;
  }
  return 0;
}
