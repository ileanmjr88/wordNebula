#include "WNebulaPresenter.hpp"
#include "WNebulaView.hpp"
#include "TextBuffer.hpp"
#include <filesystem>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <iostream>
using namespace wnebula;

int main() {
    // Ensure the logs directory exists
    std::filesystem::create_directories("logs");

    try {
        auto file_logger = spdlog::basic_logger_mt("basic_logger", "logs/WordNebula.log");
        spdlog::set_default_logger(file_logger);
        spdlog::set_level(spdlog::level::info);
        spdlog::flush_on(spdlog::level::info); // Flush on every info log

        spdlog::info("Starting Word Nebula");
        auto presenter = std::make_shared<WNebulaPresenter>();
        auto view = std::make_shared<WNebulaView>(presenter);
        presenter->setView(view);
        presenter->run();
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        return 1;
    }

    spdlog::drop_all();
    return 0;
}