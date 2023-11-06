#pragma once

#include <memory>
#include <unordered_map>
#include <functional>
#include "shader.h"
#include "room.h"

class EngineDraw {

public:
    EngineDraw();
    // called when room is loaded
    virtual void init(Room*);
    virtual void initShaders() {  }
    void addShader(int type);
    virtual void draw(Room*) = 0;
    virtual void shutdown() ;
    Shader* getShader(ShaderType type);
protected:

    std::unordered_map<int, std::function<std::shared_ptr<Shader>()>> m_shaderBuilders;
    std::vector<std::shared_ptr<Shader>> m_shaders;
    std::unordered_map<ShaderType, int> _indices;

};

class BasicEngineDraw : public EngineDraw {
public:
    //void loadShaders() override;
    void draw(Room*) override;
};

class FrameBufferEngineDraw : public EngineDraw {
public:
    void initShaders() override;
    void draw(Room*) override;
private:
    std::shared_ptr<Shader> m_blitShader;

    // frambuffer stuff
    GLuint _fb, _color, _depth;
    unsigned int quadVAO, quadVBO;

    bool _useFrameBuffer;
};