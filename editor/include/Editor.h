//
// Created by jul3x on 10.03.20.
//

#ifndef RAG3_EDITOR_INCLUDE_EDITOR_H
#define RAG3_EDITOR_INCLUDE_EDITOR_H

#include <memory>

#include <R3E/system/AbstractGame.h>
#include <R3E/system/Engine.h>
#include <R3E/ai/AgentsManager.h>
#include <R3E/audio/MusicManager.h>

#include <common/Map.h>

#include <editor/Camera.h>
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

        void start();
        void initialize() override;
        void update(float time_elapsed) override;
        void draw(graphics::Graphics& graphics) override;

        void alertCollision(HoveringObject* h_obj, StaticObject* s_obj) override {}
        void alertCollision(HoveringObject* h_obj, DynamicObject* d_obj) override {}
        void alertCollision(DynamicObject* d_obj, StaticObject* s_obj) override {}
        void alertCollision(DynamicObject* d_obj_1, DynamicObject* d_obj_2) override {}

        void setCurrentItem(const std::string& category, const std::string& id);
        [[nodiscard]] const std::pair<std::string, std::string>& getCurrentItem() const;
        [[nodiscard]] AbstractDrawableObject* getMarkedItem();
        void markCurrentItem(const sf::Vector2f& pos);

        int readItemInfo(const sf::Vector2f& pos, bool read_uid = false);
        void placeItem(const sf::Vector2f& pos, float direction = 0.0f);
        void removeItem(const sf::Vector2f& pos);

        void clearMap();
        void loadMap(const std::string& name);
        void saveMap(const std::string& name);
        [[nodiscard]] const std::string& getCurrentMapName() const;
        [[nodiscard]] sf::Vector2f getMapCoordinates(const sf::Vector2f& pos) const;

        void saveConfig(const std::string& category, const std::string& id, const std::string& content);

        void spawnError(const std::string& err);

    private:
        Editor();

        std::unique_ptr<Engine> engine_;
        std::unique_ptr<UserInterface> ui_;
        std::unique_ptr<Camera> camera_;
        Grid grid_;

        std::unique_ptr<Map> map_;

        std::string current_map_name_;

        std::pair<std::string, std::string> current_item_;
        AbstractDrawableObject* marked_item_;


    };
} // namespace editor


#endif //RAG3_EDITOR_INCLUDE_EDITOR_H
