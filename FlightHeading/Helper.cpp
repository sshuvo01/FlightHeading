#include "Helper.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <fstream>
#include <sstream>

// Begin- VertexBuffer
VertexBuffer::VertexBuffer(const void* Data, uint Size)
{
	GLCALL(glGenBuffers(1, &RendererID));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, RendererID));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, Size, Data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
	GLCALL(glDeleteBuffers(1, &RendererID));
}

void VertexBuffer::Bind() const
{
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, RendererID));
}

void VertexBuffer::Unbind() const
{
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
// End- VertexBuffer

// Begin- VertexArray
VertexArray::VertexArray()
{
	GLCALL(glGenVertexArrays(1, &RendererID));
	GLCALL(glBindVertexArray(RendererID));
}

VertexArray::~VertexArray()
{
	GLCALL(glDeleteVertexArrays(1, &RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& VB, const VertexBufferLayout& VBL)
{
	Bind();
	VB.Bind();

	auto& Elements = VBL.GetElements();

	for (int i = 0; i < Elements.size(); i++)
	{
		auto& Elm = Elements[i];

		GLCALL(glEnableVertexAttribArray(i));
		GLCALL(glVertexAttribPointer(i, Elm.Count, Elm.Type, Elm.Normalized, VBL.GetStride(), (const void*)Elm.Offset));
	}
}

void VertexArray::Bind() const
{
	GLCALL(glBindVertexArray(RendererID));
}

void VertexArray::Unbind() const
{
	GLCALL(glBindVertexArray(0));
}
// End- VertexArray

// Begin- IndexBuffer
IndexBuffer::IndexBuffer(const uint* Data, uint InCount)
	: Count(InCount)
{
	ASSERT(sizeof(GLuint) == sizeof(uint));

	GLCALL(glGenBuffers(1, &RendererID));
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererID));
	GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, Count * sizeof(uint), Data, GL_STATIC_DRAW));
}

IndexBuffer::IndexBuffer(uint InCount)
	: Count(InCount)
{
	ASSERT(sizeof(GLuint) == sizeof(uint));

	uint* Indices = new uint[Count];

	for (int i = 0; i < Count; i++)
	{
		Indices[i] = i;
	}

	GLCALL(glGenBuffers(1, &RendererID));
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererID));
	GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, Count * sizeof(uint), Indices, GL_STATIC_DRAW));

	delete[] Indices;
}

IndexBuffer::~IndexBuffer()
{
	GLCALL(glDeleteBuffers(1, &RendererID));
}

void IndexBuffer::Bind() const
{
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererID));
}

void IndexBuffer::Unbind() const
{
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
// End- IndexBuffer

// Begin- Texture

Texture::Texture(const std::string& Path, bool FlipUV /*= true*/, bool Gamma/* = true*/, GLenum RepeatMode /*= GL_REPEAT*/)
	: RendererID(0), FilePath(Path), LocalBuffer(nullptr), Width(0), Height(0), BPP(0)
{
	stbi_set_flip_vertically_on_load(FlipUV);
	LocalBuffer = stbi_load(FilePath.c_str(), &Width, &Height, &BPP, 0);

	if (!LocalBuffer)
	{
		ASSERTNOENTRY("STBI_LOAD FAILED.");
		return;
	}

	GLenum Format;
	GLenum InternalFormat;

	if (BPP == 1)
	{
		Format = GL_RED;
		InternalFormat = GL_RED;
	}
	else if (BPP == 3)
	{
		Format = GL_RGB;
		InternalFormat = Gamma ? GL_SRGB : GL_RGB;
	}
	else if (BPP == 4)
	{
		Format = GL_RGBA;
		InternalFormat = Gamma ? GL_SRGB_ALPHA : GL_RGBA;
	}

	GLCALL(glGenTextures(1, &RendererID));
	GLCALL(glBindTexture(GL_TEXTURE_2D, RendererID));
	GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Width, Height, 0, Format, GL_UNSIGNED_BYTE, LocalBuffer));

	GLCALL(glGenerateMipmap(GL_TEXTURE_2D));

	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, RepeatMode));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, RepeatMode));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
}

Texture::~Texture()
{
	GLCALL(glDeleteTextures(1, &RendererID));
	delete LocalBuffer;
}

void Texture::Bind(uint Slot) const
{
	GLCALL(glActiveTexture(GL_TEXTURE0 + Slot));
	GLCALL(glBindTexture(GL_TEXTURE_2D, RendererID));
}

// End- Texture

// Begin- Shader
Shader::Shader(const std::string& VertexPath, const std::string& FragmentPath)
{
	std::string VertexSource = ReadShader(VertexPath);
	std::string FragmentSource = ReadShader(FragmentPath);
	RendererID = CreateShader(VertexSource, FragmentSource);
}

