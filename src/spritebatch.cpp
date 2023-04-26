#include "spritebatch.h"
#include <vector>
#include "glm/glm.hpp"
#include "assetmanager.h"
#include "pyhelper.h"
#include <iostream>

SpriteBatch::~SpriteBatch() {
    free(m_quadInfoBuffer);
}

int SpriteBatch::getQuadId() {
    int next;
    if (_deallocated.empty()) {
        next = _nQuads;
        _nQuads++;
    } else {
        next = _deallocated.front();
        _deallocated.pop_front();
    }
    return next;
}


void SpriteBatch::configure(Shader* s) {

    if (s->getShaderType() != _shaderType) {
        return;
    }
    auto shaderProg = s->getProgId();
    GLuint BlockIndex = glGetUniformBlockIndex(shaderProg, "QuadInfo");

    glUniformBlockBinding(shaderProg, 0, BlockIndex);
    printf("BlockIndex %d\n", BlockIndex);

    glGetActiveUniformBlockiv(shaderProg, BlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &_blockSize);
    printf("Size %d\n", _blockSize);

    m_quadInfoBuffer = (GLubyte*)malloc(_blockSize);

    const int nElements = 4;
    const GLchar* Names[] = { "Pos", "Size", "TexCoords", "Repeat"};
    GLuint Indices[nElements] = { 0 };
    glGetUniformIndices(shaderProg, nElements, Names, Indices);

    GLint Offsets[nElements];
    glGetActiveUniformsiv(shaderProg, nElements, Indices, GL_UNIFORM_OFFSET, Offsets);

    m_quadInfoOffsets.Pos        = Offsets[0];
    m_quadInfoOffsets.Size       = Offsets[1];
    m_quadInfoOffsets.TexCoords  = Offsets[2];
    m_quadInfoOffsets.Repeat     = Offsets[3];

    for (uint i = 0 ; i < nElements ; i++) {
        printf("%s: %d %d\n", Names[i], Indices[i], Offsets[i]);
    }

    glGenBuffers(1, &m_uniformBuffer);
    printf("Uniform buffer %d\n", m_uniformBuffer);
}

SpriteBatch::SpriteBatch(const pybind11::kwargs& args) {
    _maxQuads = py_get_dict<int>(args, "max_quads");
    _shaderType = static_cast<ShaderType>(py_get_dict<int>(args, "shader_type"));
    _cam = args["cam"].cast<std::shared_ptr<Camera>>();


    _sheet = py_get_dict<std::string>(args, "sheet");
    auto& am = AssetManager::instance();
    auto tex = am.getTex(_sheet);

    auto isPalShader =_shaderType == ShaderType::SHADER_TEXTURE_PALETTE || _shaderType == ShaderType::QUAD_SHADER;

    if (isPalShader && !tex->hasPalette()) {
        std::cerr << "texture " << _sheet << " has no palette as required by spritebatch!";
        exit(1);
    }

    _texId = tex->getTexId();
    if (isPalShader) {
        _paletteId = tex->getDefaultPaletteId();
    }


    // vertices forming a quad
    const unsigned vertsPerQuad{6};
    glm::vec2 vertices[] = {
            glm::vec2(0.f, 0.f),                // bottom left
            glm::vec2(0.f, 1.f),                // top left
            glm::vec2(1.f, 1.f),                // top right
            glm::vec2(0.f, 0.f),                // bottom left
            glm::vec2(1.f, 1.f),                // top right
            glm::vec2(1.f, 0.f)                 // bottom right
    };
    //std::vector<glm::vec2> vertices_vec;
    //vertices_vec.resize(_maxQuads * vertsPerQuad);

    std::vector<V2> vertices_vec;
    vertices_vec.resize(_maxQuads * vertsPerQuad);

    for (uint i = 0; i < _maxQuads; ++i) {
        for (uint j = 0; j < vertsPerQuad; ++j) {
            vertices_vec[i * vertsPerQuad + j].pos = vertices[j];
            vertices_vec[i * vertsPerQuad + j].quadId = i;//vertices[j];
        }
    }

    // create the vertex array
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices_vec.size() * sizeof(V2), &vertices_vec[0], GL_STATIC_DRAW);

    auto stride = sizeof(V2);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, stride, (void*)(2*sizeof(float)));
}

void SpriteBatch::cleanUp() {

    _nQuads = 0;
    _deallocated.clear();
    memset(m_quadInfoBuffer, 0, _blockSize);
}



void SpriteBatch::draw(Shader* s) {



    if (s->getShaderType() != _shaderType) {
        return;
    }
    _cam->init(s);
    // set view matrix
    s->setMat4("view", _cam->getViewMatrix());
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, _blockSize, m_quadInfoBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uniformBuffer);

    if (_paletteId != GL_INVALID_VALUE) {
        s->setInt("tex_pal", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _paletteId);
    }
    s->setInt("tex_main", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texId);


    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, _nQuads * 6);
    glBindVertexArray(0);
}


void SpriteBatch::setQuad(int index, glm::vec3 worldPos, glm::vec2 size, glm::vec4 textureCoords, glm::vec2 repeat) {
    //assert(index < SPRITE_TECH_MAX_QUADS);

    auto* pBasePos = (glm::vec3*)(m_quadInfoBuffer + m_quadInfoOffsets.Pos);
    auto* pWidthHeight = (glm::vec2*) (m_quadInfoBuffer + m_quadInfoOffsets.Size);
    auto* pTexCoords = (glm::vec4*) (m_quadInfoBuffer + m_quadInfoOffsets.TexCoords);
    auto* pRepeat = (glm::vec2*) (m_quadInfoBuffer + m_quadInfoOffsets.Repeat);

    pBasePos[index].x = worldPos.x;
    pBasePos[index].y = worldPos.y;
    pBasePos[index].z = worldPos.z;

    pWidthHeight[index].x = size.x;
    pWidthHeight[index].y = size.y;

    pTexCoords[index].x = textureCoords.x;
    pTexCoords[index].y = textureCoords.y;
    pTexCoords[index].z = textureCoords.z;
    pTexCoords[index].w = textureCoords.w;

    pRepeat[index].x = repeat.x;
    pRepeat[index].y = repeat.y;

}