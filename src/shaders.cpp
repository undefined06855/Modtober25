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
uniform sampler2D CC_Texture3; // mapping 2
uniform sampler2D CC_Texture4; // transparency 2

uniform float u_weight;

void main() {
    float transparency1 = texture2D(CC_Texture2, v_texCoord).r;
    float transparency2 = texture2D(CC_Texture4, v_texCoord).r;
    float transparency = ((transparency1 * u_weight) + (transparency2 * (1.0 - u_weight))) / 2.0;
    // if (transparency != 1.0) discard; // maybe think of a better solution?

    vec3 mapping1 = texture2D(CC_Texture1, v_texCoord).rgb;
    vec3 mapping2 = texture2D(CC_Texture3, v_texCoord).rgb;
    vec3 mapping = ((mapping1 * u_weight) + (mapping2 * (1.0 - u_weight))) / 2.0;

    vec4 color = texture2D(CC_Texture0, mapping.rg);
    gl_FragColor = v_fragmentColor * color;
    gl_FragColor.a *= transparency;
}
)";
