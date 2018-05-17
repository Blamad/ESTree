#ifndef MESH_H
#define MESH_H

#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>
#include <boost/foreach.hpp>

#include "ShaderManager.h"
#include "Renderable.h"
#include "Renderer.h"
#include "Transform.h"
#include "Material.h"

#include "TextureBuffer.h"
#include "GLVertexArray.h"

#include "Context.h"
#include "Logger.h"

using namespace std;

class Mesh : public Renderable {
public:
	Material material;
	vector<Vertex> vertices;
	vector<int> indices;

	Mesh(shared_ptr<Shader> shader, int bufferUsage = GL_STATIC_DRAW) : bufferUsage(bufferUsage), Renderable(shader) {};
	Mesh(vector<Vertex> &vertices, vector<int> &indices, shared_ptr<Shader> shader, int vBufferSize = -1, int iBufferSize = -1, int bufferUsage = GL_STATIC_DRAW);
	Mesh(vector<Vertex> &vertices, vector<int> &indices, vector<shared_ptr<Shader>> shaders, int vBufferSize = -1, int iBufferSize = -1, int bufferUsage = GL_STATIC_DRAW);
	~Mesh();

	void draw(Renderer &renderer);
	void draw(Renderer &renderer, Shader *shader);

	void updateMesh();
	void showMeshWiring();
	void showNormalVisualisation();

protected:
	static Logger logger;

	bool initialized = false;
	int bufferUsage = GL_STATIC_DRAW;
	unique_ptr<VertexArray> vertexArray;

	void setupMeshes();
	void setupMesh(shared_ptr<Shader> shader);
	void addNewShader(shared_ptr<Shader> shader);

	int identifier() { return 1; }
};

#endif