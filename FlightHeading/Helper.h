#pragma once
#include "Core.h"
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>

class VertexBuffer : public Useful::NonCopyable
{
public:
	VertexBuffer(const void* Data, uint Size);
	~VertexBuffer();
	VertexBuffer() = delete;

	void Bind() const;
	void Unbind() const;

private:
	uint RendererID;
};

struct VertexBufferElement
{
	uint Type;
	uint Count;
	uint Offset;
	GLboolean Normalized;

	static uint GetTypeSize(uint InType)
	{
		switch (InType)
		{
		case GL_FLOAT:
			return sizeof(GLfloat);
		case GL_UNSIGNED_INT:
			return sizeof(GLuint);
		case GL_UNSIGNED_BYTE:
			return sizeof(GLubyte);
		default:
			ASSERTNOENTRY("This should not execute!");
			break;
		}
		return 0;
	}
};

class VertexBufferLayout
{
public:
	VertexBufferLayout() : Stride(0) {}

	void Push(uint Count)
	{
		Elements.push_back({ GL_FLOAT, Count, Stride, GL_FALSE });
		Stride += (Count * VertexBufferElement::GetTypeSize(GL_FLOAT));
	}

	inline const std::vector<VertexBufferElement>& GetElements() const { return Elements; }
	inline uint GetStride() const { return Stride; }

private:
	std::vector<VertexBufferElement> Elements;
	uint Stride;
};

class VertexArray : public Useful::NonCopyable
{
public:

	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& VB, const VertexBufferLayout& VBL);
	void Bind() const;
	void Unbind() const;

private:
	uint RendererID;
};

class IndexBuffer : public Useful::NonCopyable
{
public:
	IndexBuffer(const uint* Data, uint InCount);
	IndexBuffer(uint InCount);
	IndexBuffer() = delete;
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;
	inline uint GetCount() const { return Count; }

private:
	uint RendererID;
	uint Count;
};

class Texture : public Useful::NonCopyable
{
public:
	Texture() = delete;
	Texture(const std::string& Path, bool FlipUV = true, bool Gamma = true, GLenum RepeatMode = GL_REPEAT);
	~Texture();

	void Bind(uint Slot = 0) const;

	inline int GetWidth() const { return Width; }
	inline int GetHeight() const { return Height; }
	inline std::string GetFilePath() const { return FilePath; }

	inline uint GetID() const { return RendererID; }
private:
	uint RendererID;
	std::string FilePath;
	uchar* LocalBuffer;
	int Width, Height, BPP;
};

class Shader : public Useful::NonCopyable
{
public:
	Shader() = delete;
	Shader(const std::string& VertexPath, const std::string& FragmentPath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void SetUniform1i(const std::string& Name, int Value) const;
	void SetUniformMatrix4f(const std::string& Name, const glm::mat4& Matrix) const;
	
private:
	uint RendererID;
	const std::string ReadShader(const std::string& Filepath) const;
	uint CompileShader(uint Type, const std::string& Source) const;
	uint CreateShader(const std::string& VertexShader, const std::string& FragmentShader) const;
	int GetUniformLocation(const std::string& Uniformname) const;
};