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
#include <R3E/graphics/Lighting.h>

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
        void close() override;

        void alertCollision(HoveringObject* h_obj, StaticObject* s_obj) override
        {
        }

        void alertCollision(HoveringObject* h_obj, DynamicObject* d_obj) override
        {
        }

        void alertCollision(DynamicObject* d_obj, StaticObject* s_obj) override
        {
        }

        void alertCollision(DynamicObject* d_obj_1, DynamicObject* d_obj_2) override
        {
        }

        void resetLighting(const sf::Vector2f& size);
        void setLighting(bool on);
        void setRandomizing(int n);
        void setCurrentItem(const std::string& category, const std::string& id);
        [[nodiscard]] const std::pair<std::string, std::string>& getCurrentItem() const;
        [[nodiscard]] AbstractDrawableObject* getMarkedItem();
        void markCurrentItem(const sf::Vector2f& pos);

        const sf::Vector2f& getMarkingStart() const;
        const sf::Vector2f& getMarkingStop() const;
        bool isGridMarked() const;
        void setMarkingStart(const sf::Vector2f& pos);
        void setMarkingStop(const sf::Vector2f& pos);
        void setGridMarked(bool marked);
        void removeMarkedItems();
        void setGridMarkedPositions(const sf::Vector2f& pos_offset);

        int readItemInfo(const sf::Vector2f& pos, bool read_uid = false);
        void placeItem(const sf::Vector2f& pos, float direction = 0.0f);
        void placeItemOnMarked();
        void removeItem(const sf::Vector2f& pos);

        void clearMap();
        void loadMap(const std::string& name);
        void saveMap(const std::string& name);
        [[nodiscard]] const std::string& getCurrentMapName() const;
        [[nodiscard]] sf::Vector2f getMapCoordinates(const sf::Vector2f& pos) const;
        [[nodiscard]] Map& getMap();
        [[nodiscard]] UserInterface& getUI();

        void saveConfig(const std::string& category, const std::string& id, const std::string& content);

        void spawnError(const std::string& err);
    private:
        Editor();

        std::unique_ptr<Engine> engine_;
        std::unique_ptr<UserInterface> ui_;
        std::unique_ptr<Camera> camera_;
        std::unique_ptr<graphics::Lighting> lighting_;
        Grid grid_;

        std::unique_ptr<Map> map_;

        std::string current_map_name_;

        int randomizing_value_;
        std::pair<std::string, std::string> current_item_;
        AbstractDrawableObject* marked_item_;

        sf::Vector2f grid_mark_start_, grid_mark_stop_;
        std::unordered_set<AbstractDrawableObject*> grid_marked_items_;
        bool is_grid_marked_;

        bool is_lighting_on_;


    };
} // namespace editor


#endif //RAG3_EDITOR_INCLUDE_EDITOR_H
