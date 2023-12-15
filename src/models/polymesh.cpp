#include "polymesh.h"
#include "../pyhelper.h"
#include "../assetmanager.h"
#include "../math/earcut.h"
#include "../yamlexp.h"

void PolyMesh::cioa(const std::string& id, std::unordered_map<std::string, glm::vec2>& m, const YAML::Node& node) {
//    if (node["id"]) {
    for (const auto& a : node[id]) {
        auto key = a.first.as<std::string>();
        glm::vec2 value(a.second[0].as<float>() - m_origin.x, -(a.second[1].as<float>() - m_origin.y));
        m[key] = value;
    }
}

// All coordinates in the input file are given in image coorindates ((0, 0) is the top left point)
PolyMesh::PolyMesh(const YAML::Node& node) : DrawableModel(GL_TRIANGLES) {
	m_shaderType = SHADER_SKELETAL;
    using Coord = float;
    using Point = std::array<Coord, 2>;
    using N = uint32_t;

    auto data = node["data"].as<std::vector<float>>();
    auto imgFile = node["tex"].as<std::string>();

    //m_shaderType = ShaderType::SHADER_SKELETAL;
    m_origin = node["origin"].as<glm::vec2>(glm::vec2(0.f));

    // read key points and dimensions
    cioa("key_points", m_keyPoints, node);
    cioa("dims", m_dimensions, node);

    auto& am = AssetManager::instance();
    auto tex = am.getTex(imgFile);    //auto jointId = t.get<int>("joint_id");
    _texId = tex->getTexId();

    std::vector<float> vertices;
    std::vector<Point> polygon;
    // for each mesh, ge wive 3 numbers for each vertex:
    // the first two numbers are the image coordinate of the vertex, the 3rd is the weight
    // attached to parent node (0 = vertex in own)
    for (size_t i = 0; i < data.size(); i += 3) {
        size_t offset = i;
        // transform from image to local coordinates (xl = x - xo, yl = -(y -yo))
        float x = data[offset] - m_origin.x;
        float y = -(data[offset + 1] - m_origin.y);
        float tx = data[offset] / tex->getWidth();
        float ty = data[offset + 1] / tex->getHeight();
        float w0 = 1.f - data[offset + 2];          // weight own
        float w1 = data[offset + 2];                // weight parent
        float w2 = 0.0;
        polygon.push_back({x, y});
        vertices.insert(vertices.end(), {x, y, 0.0f, tx, ty, w0, w1, w2});
    }

    std::vector<std::vector<Point>> p;
    p.push_back(polygon);
    auto tri = mapbox::earcut<N>(p);
    this->initModel(vertices, tri);

}

glm::vec2 PolyMesh::getKeyPoint(const std::string& key) {
    return m_keyPoints.at(key);
}

glm::vec2 PolyMesh::getDimension(const std::string& key) {
    return m_dimensions.at(key);
}

std::shared_ptr<Renderer> PolyMesh::getRenderer(const pybind11::kwargs &) {
	return std::make_shared<BasicRenderer>(SHADER_SKELETAL);
}