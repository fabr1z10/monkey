#include "quadbatch.h"
#include <vector>
#include "glm/glm.hpp"
#include "../assetmanager.h"
#include "../pyhelper.h"
#include <iostream>




//
//void SpriteBatch::computeOffsets(GLuint shaderProg) {
//    const int nElements = 5;
//    const GLchar* Names[] = { "Pos", "Size", "TexCoords", "Repeat", "Palette"};
//    GLuint Indices[nElements] = { 0 };
//    glGetUniformIndices(shaderProg, nElements, Names, Indices);
//
//    GLint Offsets[nElements];
//    glGetActiveUniformsiv(shaderProg, nElements, Indices, GL_UNIFORM_OFFSET, Offsets);
//
//    m_quadInfoOffsets.Pos        = Offsets[0];
//    m_quadInfoOffsets.Size       = Offsets[1];
//    m_quadInfoOffsets.TexCoords  = Offsets[2];
//    m_quadInfoOffsets.Repeat     = Offsets[3];
//    m_quadInfoOffsets.Palette    = Offsets[4];
//
//    for (uint i = 0 ; i < nElements ; i++) {
//        printf("%s: %d %d\n", Names[i], Indices[i], Offsets[i]);
//    }
//}
//
//void SpriteBatch::configure(Shader* s) {
//
//    if (s->getShaderType() != _shaderType) {
//        return;
//    }
//    auto shaderProg = s->getProgId();
//    GLuint BlockIndex = glGetUniformBlockIndex(shaderProg, "QuadInfo");
//
//    glUniformBlockBinding(shaderProg, 0, BlockIndex);
//    printf("BlockIndex %d\n", BlockIndex);
//
//    glGetActiveUniformBlockiv(shaderProg, BlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &_blockSize);
//    printf("Size %d\n", _blockSize);
//
//    m_quadInfoBuffer = (GLubyte*)malloc(_blockSize);
//
//
//
//    glGenBuffers(1, &m_uniformBuffer);
//    printf("Uniform buffer %d\n", m_uniformBuffer);
//}
//

void QuadBatch::innerConfigure() {

	auto stride = sizeof(QuadBatchVertexData);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(QuadBatchVertexData, textureBounds));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(QuadBatchVertexData, textureCoords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(QuadBatchVertexData, palette));


	// this depends on the particular batch and should go in a virtual method
	std::vector<unsigned> indices;
	for (size_t i = 0; i < _maxElements; ++i) {
		indices.push_back(4 * i);
		indices.push_back(4 * i + 1);
		indices.push_back(4 * i + 2);
		indices.push_back(4 * i + 2);
		indices.push_back(4 * i + 3);
		indices.push_back(4 * i);
	}

	glGenBuffers(1, &_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

}

QuadBatch::QuadBatch(int maxElements, const pybind11::kwargs& args) : Batch(maxElements, 4, 6) {
	_prim = GL_TRIANGLES;
	_cam = args["cam"].cast<std::shared_ptr<Camera>>();

    //_maxPrimitives = py_get_dict<int>(args, "max_quads");
    //_shaderType = static_cast<ShaderType>(py_get_dict<int>(args, "shader_type"));

    _sheet = py_get_dict<std::string>(args, "sheet");
    auto& am = AssetManager::instance();
    auto tex = am.getTex(_sheet);

    if (!tex->hasPalette()) {
        std::cerr << "texture " << _sheet << " has no palette as required by spritebatch!";
        exit(1);
    }

    _texId = tex->getTexId();
    _paletteId = tex->getDefaultPaletteId();
    _paletteCount = tex->getPaletteCount();
    _invPaletteCount = 1.0f / _paletteCount;

    // vertices forming a quad
//    glm::vec2 vertices[] = {
//            glm::vec2(0.f, 0.f),                // bottom left
//            glm::vec2(0.f, 1.f),                // top left
//            glm::vec2(1.f, 1.f),                // top right
//            glm::vec2(0.f, 0.f),                // bottom left
//            glm::vec2(1.f, 1.f),                // top right
//            glm::vec2(1.f, 0.f)                 // bottom right
//    };
//    //std::vector<glm::vec2> vertices_vec;
//    //vertices_vec.resize(_maxQuads * vertsPerQuad);
//
//    std::vector<V2> vertices_vec;
//    vertices_vec.resize(_maxPrimitives * _verticesPerPrimitive);
//
//    for (uint i = 0; i < _maxPrimitives; ++i) {
//        for (uint j = 0; j < _verticesPerPrimitive; ++j) {
//            vertices_vec[i * _verticesPerPrimitive + j].pos = vertices[j];
//            vertices_vec[i * _verticesPerPrimitive + j].quadId = i;//vertices[j];
//        }
//    }

    // create the vertex array
    //glGenVertexArrays(1, &_vao);
    //glGenBuffers(1, &_vbo);

    //glBindVertexArray(_vao);

    //glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    //glBufferData(GL_ARRAY_BUFFER, vertices_vec.size() * sizeof(V2), &vertices_vec[0], GL_STATIC_DRAW);

}

