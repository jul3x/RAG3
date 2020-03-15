//
// Created by jul3x on 10.03.20.
//

#ifndef RAG3_EDITOR_H
#define RAG3_EDITOR_H

#include <memory>

#include <R3E/system/AbstractGame.h>
#include <R3E/system/Engine.h>
#include <R3E/ai/AgentsManager.h>
#include <R3E/audio/MusicManager.h>

#include <editor/Camera.h>
#include <editor/Map.h>
#include <editor/UserInterface.h>
#include <editor/Grid.h>


using namespace r3e;

namespace editor {
    class Editor : public AbstractGame {

    public:
        Editor(const Editor&) = delete;

        Editor& operator=(const Editor&) = delete;

        static Editor& get()
        {
            static Editor instance;
            return instance;
        }

        void initialize() override;

        void update(float time_elapsed) override;

        void draw(graphics::Graphics& graphics) override;

        void alertCollision(HoveringObject* h_obj, StaticObject* s_obj) override {}

        void alertCollision(HoveringObject* h_obj, DynamicObject* d_obj) override {}

        void alertCollision(DynamicObject* d_obj, StaticObject* s_obj) override {}

        void alertCollision(DynamicObject* d_obj_1, DynamicObject* d_obj_2) override {}

        void setCurrentItem(const std::string& category, const std::string& id);

        const std::pair<std::string, std::string>& getCurrentItem() const;

        void start();

    private:
        Editor();

        std::unique_ptr<Engine> engine_;
        std::unique_ptr<UserInterface> ui_;
        std::unique_ptr<Camera> camera_;

        std::unique_ptr<Map> map_;

        std::pair<std::string, std::string> current_item_;

        Grid grid_;

    };
} // namespace editor


#endif // RAG3_EDITOR_H
