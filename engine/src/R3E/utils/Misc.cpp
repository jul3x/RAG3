//
// Created by jul3x on 22.03.21.
//

#include <SFML/Window/Keyboard.hpp>

#include <R3E/utils/Misc.h>


namespace r3e::utils {
    uint64_t timeSinceEpochMillisec()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
    }

    std::string capitalFirst(const std::string& word)
    {
        if (word.empty())
            return word;
        std::string return_word = word;
        return_word[0] = std::toupper(return_word[0]);
        return return_word;
    }

    std::string toLower(const std::string& word)
    {
        auto out = word;
        std::transform(out.begin(), out.end(), out.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return out;
    }

    std::string toUpper(const std::string& word)
    {
        auto out = word;
        std::transform(out.begin(), out.end(), out.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        return out;
    }

    bool startsWith(const std::string& what, const std::string& prefix)
    {
        return what.rfind(prefix, 0) == 0;
    }

    bool endsWith(const std::string& what, const std::string& suffix)
    {
        auto what_size = what.size();
        auto suffix_size = suffix.size();
        return what_size >= suffix_size && 0 == what.compare(what_size - suffix_size, suffix_size, suffix);
    }

    std::string humanize(const std::string& word)
    {
        auto out = capitalFirst(word);
        std::replace(out.begin(), out.end(), '_', ' ');

        for (size_t i = 0; i < out.size() - 1; ++i)
        {
            if (out[i] == ' ')
                out[i + 1] = std::toupper(out[i + 1]);
        }

        return out;
    }

    sf::Keyboard::Key stringToKey(const std::string& str)
    {
        if (str == "A")
            return sf::Keyboard::A;
        if (str == "B")
            return sf::Keyboard::B;
        if (str == "C")
            return sf::Keyboard::C;
        if (str == "D")
            return sf::Keyboard::D;
        if (str == "E")
            return sf::Keyboard::E;
        if (str == "F")
            return sf::Keyboard::F;
        if (str == "G")
            return sf::Keyboard::G;
        if (str == "H")
            return sf::Keyboard::H;
        if (str == "I")
            return sf::Keyboard::I;
        if (str == "J")
            return sf::Keyboard::J;
        if (str == "K")
            return sf::Keyboard::K;
        if (str == "L")
            return sf::Keyboard::L;
        if (str == "M")
            return sf::Keyboard::M;
        if (str == "N")
            return sf::Keyboard::N;
        if (str == "O")
            return sf::Keyboard::O;
        if (str == "P")
            return sf::Keyboard::P;
        if (str == "Q")
            return sf::Keyboard::Q;
        if (str == "R")
            return sf::Keyboard::R;
        if (str == "S")
            return sf::Keyboard::S;
        if (str == "T")
            return sf::Keyboard::T;
        if (str == "U")
            return sf::Keyboard::U;
        if (str == "V")
            return sf::Keyboard::V;
        if (str == "W")
            return sf::Keyboard::W;
        if (str == "X")
            return sf::Keyboard::X;
        if (str == "Y")
            return sf::Keyboard::Y;
        if (str == "Z")
            return sf::Keyboard::Z;
        if (str == "Num0")
            return sf::Keyboard::Num0;
        if (str == "Num1")
            return sf::Keyboard::Num1;
        if (str == "Num2")
            return sf::Keyboard::Num2;
        if (str == "Num3")
            return sf::Keyboard::Num3;
        if (str == "Num4")
            return sf::Keyboard::Num4;
        if (str == "Num5")
            return sf::Keyboard::Num5;
        if (str == "Num6")
            return sf::Keyboard::Num6;
        if (str == "Num7")
            return sf::Keyboard::Num7;
        if (str == "Num8")
            return sf::Keyboard::Num8;
        if (str == "Num9")
            return sf::Keyboard::Num9;
        if (str == "Escape")
            return sf::Keyboard::Escape;
        if (str == "LControl")
            return sf::Keyboard::LControl;
        if (str == "LShift")
            return sf::Keyboard::LShift;
        if (str == "LAlt")
            return sf::Keyboard::LAlt;
        if (str == "LSystem")
            return sf::Keyboard::LSystem;
        if (str == "RControl")
            return sf::Keyboard::RControl;
        if (str == "RShift")
            return sf::Keyboard::RShift;
        if (str == "RAlt")
            return sf::Keyboard::RAlt;
        if (str == "RSystem")
            return sf::Keyboard::RSystem;
        if (str == "Menu")
            return sf::Keyboard::Menu;
        if (str == "LBracket")
            return sf::Keyboard::LBracket;
        if (str == "RBracket")
            return sf::Keyboard::RBracket;
        if (str == "Semicolon")
            return sf::Keyboard::Semicolon;
        if (str == "Comma")
            return sf::Keyboard::Comma;
        if (str == "Period")
            return sf::Keyboard::Period;
        if (str == "Quote")
            return sf::Keyboard::Quote;
        if (str == "Slash")
            return sf::Keyboard::Slash;
        if (str == "Backslash")
            return sf::Keyboard::Backslash;
        if (str == "Tilde")
            return sf::Keyboard::Tilde;
        if (str == "Equal")
            return sf::Keyboard::Equal;
        if (str == "Hyphen")
            return sf::Keyboard::Hyphen;
        if (str == "Space")
            return sf::Keyboard::Space;
        if (str == "Enter")
            return sf::Keyboard::Enter;
        if (str == "Backspace")
            return sf::Keyboard::Backspace;
        if (str == "Tab")
            return sf::Keyboard::Tab;
        if (str == "PageUp")
            return sf::Keyboard::PageUp;
        if (str == "PageDown")
            return sf::Keyboard::PageDown;
        if (str == "End")
            return sf::Keyboard::End;
        if (str == "Home")
            return sf::Keyboard::Home;
        if (str == "Insert")
            return sf::Keyboard::Insert;
        if (str == "Delete")
            return sf::Keyboard::Delete;
        if (str == "Add")
            return sf::Keyboard::Add;
        if (str == "Subtract")
            return sf::Keyboard::Subtract;
        if (str == "Multiply")
            return sf::Keyboard::Multiply;
        if (str == "Divide")
            return sf::Keyboard::Divide;
        if (str == "Left")
            return sf::Keyboard::Left;
        if (str == "Right")
            return sf::Keyboard::Right;
        if (str == "Up")
            return sf::Keyboard::Up;
        if (str == "Down")
            return sf::Keyboard::Down;
        if (str == "Numpad0")
            return sf::Keyboard::Numpad0;
        if (str == "Numpad1")
            return sf::Keyboard::Numpad1;
        if (str == "Numpad2")
            return sf::Keyboard::Numpad2;
        if (str == "Numpad3")
            return sf::Keyboard::Numpad3;
        if (str == "Numpad4")
            return sf::Keyboard::Numpad4;
        if (str == "Numpad5")
            return sf::Keyboard::Numpad5;
        if (str == "Numpad6")
            return sf::Keyboard::Numpad6;
        if (str == "Numpad7")
            return sf::Keyboard::Numpad7;
        if (str == "Numpad8")
            return sf::Keyboard::Numpad8;
        if (str == "Numpad9")
            return sf::Keyboard::Numpad9;
        if (str == "F1")
            return sf::Keyboard::F1;
        if (str == "F2")
            return sf::Keyboard::F2;
        if (str == "F3")
            return sf::Keyboard::F3;
        if (str == "F4")
            return sf::Keyboard::F4;
        if (str == "F5")
            return sf::Keyboard::F5;
        if (str == "F6")
            return sf::Keyboard::F6;
        if (str == "F7")
            return sf::Keyboard::F7;
        if (str == "F8")
            return sf::Keyboard::F8;
        if (str == "F9")
            return sf::Keyboard::F9;
        if (str == "F10")
            return sf::Keyboard::F10;
        if (str == "F11")
            return sf::Keyboard::F11;
        if (str == "F12")
            return sf::Keyboard::F12;
        if (str == "F13")
            return sf::Keyboard::F13;
        if (str == "F14")
            return sf::Keyboard::F14;
        if (str == "F15")
            return sf::Keyboard::F15;
        if (str == "Pause")
            return sf::Keyboard::Pause;

        return sf::Keyboard::Unknown;
    }

    std::string keyToString(sf::Keyboard::Key key)
    {
        switch (key)
        {
            case sf::Keyboard::A:
                return "A";
            case sf::Keyboard::B:
                return "B";
            case sf::Keyboard::C:
                return "C";
            case sf::Keyboard::D:
                return "D";
            case sf::Keyboard::E:
                return "E";
            case sf::Keyboard::F:
                return "F";
            case sf::Keyboard::G:
                return "G";
            case sf::Keyboard::H:
                return "H";
            case sf::Keyboard::I:
                return "I";
            case sf::Keyboard::J:
                return "J";
            case sf::Keyboard::K:
                return "K";
            case sf::Keyboard::L:
                return "L";
            case sf::Keyboard::M:
                return "M";
            case sf::Keyboard::N:
                return "N";
            case sf::Keyboard::O:
                return "O";
            case sf::Keyboard::P:
                return "P";
            case sf::Keyboard::Q:
                return "Q";
            case sf::Keyboard::R:
                return "R";
            case sf::Keyboard::S:
                return "S";
            case sf::Keyboard::T:
                return "T";
            case sf::Keyboard::U:
                return "U";
            case sf::Keyboard::V:
                return "V";
            case sf::Keyboard::W:
                return "W";
            case sf::Keyboard::X:
                return "X";
            case sf::Keyboard::Y:
                return "Y";
            case sf::Keyboard::Z:
                return "Z";
            case sf::Keyboard::Num0:
                return "Num0";
            case sf::Keyboard::Num1:
                return "Num1";
            case sf::Keyboard::Num2:
                return "Num2";
            case sf::Keyboard::Num3:
                return "Num3";
            case sf::Keyboard::Num4:
                return "Num4";
            case sf::Keyboard::Num5:
                return "Num5";
            case sf::Keyboard::Num6:
                return "Num6";
            case sf::Keyboard::Num7:
                return "Num7";
            case sf::Keyboard::Num8:
                return "Num8";
            case sf::Keyboard::Num9:
                return "Num9";
            case sf::Keyboard::Escape:
                return "Escape";
            case sf::Keyboard::LControl:
                return "LControl";
            case sf::Keyboard::LShift:
                return "LShift";
            case sf::Keyboard::LAlt:
                return "LAlt";
            case sf::Keyboard::LSystem:
                return "LSystem";
            case sf::Keyboard::RControl:
                return "RControl";
            case sf::Keyboard::RShift:
                return "RShift";
            case sf::Keyboard::RAlt:
                return "RAlt";
            case sf::Keyboard::RSystem:
                return "RSystem";
            case sf::Keyboard::Menu:
                return "Menu";
            case sf::Keyboard::LBracket:
                return "LBracket";
            case sf::Keyboard::RBracket:
                return "RBracket";
            case sf::Keyboard::Semicolon:
                return "Semicolon";
            case sf::Keyboard::Comma:
                return "Comma";
            case sf::Keyboard::Period:
                return "Period";
            case sf::Keyboard::Quote:
                return "Quote";
            case sf::Keyboard::Slash:
                return "Slash";
            case sf::Keyboard::Backslash:
                return "Backslash";
            case sf::Keyboard::Tilde:
                return "Tilde";
            case sf::Keyboard::Equal:
                return "Equal";
            case sf::Keyboard::Hyphen:
                return "Hyphen";
            case sf::Keyboard::Space:
                return "Space";
            case sf::Keyboard::Enter:
                return "Enter";
            case sf::Keyboard::Backspace:
                return "Backspace";
            case sf::Keyboard::Tab:
                return "Tab";
            case sf::Keyboard::PageUp:
                return "PageUp";
            case sf::Keyboard::PageDown:
                return "PageDown";
            case sf::Keyboard::End:
                return "End";
            case sf::Keyboard::Home:
                return "Home";
            case sf::Keyboard::Insert:
                return "Insert";
            case sf::Keyboard::Delete:
                return "Delete";
            case sf::Keyboard::Add:
                return "Add";
            case sf::Keyboard::Subtract:
                return "Subtract";
            case sf::Keyboard::Multiply:
                return "Multiply";
            case sf::Keyboard::Divide:
                return "Divide";
            case sf::Keyboard::Left:
                return "Left";
            case sf::Keyboard::Right:
                return "Right";
            case sf::Keyboard::Up:
                return "Up";
            case sf::Keyboard::Down:
                return "Down";
            case sf::Keyboard::Numpad0:
                return "Numpad0";
            case sf::Keyboard::Numpad1:
                return "Numpad1";
            case sf::Keyboard::Numpad2:
                return "Numpad2";
            case sf::Keyboard::Numpad3:
                return "Numpad3";
            case sf::Keyboard::Numpad4:
                return "Numpad4";
            case sf::Keyboard::Numpad5:
                return "Numpad5";
            case sf::Keyboard::Numpad6:
                return "Numpad6";
            case sf::Keyboard::Numpad7:
                return "Numpad7";
            case sf::Keyboard::Numpad8:
                return "Numpad8";
            case sf::Keyboard::Numpad9:
                return "Numpad9";
            case sf::Keyboard::F1:
                return "F1";
            case sf::Keyboard::F2:
                return "F2";
            case sf::Keyboard::F3:
                return "F3";
            case sf::Keyboard::F4:
                return "F4";
            case sf::Keyboard::F5:
                return "F5";
            case sf::Keyboard::F6:
                return "F6";
            case sf::Keyboard::F7:
                return "F7";
            case sf::Keyboard::F8:
                return "F8";
            case sf::Keyboard::F9:
                return "F9";
            case sf::Keyboard::F10:
                return "F10";
            case sf::Keyboard::F11:
                return "F11";
            case sf::Keyboard::F12:
                return "F12";
            case sf::Keyboard::F13:
                return "F13";
            case sf::Keyboard::F14:
                return "F14";
            case sf::Keyboard::F15:
                return "F15";
            case sf::Keyboard::Pause:
                return "Pause";
            default:
                return "ERROR";
        }
    }

    sf::IpAddress getSafeIP(const sf::IpAddress& ip, const sf::IpAddress& local)
    {
        if (ip == local)
            return sf::IpAddress::LocalHost;
        return ip;
    }
} // namespace r3e::utils
