#include "vvm/App.h"

std::string VERSION = "0.0.1";

int main()
{
  App app(VERSION);
  app.serve();
}

