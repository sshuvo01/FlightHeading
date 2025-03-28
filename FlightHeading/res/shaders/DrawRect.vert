#version 330 core
layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoord;

out vec2 texCoord;
uniform mat4 modelMat;

void main()
{
	texCoord = aTexCoord;
	gl_Position = modelMat * vec4(aPosition, 0.f, 1.f);
}