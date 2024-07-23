#include "Console.hpp"



Console *Console::instance = nullptr;

Console::Console() {}

void Console::init(){
    projection = glm::ortho(0.0f,window->getWidth(), 0.0f, window->getHight());
    initFreeType();
}


void Console::handleChar(int c, bool shift) {
    if(!isOpen) return;
    
    if (c > 64 && c < 91 && !shift) c += 32;
    switch (c)
    {
    case GLFW_KEY_ENTER:
        addMessage(processCommand(input));
        isOpen = false;
        input = "";
        break;
    case GLFW_KEY_BACKSPACE:
        if (!input.empty()) {
            input.pop_back();
        }
        break;
    default:
        if (shift) input += ShiftChar(c);
        else input += c;
        break;
    }
}

void Console::addMessage(const std::string &message) {
    std::istringstream stream(message);
    std::string line;
    while (std::getline(stream, line)) {
        if (lines.size() >= maxLines) {
            lines.erase(lines.begin());
        }
        lines.push_back(line);
    }
}


void Console::render() {
    
    glDisable(GL_DEPTH_TEST);

    int windowHeight = window->getHight();
    int windowWidth = window->getWidth();
    shaderProgram.use();
    glBindVertexArray(VAO);
    shaderProgram.setMat4("projection",projection);

    glActiveTexture(GL_TEXTURE0);

    float y = windowHeight;
    for (const auto &line : lines) {
        y -= lineSize * scale;
        renderText(line, 1.0f, y, scale);
    }

    if (isOpen) {
        renderText(input, lineSize * scale, lineSize * scale, scale);
    }

    if (debugInfo) {

        renderText(currentCameraPos(), windowWidth-12*lineSize*scale, windowHeight-lineSize * scale, scale);
        
    
        renderText(currentCameraOrent(), windowWidth-12*lineSize*scale, windowHeight-2*lineSize * scale, scale);

        renderText(currentObjectCount(), windowWidth-12*lineSize*scale, windowHeight-4*lineSize * scale, scale);


        renderText(currentTime(), windowWidth-12*lineSize*scale, windowHeight-6*lineSize * scale, scale);

    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_DEPTH_TEST);

}

// Other member functions...

void Console::initFreeType() {
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

void Console::renderText(const std::string &text, float x, float y, float scale) {
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

std::string Console::processCommand(const std::string &command) {
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

std::string Console::trim(const std::string &str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

std::string Console::extractCommand(const std::string &str) {
    size_t pos = str.find('(');
    if (pos == std::string::npos) {
        return trim(str);
    }
    return trim(str.substr(0, pos));
}
