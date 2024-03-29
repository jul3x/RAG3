//
// Created by jul3x on 10.03.20.
//

#ifndef RAG3_EDITOR_INCLUDE_EDITOR_USERINTERFACE_H
#define RAG3_EDITOR_INCLUDE_EDITOR_USERINTERFACE_H


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
#include <editor/WeaponWindow.h>
#include <editor/CharacterObjectWindow.h>
#include <editor/ObstacleObjectWindow.h>
#include <editor/ParametersWindow.h>
#include <editor/ControlsWindow.h>


using namespace r3e;

namespace editor {
    class UserInterface : public AbstractUserInterface {

    public:
        explicit UserInterface();

        UserInterface(const UserInterface&) = delete;

        UserInterface& operator=(const UserInterface&) = delete;

        void initialize(graphics::Graphics& graphics) override;
        void update(graphics::Graphics& graphics, float time_elapsed) override;
        void draw(graphics::Graphics& graphics) override;

        void registerCamera(Camera* camera);

        void spawnInfo(const std::string& msg);
        void spawnError(const std::string& msg);

        void resetMapList();

        void openConfigWindow(const std::string& category, const std::string& id);
        void openUniqueObjectWindow(const std::string& category, const std::string& name, int id);
        void openSpecialObjectWindow(const std::string& category, Special* obj);
        void openCharacterObjectWindow(const std::string& category, Character* obj);
        void openWeaponWindow(const std::string& category, PlacedWeapon* obj);
        void openObstacleWindow(const std::string& category, Obstacle* obj);
        void openParametersWindow();
        void openControlsWindow();

        void setZIndex(int value);
        int getZIndex() const;

    private:
        static constexpr size_t MIN_VECTORS_TO_MOVE_ = 5;

        inline void generateMenuBar(sf::RenderWindow& window);
        inline void handleKeys();
        inline void handleMouse(sf::RenderWindow& graphics_window, float time_elapsed);
        inline void
        handleCameraCenter(sf::RenderWindow& graphics_window, const sf::Vector2f& mouse_world_pos, float time_elapsed);
        inline void
        handleCrosshair(sf::RenderWindow& graphics_window, const sf::Vector2f& mouse_world_pos, float time_elapsed);

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
        ObstacleObjectWindow obstacle_object_window_;
        WeaponWindow weapon_window_;
        CharacterObjectWindow character_object_window_;
        ParametersWindow parameters_window_;
        ControlsWindow controls_window_;

        sf::Text information_;
        float information_a_;

        bool mouse_on_widget_;
        sf::Vector2f previous_mouse_world_pos_;
        std::deque<sf::Vector2f> mouse_camera_center_vectors_;
        AbstractDrawableObject* marked_item_;

        int max_z_index_;

    };
} // namespace editor

#endif //RAG3_EDITOR_INCLUDE_EDITOR_USERINTERFACE_H
