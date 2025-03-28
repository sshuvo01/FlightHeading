#version 330 core

out vec4 fragColor;
in vec2 texCoord;
uniform sampler2D rectTexture;

void main()
{
	vec4 sampledColor = texture(rectTexture, texCoord);
	if(sampledColor.a <= 0.1f)
	{
		discard;
	}

	fragColor = sampledColor;
	//fragColor = vec4(1.f, 0.f, 0.f, 1.f);
}
