#ifndef WNEBULAPRESENTER_HPP
#define WNEBULAPRESENTER_HPP
#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include "TextBuffer.hpp"
#include "WNebulaView.hpp"

namespace wnebula {

    class WNebulaPresenter {
    public:
        WNebulaPresenter();
        ~WNebulaPresenter();
        void onInsert(char c);
        void onDelete();
        void onMoveCursor(int delta);
        void onExit();
        void setView(std::shared_ptr<WNebulaView> view);

        void run();
    private:
        std::weak_ptr<WNebulaView> view; // Use weak_ptr to avoid circular dependency
        std::shared_ptr<TextBuffer> model;
        bool isRunning = true;
    };
} // namespace wnebula

#endif // WNEBULAPRESENTER_HPP