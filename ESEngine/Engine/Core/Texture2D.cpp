#include "Texture2D.h"

Texture2D::Texture2D(std::string fileName, int width, int height, int channels, const unsigned char* const texture, TextureType type)
{
	this->width = width;
	this->height = height;
	this->type = type;
	this->fileName = fileName;
	this->channels = channels;
	this->textureBuffer = std::shared_ptr<TextureBuffer>(new TextureBuffer());
	this->textureBuffer->setTextureBuffer(texture, width, height, channels, type);
}