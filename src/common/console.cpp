//
// Created by henry on 10/29/23.
//
#include "console.h"
#include "constants.h"
#include <hagame/core/assets.h>
#include <hagame/utils/string.h>

using namespace hg;
using namespace hg::graphics;

std::vector<std::string> Console::ARG_TYPE_NAMES = {
    "string",
    "int",
    "float"
};

Console::Console(Font* font):
        m_consoleQuad(Vec2(WIDTH, HEIGHT), Vec2((GAME_SIZE[0] - WIDTH) / 2.0, -HEIGHT)),
        m_cursorQuad(Vec2(10, 24))
{
    m_consoleQuad.centered(false);
    m_console = std::make_unique<MeshInstance>(&m_consoleQuad);
    m_cursor = std::make_unique<MeshInstance>(&m_cursorQuad);
    m_commandBuffer = std::make_unique<TextBuffer>(font, "", Vec3(0, -50), TextHAlignment::Left);
    m_historyBuffer = std::make_unique<TextBuffer>(font, "", Vec3(0, -50), TextHAlignment::Left, TextVAlignment::Top);

    m_commandBuffer->text(PREFIX);

    m_buffer = utils::f_readLines(HISTORY_FILE);
    m_historyIdx = m_buffer.size();
}


void Console::submit() {

    putLine(m_command);

    std::vector<std::string> parts = utils::s_split(m_command, ' ');

    m_commandBuffer->text(PREFIX);

    if (parts.size() == 0) {
        return;
    }

    m_buffer.push_back(m_command);
    m_historyIdx = m_buffer.size();
    utils::f_append(HISTORY_FILE, m_command + "\n");
    m_command = "";

    if (m_commands.find(parts[0]) == m_commands.end()) {
        putLine("Command not found: " + parts[0]);
        return;
    }

    Command cmd = m_commands[parts[0]];

    if (parts.size() - 1 != cmd.args.size()) {
        putLine("Invalid arguments. Usage: " + parts[0] + " " + cmd.help());
        return;
    }

    std::vector<CommandArg> args;

    for (int i = 0; i < cmd.args.size(); i++) {
        std::string raw = parts[i + 1];
        CommandArg arg;
        arg.type = cmd.args[i];
        if (!tryParse(arg.type, raw, arg.value)) {
            putLine("Invalid arguments. Usage: " + parts[0] + " " + cmd.help());
            return;
        }
        args.push_back(arg);
    }

    int returnCode = cmd.fn(args);

    if (returnCode != 0) {
        putLine("Error code: " + std::to_string(returnCode));
    }

}

void Console::newChar(char ch) {
    m_command += ch;
    m_commandBuffer->text(PREFIX + m_command);
}

void Console::setCommand(std::string command) {
    m_command = command;
    m_commandBuffer->text(PREFIX + command);
}

void Console::toggle() {
    if (m_status == Status::Open || m_status == Status::Opening) {
        m_status = Status::Closing;
    } else {
        m_status = Status::Opening;
    }
}

void Console::update(double dt) {

    if (m_status == Status::Open || m_status == Status::Closed) {
        return;
    }

    auto font = m_historyBuffer->font();

    if (m_status == Status::Closing) {
        m_t = std::clamp<float>((m_t - dt / SLIDE_DUR), 0, 1);
        if (m_t == 0) {
            m_status = Status::Closed;
        }
    }

    if (m_status == Status::Opening) {
        m_t = std::clamp<float>(m_t + dt / SLIDE_DUR, 0, 1);
        if (m_t == 1.0) {
            m_status = Status::Open;
        }
    }

    m_consoleQuad.offset(Vec2((GAME_SIZE[0] - WIDTH) / 2.0, m_t * HEIGHT - HEIGHT));
    m_console->update(&m_consoleQuad);

    float cursorOffset = font->calcMessageSize(m_command)[0];
    m_cursorQuad.offset(Vec2((GAME_SIZE[0] - WIDTH) / 2.0 + cursorOffset, m_t * HEIGHT - HEIGHT));
    m_cursor->update(&m_cursorQuad);

    m_commandBuffer->pos(Vec3((GAME_SIZE[0] - WIDTH) / 2.0, m_t * HEIGHT - HEIGHT, 0));

    float historySize = font->calcMessageSize(m_historyBuffer->text())[1];
    m_historyBuffer->pos(Vec3(0, m_t * HEIGHT - HEIGHT + historySize - 26, 0));
}

void Console::render() {

    Mat4 proj = Mat4::Orthographic(0, GAME_SIZE[0], 0, GAME_SIZE[1], -100, 100);

    auto shader = hg::getShader("color");
    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", proj);
    shader->setMat4("model", Mat4::Identity());
    shader->setVec4("color", BG_COLOR);

    m_console->render();
    m_cursor->render();
    
    shader = hg::getShader("text_buffer");
    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", proj);
    shader->setMat4("model", Mat4::Identity());
    shader->setVec4("textColor", FG_COLOR);

    m_commandBuffer->render();
    m_historyBuffer->render();

}

void Console::backspace() {
    if (m_command.length() > 0) {
        m_command.pop_back();
        m_commandBuffer->text(PREFIX + m_command);
    }
}

void Console::registerCommand(std::string command, Console::command_fn fn, std::vector<ArgType> args) {
    m_commands.insert(std::make_pair(command, Command{args, fn}));
}

void Console::putLine(std::string line) {
    m_historyBuffer->text(m_historyBuffer->text() + "\n" + line);
    float size = m_historyBuffer->font()->calcMessageSize(m_historyBuffer->text())[1];
    m_historyBuffer->pos(Vec3(0, m_t * HEIGHT - HEIGHT + size - 26, 0));
}

bool Console::tryParse(ArgType type, std::string raw, Console::arg_value_t &value) {
    if (type == ArgType::String) {
        value = raw;
        return true;
    }

    try {
        if (type == ArgType::Int) {
            value = std::stoi(raw);
        }
        if (type == ArgType::Float) {
            value = std::stof(raw);
        }
        return true;
    } catch (...) {
        return false;
    }
}

void Console::prevHistory() {

    if (m_buffer.size() == 0 || m_historyIdx == 0) {
        return;
    }

    m_historyIdx--;

    setCommand(m_buffer[m_historyIdx]);
}

void Console::nextHistory() {

    if (m_buffer.size() == 0) {
        return;
    }

    m_historyIdx++;

    if (m_historyIdx >= m_buffer.size()) {
        m_historyIdx = m_buffer.size();
        setCommand("");
        return;
    }

    setCommand(m_buffer[m_historyIdx]);
}

std::string Console::Command::help() const {
    std::string params;
    for (const auto& arg : args) {
        params += "[" + ARG_TYPE_NAMES[(int)arg] + "] ";
    }
    return params;
}
