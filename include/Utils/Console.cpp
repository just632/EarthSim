#include "Console.hpp"



Console *Console::instance = nullptr;

Console::Console() {}

void Console::init(){
    projection = glm::ortho(0.0f,window->getWidth(), 0.0f, window->getHight());
    initFreeType();
}

void Console::handleChar(int c, bool shift) {
    if (!isOpen) return;

    // Convert uppercase to lowercase if shift is not pressed
    if (c > 64 && c < 91 && !shift) c += 32;

    switch (c) {
    case GLFW_KEY_ENTER:
        // Process the command and add it to history if not empty
        if (!input.empty()) {
            addMessage(processCommand(input));
            history.push_back(input);
        }
        // Reset input and history position
        input.clear();
        inputPos = 0;
        historyPos = 0;
        isOpen = false;
        break;

    case GLFW_KEY_BACKSPACE:
        if (inputPos > 0) {
            inputPos--;
            if (!input.empty()) input.erase(inputPos, 1);
        }
        break;

    case GLFW_KEY_LEFT:
        if (inputPos > 0) {
            inputPos--;
        }
        break;

    case GLFW_KEY_RIGHT:
        if (inputPos < input.size()) {
            inputPos++;
        }
        break;

    case GLFW_KEY_UP:
        if (historyPos < history.size()) {
            if (historyPos == 0) {
                tempInput = input; // Store the current input before navigating history
            }
            input = history[history.size() - 1 - historyPos];
            inputPos = input.size();
            historyPos++;
        }
        break;

    case GLFW_KEY_DOWN:
        if (historyPos > 0) {
            historyPos--;
            if (historyPos == 0) {
                input = tempInput; // Restore the stored input if at the end of history
            } else {
                input = history[history.size() - historyPos];
            }
            inputPos = input.size();
        }
        break;

    default:
        addInput(c, shift);
        break;
    }
}

void Console::addInput(int c,bool shift)
{
    input.insert(inputPos, 1, shift ? ShiftChar(c) : c);
    inputPos++;
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
        renderTextWithIndicator(input, lineSize * scale, lineSize * scale, scale);
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

    for (GLubyte c = 0; c < 255; c++) {
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

void Console::renderText(const std::string &text, float xpos, float ypos, float scale) {
    shaderProgram.setVec3("textColor", 1.0f, 1.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    for (auto c = text.begin(); c != text.end(); c++) {
        xpos = renderCharacter(*c, xpos, ypos, scale);
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Console::renderTextWithIndicator(const std::string &text, float xpos, float ypos, float scale) {
    shaderProgram.setVec3("textColor", 1.0f, 1.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    blinkTimer += timer->getDeltaTime();
    if (blinkTimer >= blinkInterval) {
        blinkTimer = 0.0f;
        showIndicator = !showIndicator;
    }
    for (size_t i = 0; i < text.size(); ++i) {
        // Render the indicator at the current input position
        if (i == inputPos && showIndicator) {
            shaderProgram.setVec3("textColor", .8f, .8f, .8f);
            renderCharacter('|', xpos, ypos, scale,3.f);
            shaderProgram.setVec3("textColor", 1.0f, 1.0f, 1.0f);
        }
        xpos = renderCharacter(text[i], xpos, ypos, scale);
    }

    // Render the indicator at the end if inputPos is at the end of the text
    if (inputPos == text.size() && showIndicator) {
        shaderProgram.setVec3("textColor", .8f, .8f, .8f);
        xpos = renderCharacter('|', xpos, ypos, scale,3.f);
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

float Console::renderCharacter(char c, float x, float y, float scale) {
    return renderCharacter(c,x,y,scale,0.f);
}

float Console::renderCharacter(char c,float x,float y,float scale,float shiftLeft)
{
    Character ch = Characters[c];
    GLfloat xpos = x + (ch.Bearing.x * scale) - (ch.Size.x * scale * shiftLeft);
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
    return x + (ch.Advance >> 6) * scale;
}

    std::string Console::processCommand(const std::string &command) {
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;

        auto it = commands.find(cmd);
        if (it != commands.end()) {
            std::string argsStr = command.substr(command.find(cmd) + cmd.length());
            argsStr = trim(argsStr);
            std::vector<std::string> args = splitArgs(argsStr);
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

std::vector<std::string> Console::splitArgs(const std::string &args) {
    std::vector<std::string> result;
    std::regex re(R"((\"[^\"]*\")|(\S+))");
    std::sregex_iterator it(args.begin(), args.end(), re);
    std::sregex_iterator end;
    while (it != end) {
        std::smatch match = *it;
        if (match[1].matched) {
            result.push_back(match[1].str().substr(1, match[1].str().length() - 2)); // Remove quotes
        } else {
            result.push_back(match[2].str());
        }
        ++it;
    }
    return result;
}