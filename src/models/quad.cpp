#include "quad.h"
#include "../pyhelper.h"
#include "../engine.h"
#include "../assetmanager.h"

Quad::Quad(std::shared_ptr<IBatch> batch, const pybind11::kwargs& args) : Model(), _batch(nullptr), _quadCount(0) {


    _batch = dynamic_cast<QuadBatch*>(batch.get());

    auto& am = AssetManager::instance();
    auto sheetFile = _batch->getSheet();
    auto tex = am.getTex(sheetFile);

    float texw = tex->getWidth();
    float texh = tex->getHeight();

    auto py_frames = args["frames"].cast<std::vector<pybind11::dict>>();
    for (const auto& py_frame : py_frames) {
        auto py_quads = py_frame["quads"].cast<std::vector<pybind11::dict>>();
        if (_quadCount == 0) _quadCount = py_quads.size();
        assert(_quadCount == py_quads.size());
        Frame frame;
        frame.ticks = py_get_dict(py_frame, "ticks", 0);
        for (const auto& py_quad : py_quads) {
            //auto qd = quad.cast<pybind11::dict>();
            Desc desc{};
            desc.pos = py_get_dict<glm::vec3>(py_quad, "pos", glm::vec3(0.f));
            desc.size = py_get_dict<glm::vec2>(py_quad, "size");
            desc.repeat = py_get_dict<glm::vec2>(py_quad, "repeat", glm::vec2(1.f, 1.f));
            auto tc = py_get_dict<glm::vec4>(py_quad, "tex_coords");

            desc.textureCoordinates[0] = tc[0] / texw;
            desc.textureCoordinates[1] = (tc[0] + tc[2]) / texw;
            desc.textureCoordinates[2] = tc[1] / texh;
            desc.textureCoordinates[3] = (tc[1] + tc[3]) / texh;
            desc.paletteIndex = py_get_dict<int>(py_quad, "palette", 0);
            frame.quadDesc.push_back(desc);

        }
        _frames.push_back(frame);
    }
}

std::shared_ptr<Renderer> Quad::getRenderer() const {
    return std::make_shared<QuadRenderer>(_batch);

}

QuadRenderer::QuadRenderer(QuadBatch* batch) : Renderer(0, 0), _frame(0), _ticks(0), _spriteBatch(batch) {
    // request a new quad id to the batchcam_node.add(monkey_toolkit.platformer.platform_border(0, 2, 3, 5, qq, 0, platform_type=monkey_toolkit.platformer.PlatformType.LINE))


}

void QuadRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs&  args) {
    _quad = std::dynamic_pointer_cast<Quad>(model);

    auto qc = _quad->getQuadCount();
    for (int i = 0; i< qc; ++i) {
        _quadIds.push_back(_spriteBatch->getPrimitiveId());
    }

}

void QuadRenderer::start() {

}

void QuadRenderer::update(double dt) {
    const auto& a = _quad->getFrame(_frame);

    // get world pos
    auto pos = m_node->getWorldPosition();

    for (size_t i = 0; i < _quadIds.size(); ++i) {
        _spriteBatch->setQuad(_quadIds[i], pos + a.quadDesc[i].pos, a.quadDesc[i].size,
                              a.quadDesc[i].textureCoordinates, a.quadDesc[i].repeat, a.quadDesc[i].paletteIndex, false, false);
    }

    if (a.ticks > 0 && _ticks >= a.ticks) {
        // increment frame. if this animation is
        _frame++;
        if (_frame >= _quad->getFrameCount()) {
            _frame = 0;
        }
        // reset tick counter
        _ticks = 0;
    } else {
        // if it's not time to update frame, increment current frame length
        _ticks++;
    }



}

std::type_index QuadRenderer::getType() {
    return std::type_index(typeid(Renderer));
}
