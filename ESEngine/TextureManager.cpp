#include "TextureManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include <STB_IMAGE\stb_image.h>
#include <algorithm>

TextureManager& TextureManager::getInstance() {
	static TextureManager instance;
	return instance;
}

shared_ptr<Texture> TextureManager::createTexture(string fileName, TextureType type) {
	auto it = find_if(textureBuffers.begin(), textureBuffers.end(), [&fileName](const shared_ptr<Texture>& obj) {return obj->fileName == fileName; });

	if (it != textureBuffers.end()) {
		auto index = std::distance(textureBuffers.begin(), it);
		return textureBuffers[index];
	}
	
	int width, height, nrChannels;
	unsigned char *data = stbi_load(fileName.c_str(), &width, &height, &nrChannels, 0);

	shared_ptr<Texture> texturePtr = make_shared<Texture2D>(fileName, width, height, nrChannels, data, type);
	textureBuffers.push_back(texturePtr);
	stbi_image_free((void*)data);
	return texturePtr;
}

shared_ptr<Texture> TextureManager::createCubeMap(string* fileNames) {
	string fileName = fileNames[0];

	auto it = find_if(textureBuffers.begin(), textureBuffers.end(), [&fileName](const shared_ptr<Texture>& obj) {return obj->fileName == fileName; });

	if (it != textureBuffers.end()) {
		auto index = std::distance(textureBuffers.begin(), it);
		return textureBuffers[index];
	}

	int widths[6], heights[6], nrChannels[6];
	vector<const unsigned char*> tempImages;
	for (int i = 0; i < 6; i++) {
		unsigned char* data = stbi_load(fileNames[i].c_str(), &widths[i], &heights[i], &nrChannels[i], 0);
		tempImages.push_back(data);
	}

	shared_ptr<Texture> texturePtr = make_shared<CubeMap>(fileName, widths, heights, tempImages);
	textureBuffers.push_back(texturePtr);
	
	for (int i = 0; i < 6; i++) {
		stbi_image_free((void*)tempImages[i]);
	}

	return texturePtr;
}

TextureManager::~TextureManager() {
	
}
