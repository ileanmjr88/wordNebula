#include "WNebulaPresenter.hpp"
#include "WNebulaView.hpp"
#include "TextBuffer.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

using namespace wnebula;

int main() {
    auto file_logger = spdlog::basic_logger_mt("basic_logger", "logs/WordNebula.log");
    spdlog::set_default_logger(file_logger);
    spdlog::set_level(spdlog::level::info);
    
    auto presenter = std::make_shared<WNebulaPresenter>();
    auto view = std::make_shared<WNebulaView>(presenter);

    presenter->run();

    return 0;
}