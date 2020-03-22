#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>

#include "TextureBuffer.h"

enum TextureType {
	DIFFUSE,
	SPECULAR,
	AMBIENT,
	CUBEMAP,
	BUFFER,
	DEPTH_BUFFER,
	FLOATING_POINT_COLOR_BUFFER
};

class Texture {
public:
	TextureType type;
	std::string fileName;
	std::shared_ptr<TextureBuffer> textureBuffer;
};

#endif