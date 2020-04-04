#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <memory>
#include <vector>
#include <algorithm>

#include "Engine/Core/Texture2D.h"
#include "Engine/GameObject/CubeMap.h"

class TextureManager {
public:
	static TextureManager& getInstance();
	void operator=(TextureManager const&) = delete;
	TextureManager(TextureManager const&) = delete;
	~TextureManager();

	std::shared_ptr<Texture> createTexture(std::string fileName, TextureType type);
	std::shared_ptr<Texture> createCubeMap(std::string* fileName);

private:
	TextureManager() {};
	std::vector <std::shared_ptr<Texture>> textureBuffers;
};


#endif