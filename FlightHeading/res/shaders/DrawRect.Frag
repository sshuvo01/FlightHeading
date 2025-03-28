#version 330 core

out vec4 fragColor;
in vec2 texCoord;
uniform sampler2D rectTexture;

void main()
{
	float discardThreshold = 0.8f;
	vec4 sampledColor = texture(rectTexture, texCoord);
	if(sampledColor.a <= discardThreshold)
	{
		discard;
	}

	fragColor = sampledColor;
}
