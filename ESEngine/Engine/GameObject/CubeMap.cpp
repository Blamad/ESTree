#include "CubeMap.h"
#include "GraphicEngine/Core/TextureBuffer.h"

CubeMap::CubeMap(std::string path, int* widths, int* heights, std::vector<const unsigned char*>& textures)
{
	this->fileName = path;
	this->type = CUBEMAP;
	this->widths = widths;
	this->heights = heights;
	this->textureBuffer = std::shared_ptr<TextureBuffer>(new TextureBuffer());
	this->textureBuffer->setCubeMapBuffer(textures, widths, heights);
}