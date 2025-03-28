#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>

typedef unsigned int uint;
typedef unsigned char uchar;

template<typename T>
using CRef = const T&;

#ifdef _DEBUG
#define ASSERT(x) if(!(x)) __debugbreak()
#else
#define ASSERT(x)
#endif // _DEBUG

#define ASSERTNOENTRY(Msg) ASSERT(false && Msg)

#ifdef _DEBUG
#define GLCALL(x) Useful::GLClearError();x;ASSERT(Useful::GLLogCall(#x, __FILE__, __LINE__))
#else
#define GLCALL(x) x;
#endif // _DEBUG

namespace Useful
{
	constexpr static float PI = 3.1415926535f;

	class NonCopyable
	{
	public:
		NonCopyable() = default;
		NonCopyable(const NonCopyable&) = delete;
		NonCopyable& operator=(const NonCopyable&) = delete;
		virtual ~NonCopyable() {}
	};

	inline void GLClearError()
	{
		while (glGetError() != GL_NO_ERROR)
		{

		}
	}

	inline bool GLLogCall(const char* function, const char* file, int line)
	{
		while (GLenum error = glGetError())
		{
			std::cout << "OpenGL failed! info: " << error << " " << function << " " << line << std::endl;
			return false;
		}
		return true;
	}
}