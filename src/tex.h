#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>
#include <memory>
#include "palette.h"
#include <unordered_map>


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
    const unsigned char * getPal();
    static std::vector<unsigned char> getRaw(const std::string& , int&, int&);
    void assignPalette(std::shared_ptr<Palette>);
    int getColorIndex (unsigned long color);
private:
    void load_png(const std::string& file);
    void load_generic(const std::string& file);
    int m_width;
    int m_height;
    GLuint m_texId;
    GLuint m_defaultPaletteId;
    bool m_palette;
    std::shared_ptr<Palette> _palette;
    std::unordered_map<int, int> _indices;
    unsigned char* _pal;
};

inline const unsigned char * Tex::getPal() {
	return _pal;
}
inline int Tex::getWidth () const { return m_width; }
inline int Tex::getHeight() const { return m_height; }
inline GLuint Tex::getTexId() const { return m_texId; }
inline bool Tex::hasPalette() const { return _pal != nullptr; }
inline GLuint Tex::getDefaultPaletteId() const { return _palette->getTexId(); }
inline int Tex::getPaletteCount() const {
    return _palette->getPaletteCount();
}

