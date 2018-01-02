#include "application.cpp"

std::string VERSION = "0.0.1";

int main()
{
  Application app(VERSION);
  app.serve();
}
