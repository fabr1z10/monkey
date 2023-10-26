#pragma once


#include "iquad.h"
//#include "../components/renderer.h"
//
//struct TiledFrameInfo {
//    TiledFrameInfo(int a, int b, int c) : offset(a), count(b), ticks(c) {}
//    unsigned offset;
//    unsigned count;
//    int ticks;
//};
//
class TiledModel : public IQuad {
public:
    TiledModel(const pybind11::kwargs&);

};
//public:
//    TiledModel();
//    explicit TiledModel(const pybind11::kwargs&);
//protected:
//    void readSheet(const pybind11::kwargs&);
//    void readTiles(const std::string& inputString, std::vector<GLfloat>& vertices, std::vector<unsigned>& indices);
//    glm::vec2 m_tileSize;
//    float m_t1, m_t2;
//    unsigned m_i0;
//};
//
//class AnimatedTiledModel : public TiledModel {
//public:
//	explicit AnimatedTiledModel(const pybind11::kwargs&);
//	const TiledFrameInfo& getFrameInfo(int) const;
//	size_t getFrameCount() const;
//	std::shared_ptr<Renderer> getRenderer() const override;
//
//private:
//	std::vector<TiledFrameInfo> m_frameInfo;
//
//};
//
//inline const TiledFrameInfo & AnimatedTiledModel::getFrameInfo(int i) const {
//	return m_frameInfo[i];
//}
//
//inline size_t AnimatedTiledModel::getFrameCount() const {
//	return m_frameInfo.size();
//}