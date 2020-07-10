#version 460                                                                     

layout (location = 0) in vec3 pos; 
layout (location = 1) in vec2 tex;

out vec2 TexCoord;

void main()
{          
    gl_Position = vec4(pos, 1.0);
    TexCoord = (pos.xy + vec2(1.0)) / 2.0;;
}