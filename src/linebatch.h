#pragma once


#include "batch.h"

class LineBatch : public Batch {
public:
    LineBatch(const pybind11::kwargs&);
    virtual void computeOffsets(GLuint) ;
    void setLine(int index, glm::vec3 P0, glm::vec3 P1, glm::vec4 color);
private:
    struct {
        GLint P0        = 0;
        GLint P1        = 0;
        GLint Color     = 0;
    } m_lineInfoOffsets;
    struct V2 {
        float pos;
        GLuint quadId;

    };

};