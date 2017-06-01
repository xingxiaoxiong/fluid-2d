
#include "mesh.h"

Mesh::Mesh() {
	m_numIndices = 0;
	glGenVertexArrays(1, &m_vao);
};

Mesh::~Mesh() {
	glDeleteBuffers(m_Buffers.size(), &m_Buffers[0]);
	glDeleteVertexArrays(1, &m_vao);
}

void Mesh::loadData(const std::vector<float>& positions, const std::vector<float>& texcoords,
	const std::vector<unsigned int>& indices)
{
	glBindVertexArray(m_vao);
	m_numIndices = indices.size();

	glGenBuffers(m_Buffers.size(), &m_Buffers[0]);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), &positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POS_VB);
	glVertexAttribPointer(POS_VB, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texcoords.size(), &texcoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEXCOORD_VB);
	glVertexAttribPointer(TEXCOORD_VB, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_numIndices, &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Line::draw()
{
	glBindVertexArray(m_vao);
	glDrawElements(GL_LINE_STRIP, m_numIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Quad::draw()
{
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}