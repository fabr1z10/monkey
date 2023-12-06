#pragma once


#include "iquad.h"
#include "../components/batchrenderer.h"
//
//struct TiledFrameInfo {
//    TiledFrameInfo(int a, int b, int c) : offset(a), count(b), ticks(c) {}
//    unsigned offset;
//    unsigned count;
//    int ticks;
//};
//
class TiledModel : public Model {
public:
    explicit TiledModel(const pybind11::kwargs&);
    int getQuadCount() const;
protected:
	std::string ltrim(const std::string &s);
	int readFrame(const pybind11::handle&, std::vector<Desc>& desc);
	float _tsxNorm;
	float _tsyNorm;
	glm::vec2 _tileSize;
	int _quadCount;
};

inline int TiledModel::getQuadCount() const {
	return _quadCount;
}

/// this is reserved for static data that doesn't move or animate :-)
class StaticTiledModel : public TiledModel {
public:
	explicit StaticTiledModel(const pybind11::kwargs&);
	std::shared_ptr<Renderer> getRenderer(const pybind11::kwargs&) override;
	const std::vector<Desc>& getQuads() const;
private:
	std::vector<Desc> _quads;
};

inline const std::vector<Desc> & StaticTiledModel::getQuads() const {
	return _quads;
}





class AnimatedTiledModel : public TiledModel {
public:
	explicit AnimatedTiledModel(const pybind11::kwargs&);
	std::shared_ptr<Renderer> getRenderer(const pybind11::kwargs&) override;
	struct Frame {
		std::vector<Desc> _quads;
		int ticks;
	};
	int getFrameCount() const;
	const AnimatedTiledModel::Frame& getFrame(int frame) const;
private:


	std::vector<Frame> _frames;
};


inline int AnimatedTiledModel::getFrameCount() const {
	return _frames.size();
}
inline const AnimatedTiledModel::Frame& AnimatedTiledModel::getFrame(int frame) const {
	return _frames[frame];
}


class StaticTiledModelRenderer : public BatchRenderer<QuadBatch>  {
public:
	explicit StaticTiledModelRenderer(const pybind11::kwargs& args);
	void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args) override;

};


class AnimatedTiledModelRenderer : public BatchRenderer<QuadBatch>  {
public:
	explicit AnimatedTiledModelRenderer(const pybind11::kwargs& args);
	void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args) override;
	void update(double) override;

private:
	AnimatedTiledModel * _model;
	int _frame;
	int _ticks;
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