#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <vector>

#include "Texture.h"

using namespace std;

struct CubeMap : public Texture
{
	int* widths, *heights;
	//Defines blend strength and blend operation with other textures

	CubeMap(string fileName, int* widths, int* heights, vector<const unsigned char*>& textures);
};

#endif