Shader::~Shader()
{
	GLCALL(glDeleteProgram(RendererID));
}

void Shader::Bind() const
{
	GLCALL(glUseProgram(RendererID));
}

void Shader::Unbind() const
{
	GLCALL(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& Name, int Value) const
{
	Bind();
	int Loc = GetUniformLocation(Name);
	if (Loc == -1)
	{
		return;
	}

	GLCALL(glUniform1i(Loc, Value));
}

void Shader::SetUniformTexture(const std::string& Name, int Value) const
{
	SetUniform1i(Name, Value);
}

void Shader::SetUniform1f(const std::string& Name, float Value) const
{
	Bind();
	int Loc = GetUniformLocation(Name);
	if (Loc == -1)
	{
		return;
	}

	GLCALL(glUniform1f(Loc, Value));
}

void Shader::SetUniform2f(const std::string& Name, const glm::vec2& Vec) const
{
	Bind();
	int Loc = GetUniformLocation(Name);
	if (Loc == -1)
	{
		return;
	}

	GLCALL(glUniform2f(Loc, Vec.x, Vec.y));
}

void Shader::SetUniform4f(const std::string& Name, float V0, float V1, float V2, float V3) const
{
	Bind();
	int Loc = GetUniformLocation(Name);
	if (Loc == -1)
	{
		return;
	}

	GLCALL(glUniform4f(Loc, V0, V1, V2, V3));
}

void Shader::SetUniform4f(const std::string& Name, const glm::vec4& Vec) const
{
	Bind();
	int Loc = GetUniformLocation(Name);
	if (Loc == -1)
	{
		return;
	}

	GLCALL(glUniform4f(Loc, Vec[0], Vec[1], Vec[2], Vec[3]));
}

void Shader::SetUniform3f(const std::string& Name, const glm::vec3& Vec) const
{
	Bind();
	int Loc = GetUniformLocation(Name);
	if (Loc == -1)
	{
		return;
	}

	GLCALL(glUniform3f(Loc, Vec[0], Vec[1], Vec[2]));
}

void Shader::SetUniformMatrix4f(const std::string& Name, const glm::mat4& Matrix) const
{
	Bind();
	int Loc = GetUniformLocation(Name);
	if (Loc == -1)
	{
		return;
	}

	GLCALL(glUniformMatrix4fv(Loc, 1, GL_FALSE, &Matrix[0][0]));
}

const std::string Shader::ReadShader(const std::string& Filepath) const
{
	std::ifstream Stream(Filepath);

	if (!Stream.is_open())
	{
		ASSERTNOENTRY("Could not open file!");
		return "";
	}

	std::string Line;
	std::stringstream ReadShader;

	while (getline(Stream, Line))
	{
		ReadShader << Line << '\n';
	}

	return ReadShader.str();
}

int Shader::GetUniformLocation(const std::string& Uniformname) const
{
	GLCALL(int Location = glGetUniformLocation(RendererID, Uniformname.c_str()));

	if (Location == -1)
	{
		ASSERTNOENTRY("Uniform name not found in the shader!");
	}

	return Location;
}

uint Shader::CompileShader(uint Type, const std::string& Source) const
{
	uint Id = glCreateShader(Type);
	const char* Src = Source.c_str();
	GLCALL(glShaderSource(Id, 1, &Src, nullptr));
	GLCALL(glCompileShader(Id));

	int Result;
	GLCALL(glGetShaderiv(Id, GL_COMPILE_STATUS, &Result));
	if (Result == GL_FALSE)
	{
		int Length;
		GLCALL(glGetShaderiv(Id, GL_INFO_LOG_LENGTH, &Length));
		char* Message = (char*)alloca(Length * sizeof(char));

		GLCALL(glGetShaderInfoLog(Id, Length, &Length, Message));

		GLCALL(glDeleteShader(Id));

		std::cout << "Failed to compile shader. Message: " << Message << std::endl;
		ASSERTNOENTRY("Shader compilation error!");
		return 0;
	}

	return Id;
}

uint Shader::CreateShader(const std::string& VertexShader, const std::string& FragmentShader) const
{
	uint Program = glCreateProgram();
	uint VS = CompileShader(GL_VERTEX_SHADER, VertexShader);
	uint FS = CompileShader(GL_FRAGMENT_SHADER, FragmentShader);

	GLCALL(glAttachShader(Program, VS));
	GLCALL(glAttachShader(Program, FS));

	GLCALL(glLinkProgram(Program));
	GLCALL(glValidateProgram(Program));

	GLCALL(glDeleteShader(FS));
	GLCALL(glDeleteShader(VS));

	return Program;
}

// End- Shader
