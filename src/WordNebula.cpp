#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <iostream>
#include <memory>

#include "Model/WNebulaModel.hpp"
#include "Presenter/WNebulaPresenter.hpp"
#include "View/FtxuiView.hpp"

using namespace wnebula;

int main() {
    // Ensure the logs directory exists
    std::filesystem::create_directories("logs");

    try {
        auto fileLogger = spdlog::basic_logger_mt("basic_logger", "logs/WordNebula.log");
        spdlog::set_default_logger(fileLogger);
        spdlog::set_level(spdlog::level::info);
        spdlog::flush_on(spdlog::level::info); // Flush on every info log

        spdlog::info("Starting Word Nebula");
        const auto presenter = std::make_shared<WNebulaPresenter>();
        const auto model = std::make_shared<WNebulaModel>();
        const auto view = std::make_shared<FtxuiView>();
        presenter->setup(view, model);
        presenter->run();
    } catch (const spdlog::spdlog_ex &ex) {
        std::cerr << "Log initialization failed: " << ex.what() << '\n';
        return 1;
    }

    spdlog::drop_all();
    return 0;
}
