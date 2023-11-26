//
// Created by henry on 10/29/23.
//

#ifndef SCIFISHOOTER_CONSOLE_H
#define SCIFISHOOTER_CONSOLE_H

#include <hagame/graphics/primitives/quad.h>
#include <hagame/graphics/mesh.h>
#include <hagame/graphics/textBuffer.h>
#include <hagame/graphics/resolution.h>
#include <hagame/graphics/shaderProgram.h>
#include <variant>
#include "constants.h"

class Console {
public:

    enum class ArgType {
        String,
        Int,
        Float,
    };

    static std::vector<std::string> ARG_TYPE_NAMES;

    enum class Status {
        Open,
        Opening,
        Closing,
        Closed,
    };

    using arg_value_t = std::variant<std::string, int, float>;

    bool tryParse(ArgType type, std::string raw, arg_value_t& value);

    struct CommandArg {
        ArgType type;
        arg_value_t value;
    };

    using command_fn = std::function<int(std::vector<CommandArg>)>;

    struct Command {
        std::vector<ArgType> args;
        command_fn fn;

        std::string help() const;
    };

    const std::string HISTORY_FILE = ".history";
    const std::string PREFIX = "> ";
    const int WIDTH = GAME_SIZE[0] * 1.0;
    const int HEIGHT = GAME_SIZE[1] * 1.0;
    const float SLIDE_DUR = 0.125f;

    const hg::graphics::Color BG_COLOR = hg::graphics::Color(4, 30, 135, 150);
    const hg::graphics::Color FG_COLOR = hg::graphics::Color::white();

    Console(hg::graphics::Font* font);

    void registerCommand(std::string command, command_fn fn, std::vector<ArgType> args = {});

    // Close / open the console
    void toggle();

    // Enter current command
    void submit();

    // Add the latest typed character
    void newChar(char ch);

    // Update the current command
    void setCommand(std::string command);

    // Add a line to the buffer
    void putLine(std::string line);

    // Delete
    void backspace();

    void prevHistory();
    void nextHistory();

    Status status() const { return m_status; }

    void update(double dt);
    void render();

    bool isOpen() const { return m_status == Status::Opening || m_status == Status::Open; }

    std::unordered_map<std::string, Command> m_commands;

private:

    int m_historyIdx = 0;

    double m_elapsedTime;

    Status m_status = Status::Closed;
    float m_t = 0;

    std::string m_command;
    std::vector<std::string> m_buffer;

    hg::graphics::primitives::Quad m_consoleQuad;
    hg::graphics::primitives::Quad m_cursorQuad;
    std::unique_ptr<hg::graphics::MeshInstance> m_console;
    std::unique_ptr<hg::graphics::MeshInstance> m_cursor;
    std::unique_ptr<hg::graphics::TextBuffer> m_commandBuffer;
    std::unique_ptr<hg::graphics::TextBuffer> m_historyBuffer;

};

#endif //SCIFISHOOTER_CONSOLE_H
