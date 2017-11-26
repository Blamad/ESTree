#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>

#include "TextureBuffer.h"

using namespace std;

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
	string fileName;
	shared_ptr<TextureBuffer> textureBuffer;
};

#endif