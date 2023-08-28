#pragma once

#include "../batch.h"

#include <string>
#include <glm/glm.hpp>
#include <pybind11/pytypes.h>
//#include "symbols.h"

class SpriteSheet;

struct QuadBatchVertexData {
	glm::vec3 position;
	glm::vec4 textureBounds;
	glm::vec2 textureCoords;
	float palette;
	//unsigned camera;
} ;


class QuadBatch : public Batch<QuadBatchVertexData> {
public:
    QuadBatch(const pybind11::kwargs&);

    void initDraw(Shader* s) override;

    //[[nodiscard]] std::string getSheet() const;

	void innerConfigure() override;


	void setQuad (int index, glm::vec3 bottomLeft, glm::vec2 size, glm::vec4 textureBounds, glm::vec2 textureRepeat,
			   int palette, bool fliph, bool flipv, float zLayer);

	void setInvisible(int index) override;
private:
    //std::string _sheet;
    GLuint _texId;
    GLuint _paletteId;
    int _paletteCount;
    float _invPaletteCount;
    SpriteSheet* _sheet;
};

//inline std::string QuadBatch::getSheet() const {
//    return _sheet;
//}


