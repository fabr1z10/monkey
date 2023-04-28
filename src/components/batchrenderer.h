//#pragma once
//
//
//#include "renderer.h"
//
//
//template<typename B, typename M>
//class BatchRenderer : public Renderer {
//public:
//    explicit BatchRenderer(B* b) : Renderer(0, 0), _batch(b) {
//
//    }
//
//    void setModel(std::shared_ptr<Model> model) override {
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
//private:
//    B* _batch;
//    std::shared_ptr<M> _m;
//    std::vector<int> _primitiveIds;
//};
//
