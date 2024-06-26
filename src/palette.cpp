#include "palette.h"
#include <GL/glew.h>
#include <cassert>

Palette::Palette(const std::vector<unsigned char>& in) : m_texId(GL_INVALID_VALUE) {
//
//    std::vector<unsigned char> data(256, 0.f);
//    for (size_t i = 0; i < in.size(); ++i) {
//        data[i] = in[i];// / 255.f;
//    }
//    glGenTextures (1, &m_texId);
//    glBindTexture (GL_TEXTURE_1D, m_texId);
//    glTexImage1D (GL_TEXTURE_1D, 0, GL_RGBA , 256, 0,  GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
//    glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameterf (GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameterf (GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    assert(in.size() % 1024 == 0);
    _palCount = in.size() / 1024;

    glGenTextures (1, &m_texId);
    glBindTexture (GL_TEXTURE_2D, m_texId);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, 256, _palCount, 0,  GL_RGBA, GL_UNSIGNED_BYTE, &in[0]);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
