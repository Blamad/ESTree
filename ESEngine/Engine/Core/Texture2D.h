#ifndef TEXTURE_2D_H
#define TEXTURE_2D_H

#include "Texture.h"

struct Texture2D : public Texture
{
	int width, height, channels;
	Texture2D(std::string path, int width, int height, int channels, const unsigned char* const texture, TextureType type);
};

#endif