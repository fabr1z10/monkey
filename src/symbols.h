#pragma once

#include <unordered_map>

enum ShaderType {
	BATCH_QUAD_PALETTE = 0,
	BATCH_QUAD_NO_PALETTE = 1,
	BATCH_LINES = 2,

	COLOR = 10,
	SHADER_SKELETAL = 20,
	BLIT
//    SHADER_COLOR = 0,
//    SHADER_TEXTURE,
//    SHADER_TEXTURE_PALETTE,
//    SHADER_TEXTURE_LIGHT,
//
//    NONE,
//    QUAD_SHADER_PALETTE,
//    QUAD_SHADER,
//    LINE_SHADER
};

extern std::unordered_map<ShaderType, std::string> g_vertexFormat;

enum FillType {
    OUTLINE = 0,
    SOLID
};