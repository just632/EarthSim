#include "Console.hpp"
#include <iostream>
#include <sstream>
#include "Engine.hpp"  // Adjust path as necessary

ConsoleBuffer::ConsoleBuffer(Engine& engine, int maxLines) : engine(engine), maxLines(maxLines) {
    commands = engine.getCommands();
}

void ConsoleBuffer::load(){
    projection = glm::ortho(0.0f, float(engine.windowWidth), 0.0f, float(engine.windowHight));
    initFreeType();
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
    if (lines.size() >= maxLines) {
        lines.erase(lines.begin());
    }
    lines.push_back(message);
}

void ConsoleBuffer::render() {
    int windowHeight = engine.windowHight;
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    float y = windowHeight;
    for (const auto &line : lines) {
        y -= lineSize * SCALE;
        renderText(line, 1.0f, y, SCALE);
    }

    if (isOpen) {
        renderText(input, lineSize * SCALE, lineSize * SCALE, SCALE);
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
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

    const char *vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec4 vertex;
        out vec2 TexCoords;
        uniform mat4 projection;
        void main() {
            gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
            TexCoords = vertex.zw;
        }
    )";
    
    const char *fragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoords;
        out vec4 color;
        uniform sampler2D text;
        uniform vec3 textColor;
        void main() {    
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
            color = vec4(textColor, 1.0) * sampled;
        }
    )";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void ConsoleBuffer::renderText(const std::string &text, float x, float y, float scale) {
    glUniform3f(glGetUniformLocation(shaderProgram, "textColor"), 1.0f, 1.0f, 1.0f);
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
