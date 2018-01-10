#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <sstream>
#include <iterator>
#include <iostream>
#include "format.h"
#include "vvm/application.hpp"

void App::resetSeqWait(bool success)
{
	wait_for_key = false;
  if (!success) {
    statusMsg = fmt::format("{} is undefined", keySeq.substr(0, keySeq.size()-4));
  }
	keySeq = "";
	pressed_seq.clear();
}

void App::processKey(const sf::Event event)
{
	switch (event.key.code) {
	case sf::Keyboard::RControl:
	case sf::Keyboard::LControl:
	case sf::Keyboard::LAlt:
	case sf::Keyboard::RAlt:
	case sf::Keyboard::RShift:
	case sf::Keyboard::LShift:
		break;
	default:
    lastFiredAction = nullptr;
    statusMsg = "";
		const auto name = getKeyName(event.key.code, event.key.alt, event.key.control, event.key.shift);
		pressed_seq.push_back(name);

    std::ostringstream oss;
    std::copy(pressed_seq.begin(), pressed_seq.end(), std::ostream_iterator<std::string>(oss, " -> "));

    keySeq = oss.str();

		const auto seq = std::find_if(actions.begin(), actions.end(), [&](sequence s) {
			if (s.keys.size() < pressed_seq.size()) { return false; }
			auto res = true;
			auto n = 0;
			for (auto k : pressed_seq)
			{
				std::transform(k.begin(), k.end(), k.begin(), ::tolower);
				auto sk = s.keys[n];
				std::transform(sk.begin(), sk.end(), sk.begin(), ::tolower);
				res = k == sk;
				if (!res) break;
				n++;
			}
			return res;
		});
		if (seq != actions.end())
		{
			if ((*seq).keys.size() > pressed_seq.size())
			{
				// fmt::print("waiting for other key\n");
			}
			else if ((*seq).keys.size() == pressed_seq.size())
			{
				if((*seq).func != nullptr)
				{
          statusMsg = "";
          lastFiredAction = &(*seq);
					(*seq).func();
				}
				resetSeqWait(true);
				return;
			}
			wait_for_key = false;
			if (st.joinable()) st.join();
			wait_for_key = true;
			st = std::thread([&](int delay) {
				auto n = 0;
				while (n < delay && wait_for_key)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
					n++;
				}
				if (wait_for_key)
				{
					resetSeqWait(false);
				}
			}, leader_key_delay);
		}
		else
		{
			statusMsg = fmt::format("{} is undefined", keySeq.substr(0, keySeq.size()-4));
			resetSeqWait(false);
		}
		return;

	}
}

