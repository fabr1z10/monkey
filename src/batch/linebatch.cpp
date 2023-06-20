#include "linebatch.h"
#include "../pyhelper.h"
#include <iostream>


LineBatch::LineBatch(int maxElements, std::shared_ptr<Camera> cam) : Batch(maxElements, 2, 2) {
	_prim = GL_LINES;
	_cam = cam;// args["cam"].cast<std::shared_ptr<Camera>>();
//
//	_maxPrimitives = py_get_dict<int>(args, "max_lines");
//    _shaderType = ShaderType::LINE_SHADER;
//    // vertices forming a line
//    float vertices[] = {0, 1};
//    std::vector<V2> vertices_vec;
//    vertices_vec.resize(_maxPrimitives * _verticesPerPrimitive);
//
//    for (uint i = 0; i < _maxPrimitives; ++i) {
//        for (uint j = 0; j < _verticesPerPrimitive; ++j) {
//            vertices_vec[i * _verticesPerPrimitive + j].pos = vertices[j];
//            vertices_vec[i * _verticesPerPrimitive + j].quadId = i;
//        }
//    }
//
//    // create the vertex array
//    glGenVertexArrays(1, &_vao);
//    glGenBuffers(1, &_vbo);
//
//    glBindVertexArray(_vao);
//
//    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
//    glBufferData(GL_ARRAY_BUFFER, vertices_vec.size() * sizeof(V2), &vertices_vec[0], GL_STATIC_DRAW);
//
//    auto stride = sizeof(V2);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, stride, 0);
//    glEnableVertexAttribArray(1);
//    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, stride, (void*)(sizeof(float)));
//    glBindVertexArray(0);
}

void LineBatch::innerConfigure() {

	auto stride = sizeof(LineBatchVertexData);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(float)));

    // this depends on the particular batch and should go in a virtual method
    std::vector<unsigned> indices;
    for (size_t i = 0; i < _maxElements; ++i) {
        indices.push_back(2 * i);
        indices.push_back(2 * i + 1);
    }

    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
}

//void LineBatch::computeOffsets(GLuint shaderProg) {
//    const int nElements = 3;
//    const GLchar* Names[] = { "Pos0", "Pos1", "Color"};
//    GLuint Indices[nElements] = { 0 };
//    glGetUniformIndices(shaderProg, nElements, Names, Indices);
//
//    GLint Offsets[nElements];
//    glGetActiveUniformsiv(shaderProg, nElements, Indices, GL_UNIFORM_OFFSET, Offsets);
//
//    m_lineInfoOffsets.P0    = Offsets[0];
//    m_lineInfoOffsets.P1    = Offsets[1];
//    m_lineInfoOffsets.Color = Offsets[2];
//
//    for (uint i = 0 ; i < nElements ; i++) {
//        printf("%s: %d %d\n", Names[i], Indices[i], Offsets[i]);
//    }
//}
//
void LineBatch::setLine(int index, glm::vec3 P0, glm::vec3 P1, glm::vec4 color) {

    int offset = index * _vertsPerElement;
    _data[offset].position = P0;
    _data[offset].color = color;
    _data[offset+1].position = P1;
    _data[offset+1].color = color;


//    auto* p0Pos = (glm::vec4*)(m_quadInfoBuffer + m_lineInfoOffsets.P0);
//    auto* p1Pos = (glm::vec4*) (m_quadInfoBuffer + m_lineInfoOffsets.P1);
//    auto* pColor = (glm::vec4*) (m_quadInfoBuffer + m_lineInfoOffsets.Color);
//
//    p0Pos[index].x = P0.x;
//    p0Pos[index].y = P0.y;
//    p0Pos[index].z = P0.z;
//    p0Pos[index].w = 0.f;
//
//    p1Pos[index].x = P1.x;
//    p1Pos[index].y = P1.y;
//    p1Pos[index].z = P1.z;
//    p1Pos[index].w = 0.f;
//
//    pColor[index].x = color.x;
//    pColor[index].y = color.y;
//    pColor[index].z = color.z;
//    pColor[index].w = color.w;
//
}

//void LineBatch::draw(Shader * s) {
//
//    float* ciccio = (float*)m_quadInfoBuffer;
//    for (size_t i = 0; i < 20; i++) {
//        std::cout << *ciccio << "\n";
//        ciccio++;
//    }
//    std::cout << "---\n";
//    ciccio =(float*)(m_quadInfoBuffer + m_lineInfoOffsets.P1);
//    for (size_t i = 0; i < 20; i++) {
//        std::cout << *ciccio << "\n";
//        ciccio++;
//    }
//    std::cout << "###\n";
//    Batch::draw(s);
//}
