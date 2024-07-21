#include "Console.hpp"
#include <iostream>
#include <sstream>
#include "Engine.hpp"  // Adjust path as necessary


ConsoleBuffer::ConsoleBuffer(Engine* engine, int maxLines) : engine(engine), maxLines(maxLines),isOpen(false),debugInfo(false) {
}

void ConsoleBuffer::load(){
    projection = glm::ortho(0.0f, float(engine->windowWidth), 0.0f, float(engine->windowHight));
    initFreeType();
    commands = engine->getCommands();
    std::cout << "ConsoleBuffer recived commands : START" << std::endl;
    for(auto cmd : commands){

        std::cout << cmd.first << std::endl;
    }
    std::cout << "ConsoleBuffer recived commands : END" << std::endl;

    
}

int ConsoleBuffer::addShiftInput(int c) {
    switch (c) {
        case '`': return '~';
        case '1': return '!';
        case '2': return '@';
        case '3': return '#';
        case '4': return '$';
        case '5': return '%';
        case '6': return '^';
        case '7': return '&';
        case '8': return '*';
        case '9': return '(';
        case '0': return ')';
        case '-': return '_';
        case '=': return '+';
        case '[': return '{';
        case ']': return '}';
        case '\\': return '|';
        case ';': return ':';
        case '\'': return '"';
        case ',': return '<';
        case '.': return '>';
        case '/': return '?';
        default: return c;
    }
}

void ConsoleBuffer::addInput(int c, bool shift) {
    if(!isOpen) return;
    
    if (c > 64 && c < 91 && !shift) c += 32;
    if (c == GLFW_KEY_ENTER) {
        addMessage(processCommand(input));
        isOpen = false;
        input = "";
    } else if (c == GLFW_KEY_BACKSPACE) {
        if (!input.empty()) {
            input.pop_back();
        }
    } else {
        if (shift) input += addShiftInput(c);
        else input += c;
    }
}

void ConsoleBuffer::addMessage(const std::string &message) {
    std::istringstream stream(message);
    std::string line;
    while (std::getline(stream, line)) {
        if (lines.size() >= maxLines) {
            lines.erase(lines.begin());
        }
        lines.push_back(line);
    }
}


void ConsoleBuffer::render() {
    
    glDisable(GL_DEPTH_TEST);

    int windowHeight = engine->windowHight;
    shaderProgram.use();
    glBindVertexArray(VAO);
    shaderProgram.setMat4( "projection",projection);

    glActiveTexture(GL_TEXTURE0);

    float y = windowHeight;
    for (const auto &line : lines) {
        y -= lineSize * SCALE;
        renderText(line, 1.0f, y, SCALE);
    }

    if (isOpen) {
        renderText(input, lineSize * SCALE, lineSize * SCALE, SCALE);
    }

    if (debugInfo) {

        renderText(engine->currentCameraPos(), engine->windowWidth-12*lineSize*SCALE, windowHeight-lineSize * SCALE, SCALE);
        
    
        renderText(engine->currentCameraOrent(), engine->windowWidth-12*lineSize*SCALE, windowHeight-2*lineSize * SCALE, SCALE);

        renderText(engine->currentObjectCount(), engine->windowWidth-12*lineSize*SCALE, windowHeight-4*lineSize * SCALE, SCALE);

    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_DEPTH_TEST);

}

// Other member functions...

void ConsoleBuffer::initFreeType() {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, "fonts/JetBrainsMono-Light.ttf", 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, lineSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (GLubyte c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)};
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    
    shaderProgram.load();
}

void ConsoleBuffer::renderText(const std::string &text, float x, float y, float scale) {
    shaderProgram.setVec3("textColor", 1.0f, 1.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    for (auto c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];
        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

std::string ConsoleBuffer::processCommand(const std::string &command) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    auto it = commands.find(extractCommand(cmd));
    if (it != commands.end()) {
        size_t start = command.find('(') + 1;
        size_t end = command.find_last_of(')');
        std::string args = (start < end) ? command.substr(start, end - start) : "";
        args = trim(args);
        return it->second(args);
    } else {
        return "Unknown command: " + cmd;
    }
}

std::string ConsoleBuffer::trim(const std::string &str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

std::string ConsoleBuffer::extractCommand(const std::string &str) {
    size_t pos = str.find('(');
    if (pos == std::string::npos) {
        return trim(str);
    }
    return trim(str.substr(0, pos));
}
