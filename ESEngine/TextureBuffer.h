#ifndef TEXTURE_BUFFER_H
#define TEXTURE_BUFFER_H

#include <glad/glad.h>
#include <vector>

enum TextureType;

class TextureBuffer {
public:
	GLuint id;

	void setTextureBuffer(const unsigned char* const texture, const int& width, const int& height, const int& channels, TextureType type);
	void setCubeMapBuffer(std::vector<const unsigned char*>& textures, const int* widths, const int* heights);
};

#endif