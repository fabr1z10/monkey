#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>
#include <memory>
#include "palette.h"

enum TexFilter { nearest, linear, linear_mipmap };

class Tex {
public:
    Tex (const std::string& file);
    ~Tex () ;
    int getWidth () const;
    int getHeight() const;
    GLuint getTexId() const;
    bool hasPalette() const;
    GLuint getDefaultPaletteId() const;
    int getPaletteCount() const;
    static std::vector<unsigned char> getRaw(const std::string& , int&, int&);
private:
    void load_png(const std::string& file);
    void load_generic(const std::string& file);
    int m_width;
    int m_height;
    GLuint m_texId;
    GLuint m_defaultPaletteId;
    bool m_palette;
    std::shared_ptr<Palette> _pal;
};

inline int Tex::getWidth () const { return m_width; }
inline int Tex::getHeight() const { return m_height; }
inline GLuint Tex::getTexId() const { return m_texId; }
inline bool Tex::hasPalette() const { return m_palette; }
inline GLuint Tex::getDefaultPaletteId() const { return m_defaultPaletteId; }
inline int Tex::getPaletteCount() const {
    return _pal->getPaletteCount();
}

