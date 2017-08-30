#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <memory>
#include <vector>

#include "Texture.h"

using namespace std;

class TextureManager {
public:
	static TextureManager& getInstance();
	void operator=(TextureManager const&) = delete;
	TextureManager(TextureManager const&) = delete;
	~TextureManager();

	shared_ptr<Texture> createTexture(string fileName, TextureType type);
	shared_ptr<Texture> createCubeMap(string* fileName);

private:
	TextureManager() {};
	vector <shared_ptr<Texture>> textureBuffers;
};


#endif