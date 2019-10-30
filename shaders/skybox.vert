#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
layout (location = 4) in vec2 aTexCoords;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out float height;

void main(void) {

     mat4 view = mat4(mat3(uViewMatrix));
     vec4 position = uProjectionMatrix * view * vec4(aPos, 1.0);

     height = aPos.y;

     gl_Position = position.xyww;
}

