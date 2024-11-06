#include "WNebulaPresenter.hpp"
#include "WNebulaView.hpp"
#include "TextBuffer.hpp"

using namespace wnebula;

int main() {
    auto presenter = std::make_shared<WNebulaPresenter>();
    auto view = std::make_shared<WNebulaView>(presenter);

    presenter->run();

    return 0;
}