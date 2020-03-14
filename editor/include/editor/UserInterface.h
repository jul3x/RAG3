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


using namespace r3e;

namespace editor {
    class UserInterface : public AbstractUserInterface {

    public:
        explicit UserInterface();

        UserInterface(const UserInterface&) = delete;

        UserInterface& operator=(const UserInterface&) = delete;

        void initialize(graphics::Graphics& graphics) override;

        void handleEvents(graphics::Graphics& graphics, float time_elapsed) override;

        void draw(graphics::Graphics& graphics) override;

    private:
        inline void handleScrolling(float delta);

        inline void handleKeys();

        inline void handleMouse(sf::RenderWindow& graphics_window);

        static constexpr float LOGO_OFF_X_ = 240.0f;
        static constexpr float LOGO_OFF_Y_ = 120.0f;
        static constexpr float LOGO_SIZE_X_ = 512.0f;
        static constexpr float LOGO_SIZE_Y_ = 288.0f;

        Crosshair crosshair_;

        AbstractDrawableObject logo_;

        tgui::Gui gui_;
        tgui::Theme gui_theme_;

        ListWindow tiles_window_;

    };
} // namespace editor

#endif // RAG3_EDITOR_USERINTERFACE_H
