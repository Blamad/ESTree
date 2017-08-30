#include "CubeMap.h"
#include "TextureBuffer.h"

CubeMap::CubeMap(string path, int* widths, int* heights, vector<const unsigned char*>& textures)
{
	this->fileName = path;
	this->type = CUBEMAP;
	this->widths = widths;
	this->heights = heights;
	this->textureBuffer = shared_ptr<TextureBuffer>(new TextureBuffer());
	this->textureBuffer->setCubeMapBuffer(textures, widths, heights);
}