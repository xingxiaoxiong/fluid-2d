#pragma once

#include <GL/glew.h>

#include <glm\glm.hpp>

#include <map>
#include <array>
#include <vector>


class Mesh
{
public:
	Mesh();
	virtual void loadData(const std::vector<float>& positions, const std::vector<float>& texcoords,
		const std::vector<unsigned int>& indices);
	virtual ~Mesh();
	virtual void draw() = 0;

	static const int POS_VB = 0;
	static const int TEXCOORD_VB = 1;
	static const int INDEX_BUFFER = 2;

protected:
	GLuint m_vao;
	std::array<GLuint, 3> m_Buffers;

	unsigned int m_numIndices;
};

class Line : public Mesh {
	void draw();
};

class Quad : public Mesh {
	void draw();
};