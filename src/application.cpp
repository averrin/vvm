#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class Application {
	std::string VERSION;
	sf::RenderWindow *window;

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

		ImGui::StyleColorsDark();
		while (window->isOpen()) {
			sf::Event event;
			while (window->pollEvent(event)) {
				processEvent(event);
			}

			window->clear(sf::Color(100,100,100));
			ImGui::SFML::Update(*window, deltaClock.restart());
			drawMainWindow();


			ImGui::SFML::Render(*window);
			window->display();

		}
		ImGui::SFML::Shutdown();
	}
};