std::string App::getKeyName(const sf::Keyboard::Key key, bool alt, bool control, bool shift) {
	std::string name;
	std::string mods;
	if (alt) { mods = "meta+"; }
	if (control) { mods = fmt::format("{}control+", mods); }
	if (shift) { mods = fmt::format("{}shift+", mods); }
	switch (key) {
	default:
	case sf::Keyboard::Unknown:
		name = "Unknown"; break;
	case sf::Keyboard::A:
		name = "A"; break;
	case sf::Keyboard::B:
		name = "B"; break;
	case sf::Keyboard::C:
		name = "C"; break;
	case sf::Keyboard::D:
		name = "D"; break;
	case sf::Keyboard::E:
		name = "E"; break;
	case sf::Keyboard::F:
		name = "F"; break;
	case sf::Keyboard::G:
		name = "G"; break;
	case sf::Keyboard::H:
		name = "H"; break;
	case sf::Keyboard::I:
		name = "I"; break;
	case sf::Keyboard::J:
		name = "J"; break;
	case sf::Keyboard::K:
		name = "K"; break;
	case sf::Keyboard::L:
		name = "L"; break;
	case sf::Keyboard::M:
		name = "M"; break;
	case sf::Keyboard::N:
		name = "N"; break;
	case sf::Keyboard::O:
		name = "O"; break;
	case sf::Keyboard::P:
		name = "P"; break;
	case sf::Keyboard::Q:
		name = "Q"; break;
	case sf::Keyboard::R:
		name = "R"; break;
	case sf::Keyboard::S:
		name = "S"; break;
	case sf::Keyboard::T:
		name = "T"; break;
	case sf::Keyboard::U:
		name = "U"; break;
	case sf::Keyboard::V:
		name = "V"; break;
	case sf::Keyboard::W:
		name = "W"; break;
	case sf::Keyboard::X:
		name = "X"; break;
	case sf::Keyboard::Y:
		name = "Y"; break;
	case sf::Keyboard::Z:
		name = "Z"; break;
	case sf::Keyboard::Num0:
		name = "Num0"; break;
	case sf::Keyboard::Num1:
		name = "Num1"; break;
	case sf::Keyboard::Num2:
		name = "Num2"; break;
	case sf::Keyboard::Num3:
		name = "Num3"; break;
	case sf::Keyboard::Num4:
		name = "Num4"; break;
	case sf::Keyboard::Num5:
		name = "Num5"; break;
	case sf::Keyboard::Num6:
		name = "Num6"; break;
	case sf::Keyboard::Num7:
		name = "Num7"; break;
	case sf::Keyboard::Num8:
		name = "Num8"; break;
	case sf::Keyboard::Num9:
		name = "Num9"; break;
	case sf::Keyboard::Escape:
		name = "Escape"; break;
	case sf::Keyboard::LSystem:
		name = "LSystem"; break;
	case sf::Keyboard::RSystem:
		name = "RSystem"; break;
	case sf::Keyboard::Menu:
		name = "Menu"; break;
	case sf::Keyboard::LBracket:
		name = "LBracket"; break;
	case sf::Keyboard::RBracket:
		name = "RBracket"; break;
	case sf::Keyboard::SemiColon:
		name = "SemiColon"; break;
	case sf::Keyboard::Comma:
		name = "Comma"; break;
	case sf::Keyboard::Period:
		name = "Period"; break;
	case sf::Keyboard::Quote:
		name = "Quote"; break;
	case sf::Keyboard::Slash:
		name = "Slash"; break;
	case sf::Keyboard::BackSlash:
		name = "BackSlash"; break;
	case sf::Keyboard::Tilde:
		name = "Tilde"; break;
	case sf::Keyboard::Equal:
		name = "Equal"; break;
	case sf::Keyboard::Dash:
		name = "Dash"; break;
	case sf::Keyboard::Space:
		name = "Space"; break;
	case sf::Keyboard::Return:
		name = "Return"; break;
	case sf::Keyboard::BackSpace:
		name = "BackSpace"; break;
	case sf::Keyboard::Tab:
		name = "Tab"; break;
	case sf::Keyboard::PageUp:
		name = "PageUp"; break;
	case sf::Keyboard::PageDown:
		name = "PageDown"; break;
	case sf::Keyboard::End:
		name = "End"; break;
	case sf::Keyboard::Home:
		name = "Home"; break;
	case sf::Keyboard::Insert:
		name = "Insert"; break;
	case sf::Keyboard::Delete:
		name = "Delete"; break;
	case sf::Keyboard::Add:
		name = "Add"; break;
	case sf::Keyboard::Subtract:
		name = "Subtract"; break;
	case sf::Keyboard::Multiply:
		name = "Multiply"; break;
	case sf::Keyboard::Divide:
		name = "Divide"; break;
	case sf::Keyboard::Left:
		name = "Left"; break;
	case sf::Keyboard::Right:
		name = "Right"; break;
	case sf::Keyboard::Up:
		name = "Up"; break;
	case sf::Keyboard::Down:
		name = "Down"; break;
	case sf::Keyboard::Numpad0:
		name = "Numpad0"; break;
	case sf::Keyboard::Numpad1:
		name = "Numpad1"; break;
	case sf::Keyboard::Numpad2:
		name = "Numpad2"; break;
	case sf::Keyboard::Numpad3:
		name = "Numpad3"; break;
	case sf::Keyboard::Numpad4:
		name = "Numpad4"; break;
	case sf::Keyboard::Numpad5:
		name = "Numpad5"; break;
	case sf::Keyboard::Numpad6:
		name = "Numpad6"; break;
	case sf::Keyboard::Numpad7:
		name = "Numpad7"; break;
	case sf::Keyboard::Numpad8:
		name = "Numpad8"; break;
	case sf::Keyboard::Numpad9:
		name = "Numpad9"; break;
	case sf::Keyboard::F1:
		name = "F1"; break;
	case sf::Keyboard::F2:
		name = "F2"; break;
	case sf::Keyboard::F3:
		name = "F3"; break;
	case sf::Keyboard::F4:
		name = "F4"; break;
	case sf::Keyboard::F5:
		name = "F5"; break;
	case sf::Keyboard::F6:
		name = "F6"; break;
	case sf::Keyboard::F7:
		name = "F7"; break;
	case sf::Keyboard::F8:
		name = "F8"; break;
	case sf::Keyboard::F9:
		name = "F9"; break;
	case sf::Keyboard::F10:
		name = "F10"; break;
	case sf::Keyboard::F11:
		name = "F11"; break;
	case sf::Keyboard::F12:
		name = "F12"; break;
	case sf::Keyboard::F13:
		name = "F13"; break;
	case sf::Keyboard::F14:
		name = "F14"; break;
	case sf::Keyboard::F15:
		name = "F15"; break;
	case sf::Keyboard::Pause:
		name = "Pause"; break;
	}
	return fmt::format("{}{}",mods,name);
}
