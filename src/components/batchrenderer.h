#pragma once


#include "renderer.h"
#include "../room.h"
#include "../engine.h"


template<typename BATCH>
class BatchRenderer : public Renderer {
public:
	BatchRenderer(const std::string& batchId, const pybind11::kwargs& args) : Renderer(args), _batchId(batchId) {
		//assert(args["batch"]);

		_batch = dynamic_cast<BATCH*>(Engine::instance().getRoom()->getBatch(_batchId));
		assert(_batch != nullptr);
	}

	~BatchRenderer() {
		for (const auto& primitiveId : _primitiveIds) {
			_batch->release(primitiveId);
		}
	}

	void setState(NodeState value) override {
		Component::setState(value);
		for (const auto& quad : _primitiveIds) {
			_batch->setVisible(quad, value != NodeState::INACTIVE);
		}
	}

    void setPalette(const std::string& palId) override {
        _paletteId =  _batch->getPalette(palId);
        std::fill(_palettes.begin(), _palettes.end(), _paletteId);
	}

	void setPrimitivePalette(int id, const std::string& palId) {
        _paletteId =  _batch->getPalette(palId);
        _palettes[id] = _paletteId;
	}

    void setPrimitivePalette(int id, int palId) {
        _palettes[id] = palId;
    }

    int getPrimitivePalette(int id) {
	    return _palettes[id];
	}



    //    explicit BatchRenderer(B* b) : Renderer(0, 0), _batch(b) {
    //
    //    }
//
//    void setNodeModel(std::shared_ptr<Model> model) override {
//        _m = std::dynamic_pointer_cast<M>(model);
//        auto qc = _m->getPrimitiveCount();
//        for (int i = 0; i< qc; ++i) {
//            _primitiveIds.push_back(_batch->getPrimitiveId());
//        }
//    }
//
//
//
//
protected:
	std::vector<int> _primitiveIds{};
	std::vector<int> _palettes{};

	BATCH* _batch;
	std::string _batchId;
//    std::shared_ptr<M> _m;
//    std::vector<int> _primitiveIds;
};


