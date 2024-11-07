#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <iostream>

#include "Model/TextBuffer.hpp"
#include "WNebulaModel.hpp"
#include "WNebulaPresenter.hpp"
#include "WNebulaView.hpp"

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
        std::shared_ptr<WNebulaPresenter> presenter = std::make_shared<WNebulaPresenter>();
        std::shared_ptr<WNebulaModel> model = std::make_shared<WNebulaModel>();
        std::shared_ptr<WNebulaView> view = std::make_shared<WNebulaView>(presenter);
        presenter->setup(view, model);
        presenter->run();
    } catch (const spdlog::spdlog_ex &ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        return 1;
    }

    spdlog::drop_all();
    return 0;
}