#ifndef TEXTURE_BUFFER_H
#define TEXTURE_BUFFER_H

#include <GL/glew.h>
#include <vector>

using namespace std;

enum TextureType;

class TextureBuffer {
public:
	GLuint id;

	void setTextureBuffer(const unsigned char* const texture, const int& width, const int& height, const int& channels, TextureType type);
	void setCubeMapBuffer(vector<const unsigned char*>& textures, const int* widths, const int* heights);
};

#endif