void QuadBatch::releaseQuad(int index) {
	_deallocated.push_back(index);
	int offset = index * _vertsPerElement;
	memset(&_data[offset], 0, sizeof(QuadBatchVertexData) * 4);
}

void QuadBatch::setQuad(int index, glm::vec3 bottomLeft, glm::vec2 size, glm::vec4 textureBounds, glm::vec2 textureRepeat,
						int palette, bool fliph, bool flipv)
{

	float txl = fliph ? textureRepeat.x : 0.f;
	float txr = fliph ? 0.f : textureRepeat.x;
	float tyb = flipv ? 0.f : textureRepeat.y;
	float tyt = flipv ? textureRepeat.y : 0.f;
	float palY = _invPaletteCount * (0.5f + palette);
	int offset = index * _vertsPerElement;

	_data[offset].position = bottomLeft;
	_data[offset].textureBounds = textureBounds;
	_data[offset].palette = palY;
	_data[offset].textureCoords = glm::vec2(txl, tyb);

	_data[offset+1].position = bottomLeft + glm::vec3(size.x, 0.f, 0.f);
	_data[offset+1].textureBounds = textureBounds;
	_data[offset+1].palette = palY;
	_data[offset+1].textureCoords = glm::vec2(txr, tyb);

	_data[offset+2].position = bottomLeft + glm::vec3(size.x, size.y, 0.f);
	_data[offset+2].textureBounds = textureBounds;
	_data[offset+2].palette = palY;
	_data[offset+2].textureCoords = glm::vec2(txr, tyt);

	_data[offset+3].position = bottomLeft + glm::vec3(0.f, size.y, 0.f);
	_data[offset+3].textureBounds = textureBounds;
	_data[offset+3].palette = palY;
	_data[offset+3].textureCoords = glm::vec2(txl, tyt);


}

void QuadBatch::initDraw(Shader* s) {

	// initialize texture and palette texture

	s->setInt("tex_pal", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _paletteId);

    s->setInt("tex_main", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texId);

}

//void SpriteBatch::setQuad(int index, glm::vec3 worldPos, glm::vec2 size, glm::vec4 textureCoords, glm::vec2 repeat, int paletteIndex,
//                          bool flipx, bool flipy) {
//    //assert(index < SPRITE_TECH_MAX_QUADS);
//
//    auto* pBasePos = (glm::vec4*)(m_quadInfoBuffer + m_quadInfoOffsets.Pos);
//    auto* pWidthHeight = (glm::vec2*) (m_quadInfoBuffer + m_quadInfoOffsets.Size);
//    auto* pTexCoords = (glm::vec4*) (m_quadInfoBuffer + m_quadInfoOffsets.TexCoords);
//    auto* pRepeat = (glm::vec2*) (m_quadInfoBuffer + m_quadInfoOffsets.Repeat);
//    auto* pPalette = (float*) (m_quadInfoBuffer + m_quadInfoOffsets.Palette);
//
//    pBasePos[index].x = worldPos.x;
//    pBasePos[index].y = worldPos.y;
//    pBasePos[index].z = worldPos.z;
//    pBasePos[index].w = 1.0;
//
//    pWidthHeight[index].x = size.x;
//    pWidthHeight[index].y = size.y;
//
//    if (flipx) {
//        pTexCoords[index].x = textureCoords.y;
//        pTexCoords[index].y = textureCoords.x;
//    } else {
//        pTexCoords[index].x = textureCoords.x;
//        pTexCoords[index].y = textureCoords.y;
//    }
//    if (flipy) {
//        pTexCoords[index].z = textureCoords.w;
//        pTexCoords[index].w = textureCoords.z;
//
//    } else {
//        pTexCoords[index].z = textureCoords.z;
//        pTexCoords[index].w = textureCoords.w;
//    }
//
//
//    pRepeat[index].x = repeat.x;
//    pRepeat[index].y = repeat.y;
//
//    pPalette[index] = _invPaletteCount * (0.5f + paletteIndex);
//
//}