//
// Created by jul3x on 10.03.20.
//

#ifndef RAG3_EDITOR_USERINTERFACE_H
#define RAG3_EDITOR_USERINTERFACE_H


#include <TGUI/TGUI.hpp>

#include <R3E/system/Config.h>
#include <R3E/system/AbstractUserInterface.h>

#include <editor/Crosshair.h>
#include <editor/Camera.h>
#include <editor/ListWindow.h>
#include <editor/MenuWindow.h>
#include <editor/SaveWindow.h>
#include <editor/LoadWindow.h>
#include <editor/ConfigWindow.h>
#include <editor/UniqueObjectWindow.h>
#include <editor/SpecialObjectWindow.h>


using namespace r3e;

namespace editor {
    class UserInterface : public AbstractUserInterface {

    public:
        explicit UserInterface();

        UserInterface(const UserInterface&) = delete;

        UserInterface& operator=(const UserInterface&) = delete;

        void initialize(graphics::Graphics& graphics) override;

        void registerCamera(Camera* camera);

        void handleEvents(graphics::Graphics& graphics, float time_elapsed) override;

        void draw(graphics::Graphics& graphics) override;

        void spawnInfo(const std::string& msg);

        void spawnError(const std::string& msg);

        void resetMapList();

        void openConfigWindow(const std::string& category, const std::string& id);

        void openUniqueObjectWindow(const std::string& category, const std::string& name, int id);

        void openSpecialObjectWindow(const std::string& category, Functional* obj);

    private:
        inline void generateMenuBar(sf::RenderWindow& window);

        inline void handleKeys();

        inline void handleMouse(sf::RenderWindow& graphics_window);

        inline void handleCameraCenter(sf::RenderWindow& graphics_window, const sf::Vector2f& mouse_world_pos);

        inline void handleCrosshair(sf::RenderWindow& graphics_window, const sf::Vector2f& mouse_world_pos);

        static constexpr float LOGO_OFF_X_ = 240.0f;
        static constexpr float LOGO_OFF_Y_ = 120.0f;
        static constexpr float LOGO_SIZE_X_ = 512.0f;
        static constexpr float LOGO_SIZE_Y_ = 288.0f;

        Camera* camera_;

        Crosshair crosshair_;

        AbstractDrawableObject logo_;

        tgui::Gui gui_;
        tgui::Theme gui_theme_;

        ListWindow tiles_window_, objects_window_;
        MenuWindow menu_window_;

        SaveWindow save_window_;
        LoadWindow load_window_;
        ConfigWindow config_window_;
        UniqueObjectWindow unique_object_window_;
        SpecialObjectWindow special_object_window_;

        sf::Text information_;

        sf::Vector2f previous_mouse_world_pos_;

    };
} // namespace editor

#endif // RAG3_EDITOR_USERINTERFACE_H
