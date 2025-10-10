#include "shaders.hpp"

const GLchar* g_mappingShaderVertex = R"(
attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec4 a_color;

#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

void main() {
    gl_Position = CC_MVPMatrix * a_position;
    v_fragmentColor = a_color;
    v_texCoord = a_texCoord;
}
)";

const GLchar* g_mappingShaderFragment = R"(
#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
uniform sampler2D CC_Texture0; // icon data
uniform sampler2D CC_Texture1; // mapping
uniform sampler2D CC_Texture2; // transparency

void main() {
    float transparency = texture2D(CC_Texture2, v_texCoord).r;
    if (transparency != 1.0) discard; // maybe think of a better solution?

    vec3 mapping = texture2D(CC_Texture1, v_texCoord).rgb;

    vec4 color = texture2D(CC_Texture0, mapping.rg);
    gl_FragColor = v_fragmentColor * color;
    gl_FragColor.a *= transparency;
}
)";
