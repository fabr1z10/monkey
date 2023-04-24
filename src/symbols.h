#pragma once


enum ShaderType {
    SHADER_COLOR = 0,
    SHADER_TEXTURE,
    SHADER_TEXTURE_PALETTE,
    SHADER_TEXTURE_LIGHT,
	SHADER_SKELETAL,
    NONE,
    QUAD_SHADER
};

enum FillType {
    OUTLINE = 0,
    SOLID
};