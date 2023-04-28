#include "linebatch.h"
#include "pyhelper.h"

LineBatch::LineBatch(const pybind11::kwargs& args) : Batch(GL_LINES, 2) {
    _maxPrimitives = py_get_dict<int>(args, "max_lines");
    _shaderType = ShaderType::LINE_SHADER;
    // vertices forming a line
    float vertices[] = {0, 1};
    _cam = args["cam"].cast<std::shared_ptr<Camera>>();
    std::vector<V2> vertices_vec;
    vertices_vec.resize(_maxPrimitives * _verticesPerPrimitive);

    for (uint i = 0; i < _maxPrimitives; ++i) {
        for (uint j = 0; j < _verticesPerPrimitive; ++j) {
            vertices_vec[i * _verticesPerPrimitive + j].pos = vertices[j];
            vertices_vec[i * _verticesPerPrimitive + j].quadId = i;
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
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, stride, (void*)(sizeof(float)));
}

void LineBatch::computeOffsets(GLuint shaderProg) {
    const int nElements = 3;
    const GLchar* Names[] = { "Pos0", "Pos1", "Color"};
    GLuint Indices[nElements] = { 0 };
    glGetUniformIndices(shaderProg, nElements, Names, Indices);

    GLint Offsets[nElements];
    glGetActiveUniformsiv(shaderProg, nElements, Indices, GL_UNIFORM_OFFSET, Offsets);

    m_lineInfoOffsets.P0    = Offsets[0];
    m_lineInfoOffsets.P1    = Offsets[1];
    m_lineInfoOffsets.Color = Offsets[2];

    for (uint i = 0 ; i < nElements ; i++) {
        printf("%s: %d %d\n", Names[i], Indices[i], Offsets[i]);
    }
}

void LineBatch::setLine(int index, glm::vec3 P0, glm::vec3 P1, glm::vec4 color) {

    auto* p0Pos = (glm::vec3*)(m_quadInfoBuffer + m_lineInfoOffsets.P0);
    auto* p1Pos = (glm::vec3*) (m_quadInfoBuffer + m_lineInfoOffsets.P1);
    auto* pColor = (glm::vec4*) (m_quadInfoBuffer + m_lineInfoOffsets.Color);

    p0Pos[index].x = P0.x;
    p0Pos[index].y = P0.y;
    p0Pos[index].z = P0.z;

    p1Pos[index].x = P1.x;
    p1Pos[index].y = P1.y;
    p1Pos[index].z = P1.z;

    pColor[index].x = color.x;
    pColor[index].y = color.y;
    pColor[index].z = color.z;
    pColor[index].w = color.w;

}