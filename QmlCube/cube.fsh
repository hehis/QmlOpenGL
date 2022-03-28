//varying highp vec3 v_Color;
//varying highp vec2 v_texCoord;
//uniform sampler2D textureImg;

//void main( void )
//{
//    gl_FragColor = vec4(v_Color, 1.0);
//    //gl_FragColor = texture2D(textureImg, v_texCoord);
//    //gl_FragColor = texture(textureImg, v_texCoord);
//}


#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    //FragColor = vec4(TexCoord, ourColor.xy);
    //FragColor = vec4(ourColor, 1.0);
    //FragColor = mix(vec4(ourColor, 1.0), texture(ourTexture, TexCoord), 0.2);
    FragColor = texture(ourTexture, TexCoord);
}
