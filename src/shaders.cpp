#include "shaders.hpp"

// TODO: rerender mapping sprites to ensure they dont have artifacts on the edge
// where they sample random pixels
// (didnt know where to put this i dont have a todo file :broken_heart:)

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

void main() {
    vec3 mapping = texture2D(CC_Texture1, v_texCoord).rgb;

    if (mapping.b != 0.0) {
        // edge pixel
        
    }

    // if (mapping.b == 0.0) discard;

    vec4 color = texture2D(CC_Texture0, mapping.rg);
    gl_FragColor = v_fragmentColor * color;
    gl_FragColor.a *= mapping.b;

    // gl_FragColor = vec4(mapping, 1.0);
}
)";
