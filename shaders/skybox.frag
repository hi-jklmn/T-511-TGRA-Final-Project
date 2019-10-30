#version 330 core
out vec4 FragColor;

uniform sampler2D uTextureDiffuse1;
uniform sampler2D uTextureSpecular1;
uniform sampler2D uTextureNormal1;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

uniform vec4 uEyePosition;

in float height;

void main()
{
  float y = height + 0.5;
  vec4 color1 = vec4(0.0, 0.1, 0.4, 1.0);
  vec4 color2 = vec4(0.2, 0.0, 0.0, 1.0);
  gl_FragColor = mix(color2, color1, clamp(y, 0.2, 0.8));
}
