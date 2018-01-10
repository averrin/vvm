#include "vvm/application.hpp"

std::string VERSION = "0.0.1";

int main()
{
  App app(VERSION);
  app.serve();
}

