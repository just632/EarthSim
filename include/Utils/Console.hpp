#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>

struct Character {
    GLuint TextureID;  // ID handle of the glyph texture
    glm::ivec2 Size;   // Size of glyph
    glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
    GLuint Advance;    // Offset to advance to next glyph
};
