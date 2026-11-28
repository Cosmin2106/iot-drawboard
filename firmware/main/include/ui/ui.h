#pragma once

#include <functional>
#include <map>
#include <tuple>
#include <vector>

#include "canvas.h"

struct UINode {
    int node_id = 0;
    Canvas* from = nullptr;
    Canvas* to = nullptr;
    std::function<void()> on_select{};
};

class UI {
    static struct TrackedHWState {
        bool trigger_left_pressed = false;
        bool trigger_right_pressed = false;
        int pot_left_perc = -1;
        int pot_right_perc = -1;
        int bat_perc = -1;

        bool operator==(const TrackedHWState& other) const {
            return trigger_left_pressed == other.trigger_left_pressed
                && trigger_right_pressed == other.trigger_right_pressed
                && pot_left_perc == other.pot_left_perc
                && pot_right_perc == other.pot_right_perc
                && bat_perc == other.bat_perc;
        }
    } current_hw_state_, old_hw_state_;

    inline static portMUX_TYPE* mux_ = nullptr;
    static std::map<Canvas*, std::vector<UINode>> ui_graph_;
    inline static Canvas* active_screen_ = nullptr;
    static bool change_screens_;

    static void setActiveScreen(Canvas& screen);

    static void select(int node_id);

    static void back();

    static void lazyDraw();

public:
    UI() = default;

    UI(const UI&) = delete;

    UI(UI&&) = delete;

    UI& operator=(const UI&) = delete;

    UI& operator=(UI&&) = delete;

    static void init(portMUX_TYPE& mux);

    static void addScreen(Canvas& screen, bool focused, const std::tuple<int, Canvas*, Canvas*>& screen_nav_map,
                          std::function<void()> on_select = {});

    static void draw();
};
