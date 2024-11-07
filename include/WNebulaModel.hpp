#ifndef WNEBULAMODEL_HPP
#define WNEBULAMODEL_HPP

#include "Model/IBuffer.hpp"
#include "Model/TextBuffer.hpp"
#include <memory>
#include <spdlog/spdlog.h>

namespace wnebula {

class WNebulaModel : public IBuffer {
public:
    WNebulaModel();
    WNebulaModel(std::shared_ptr<TextBuffer> textBuffer);
    virtual ~WNebulaModel() override = default;

    void insert(char c) override;
    void insertChar(char c) override;
    void deleteChar() override;
    void deleteText(int position, int length) override;
    void moveCursor(int offset) override;
    std::string getText() const override;
    int getCursorPosition() const override;

private:
    std::shared_ptr<TextBuffer> buffer;
};

} // namespace wnebula

#endif // WNEBULAMODEL_HPP