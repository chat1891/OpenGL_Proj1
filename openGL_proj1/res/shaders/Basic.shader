#shader vertex
#version 330 core

layout(location =0) in vec4 position;
layout(location =1) in vec2 texCoord;
layout(location =2) in vec4 vertexColor;

out vec2 v_TexCoord;
out vec4 v_Color;

uniform mat4 u_ModelViewProjectionMatrix;

void main()
{
   gl_Position = u_ModelViewProjectionMatrix * position ;
   v_TexCoord = texCoord;
   v_Color = vertexColor;
};


#shader fragment
#version 330 core

layout(location =0) out vec4 color;

in vec2 v_TexCoord;
in vec4 v_Color;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main()
{
	vec4 texColor = texture(u_Texture, v_TexCoord) * u_Color;
    color = texColor; //r g b a
};