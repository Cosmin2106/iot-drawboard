#include "ui/ui.h"

#include <utility>

#include "hw_peripherals.h"
#include "ui/list.h"

UI::TrackedHWState UI::current_hw_state_{};
UI::TrackedHWState UI::old_hw_state_{};
bool UI::change_screens_ = false;
std::map<Canvas*, std::vector<UINode>> UI::ui_graph_{};

void UI::init(portMUX_TYPE& mux) {
    mux_ = &mux;
}

void UI::setActiveScreen(Canvas& screen) {
    if (active_screen_) {
        active_screen_->focus(false);
    }
    active_screen_ = &screen;
    screen.focus(true);
    change_screens_ = true;
}

void UI::select(const int node_id) {
    const auto screen_node = ui_graph_.find(active_screen_);
    if (screen_node == ui_graph_.end()) {
        return;
    }

    for (const auto& node : screen_node->second) {
        if (node.node_id == node_id) {
            if (node.to) {
                setActiveScreen(*node.to);
            }
            else if (node.on_select) {
                node.on_select();
            }
            return;
        }
    }
}

void UI::back() {
    const auto screen_node = ui_graph_.find(active_screen_);
    if (screen_node == ui_graph_.end()) {
        return;
    }

    if (!screen_node->second.empty() && screen_node->second[0].from) {
        // All elements in a canvas should point to the same source
        setActiveScreen(*screen_node->second[0].from);
    }
}

void UI::lazyDraw() {
    portENTER_CRITICAL(mux_);
    current_hw_state_.trigger_left_pressed = HWPeripherals::trigger_left_pressed;
    current_hw_state_.trigger_right_pressed = HWPeripherals::trigger_right_pressed;
    current_hw_state_.pot_left_perc = HWPeripherals::pot_left_perc;
    current_hw_state_.pot_right_perc = HWPeripherals::pot_right_perc;
    current_hw_state_.bat_perc = static_cast<int>(HWPeripherals::bat_perc_raw);
    portEXIT_CRITICAL(mux_);

    if (current_hw_state_ != old_hw_state_ || change_screens_) {
        active_screen_->draw();
        old_hw_state_ = current_hw_state_;
        change_screens_ = false;
    }
}

void UI::addScreen(Canvas& screen, const bool focused,
                   std::tuple<int, Canvas*, Canvas*, std::function<void()>> screen_nav_map) {
    ui_graph_[&screen].push_back({
        .node_id = std::get<0>(screen_nav_map),
        .from = std::get<1>(screen_nav_map),
        .to = std::get<2>(screen_nav_map),
        .on_select = std::move(std::get<3>(screen_nav_map))
    });

    if (focused) {
        setActiveScreen(screen);
    }
    else if (active_screen_ != &screen) {
        screen.focus(false);
    }
}

void UI::draw() {
    if (!active_screen_) {
        return;
    }

    if (auto* active_list = dynamic_cast<UIList*>(active_screen_)) {
        portENTER_CRITICAL(mux_);
        if (active_list->size()) {
            active_list->setCurrentPosition(HWPeripherals::pot_left_perc * static_cast<int>(active_list->size()) / 100);
        }
        portEXIT_CRITICAL(mux_);
    }
    lazyDraw();
}
