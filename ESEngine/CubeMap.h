#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <vector>

#include "Texture.h"

struct CubeMap : public Texture
{
	int* widths, *heights;
	CubeMap(std::string fileName, int* widths, int* heights, std::vector<const unsigned char*>& textures);
};

#endif