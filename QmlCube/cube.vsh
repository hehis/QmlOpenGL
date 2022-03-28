
//in vec3 aPos;
//in vec3 aColor;
//in vec2 aTexCoord;

//uniform mat4 modelViewMatrix;
//uniform mat4 projectionMatrix;

//varying highp vec3 v_Color;
//varying highp vec2 v_texCoord;
//void main( void )
//{
//    gl_Position = projectionMatrix *
//            modelViewMatrix *
//            vec4(aPos, 1.0 );
//    v_texCoord = aTexCoord;
//    v_Color = aColor;
//}


#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = projectionMatrix *
            modelViewMatrix *
            vec4(aPos, 1.0 );
    TexCoord = aTexCoord;
    ourColor = aColor;

}
