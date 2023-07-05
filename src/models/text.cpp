#include "text.h"
#include "../pyhelper.h"
#include "../util.h"
#include "../font.h"
#include "../assetmanager.h"
#include <iostream>

Text::Text(const pybind11::kwargs & args) : IQuad() {

    auto text = py_get_dict<std::string>(args, "text");
    auto s32 = getString32(text);

    auto fontId = py_get_dict<std::string>(args, "font");
    auto font = AssetManager::instance().getFont(fontId);

    Animation anim;
    Frame frame;
    _quadCount = 0;

    glm::vec2 position(0.f, 0.f);
    for (const auto& c : s32) {
        //font->get
        // one quad per character
        Desc quad;
        const auto& charInfo = font->getCharInfo(c);
        std::cout << "ciao " << charInfo.advance << "\n";

        quad.textureCoordinates = glm::vec4(charInfo.tx, charInfo.tx + charInfo.tw, charInfo.ty, charInfo.ty + charInfo.th);
        quad.size = glm::vec2(charInfo.w, charInfo.h);
        quad.anchorPoint = position;
        position.x -= charInfo.advance;
        quad.repeat = glm::vec2(1, 1);
        frame.quads.push_back(quad);
        _quadCount++;
    }
    anim.frames.push_back(frame);
	_defaultAnimation = "main";
	_animations["main"] = anim;

}
