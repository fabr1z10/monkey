#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "enginedraw.h"
#include "engine.h"
#include "shaders/batchshader.h"
#include "shaders/shaders.h"
#include "error.h"


extern GLFWwindow* window;

EngineDraw::EngineDraw() {
	m_shaderBuilders[ShaderType::BATCH_QUAD_PALETTE] = [&] () {
		return std::make_shared<BatchShader>(ShaderType::BATCH_QUAD_PALETTE, quad_vs, quad_fs, "2f1I"); };
    m_shaderBuilders[ShaderType::BATCH_LINES] = [&] () {
    	return std::make_shared<BatchShader>(ShaderType::BATCH_LINES, line_vs, line_fs, "3f4f"); };
//	m_shaderBuilders[ShaderType::BATCH_COLOR_TRI] = [&] () {
//		return std::make_shared<BatchShader>(ShaderType::BATCH_COLOR_TRI, line_vs, line_fs, "3f4f"); };
	m_shaderBuilders[ShaderType::BATCH_QUAD_NO_PALETTE] = [&] () {
		return std::make_shared<BatchShader>(ShaderType::BATCH_QUAD_NO_PALETTE, quad_vs_nopal, quad_fs_nopal, "2f1I"); };
	m_shaderBuilders[ShaderType::SHADER_SKELETAL] = [&] () {
		return std::make_shared<BatchShader>(ShaderType::SHADER_SKELETAL, skeletal_vs, skeletal_fs, "3f2f3f"); };

}

Shader * EngineDraw::getShader(ShaderType type) {
	auto it = _indices.find(type);
	if (it == _indices.end()) {
		GLIB_FAIL("Shader " << type << " not loaded.");
	}
	return m_shaders[it->second].get();
}

void EngineDraw::addShader(int type) {
	auto shader = m_shaderBuilders[type]();
	_indices.insert(std::make_pair(static_cast<ShaderType>(type), m_shaders.size()));
	m_shaders.push_back(shader);
}

void EngineDraw::init(Room * room) {



    for (size_t i = 0; i < m_shaders.size(); ++i) {
        auto current = m_shaders[i].get();
        current->use();
        room->configure(current, i);

    }
}

//void BasicEngineDraw::loadShaders() {
//
//    auto quad_shader = m_shaderBuilders[ShaderType::BATCH_QUAD_PALETTE]();
//    m_shaders.push_back(quad_shader);
//
//    auto line_shader = m_shaderBuilders[ShaderType::BATCH_LINES]();
//    m_shaders.push_back(line_shader);
//
//}




void BasicEngineDraw::draw(Room* room) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (size_t i = 0; i < m_shaders.size(); ++i) {
        auto current = m_shaders[i].get();
        current->use();
        room->draw(current);
    }


}

void FrameBufferEngineDraw::initShaders() {
//    auto quad_shader = m_shaderBuilders[ShaderType::BATCH_QUAD_PALETTE]();
//    m_shaders.push_back(quad_shader);
//
//	auto quad_shader_nopal= m_shaderBuilders[ShaderType::BATCH_QUAD_NO_PALETTE]();
//	m_shaders.push_back(quad_shader_nopal);
//
//    auto line_shader = m_shaderBuilders[ShaderType::BATCH_LINES]();
//    m_shaders.push_back(line_shader);

    m_blitShader = std::make_shared<Shader>(ShaderType::BLIT, blit_vs, blit_fs, "2f2f");

    auto deviceSize = Engine::instance().getDeviceSize();

    float quadVertices[] = {
        // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
    };
    // screen quad VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // create frame buffer
    glGenFramebuffers(1, &_fb);
    glBindFramebuffer(GL_FRAMEBUFFER, _fb);
    glGenTextures(1, &_color);
    glBindTexture(GL_TEXTURE_2D, _color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, deviceSize.x, deviceSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _color, 0);

    // We also want to make sure OpenGL is able to do depth testing (and optionally stencil testing) so we have to make
    // sure to add a depth (and stencil) attachment to the framebuffer. Since we'll only be sampling the color buffer
    // and not the other buffers we can create a renderbuffer object for this purpose.

    // Creating a renderbuffer object isn't too hard. The only thing we have to remember is that we're creating it as a
    // depth and stencil attachment renderbuffer object. We set its internal format to GL_DEPTH24_STENCIL8 which
    // is enough precision for our purposes:

	glGenTextures(1, &_depth);
	glBindTexture(GL_TEXTURE_2D, _depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, deviceSize.x, deviceSize.y, 0,  GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

//    glGenRenderbuffers(1, &_depth);
//    glBindRenderbuffer(GL_RENDERBUFFER, _depth);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, deviceSize.x, deviceSize.y);
//    glBindRenderbuffer(GL_RENDERBUFFER, 0);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depth);
//    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferEngineDraw::draw(Room *room) {

    // restore if you want framebuffer rendering
    glBindFramebuffer(GL_FRAMEBUFFER, _fb);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glAlphaFunc ( GL_GREATER, 0.1 ) ;
//	glEnable ( GL_ALPHA_TEST ) ;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (size_t i = 0; i < m_shaders.size(); ++i) {
        auto current = m_shaders[i].get();
        current->use();
        current->draw();
        //room->draw(current);
    }
    auto vp = Engine::instance().getWindowViewport();
    //std::vector<float> ccc(256*240);
    //glReadPixels(0,0,256,240, GL_DEPTH_COMPONENT, GL_FLOAT, &ccc[0]);
    // frame buffer rendering start
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glViewport(vp.x, vp.y, vp.z, vp.w);
    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_blitShader->use();
    glBindVertexArray(quadVAO);
    m_blitShader->setInt("screenTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _color);	// use the color attachment texture as the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // frame buffer rendering end
//    glfwSwapBuffers(window);
//    glfwPollEvents();
}


void EngineDraw::shutdown() {
    m_shaders.clear();
}