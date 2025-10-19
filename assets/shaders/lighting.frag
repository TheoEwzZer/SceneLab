#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D ourTexture;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform bool useTexture;
uniform vec3 objectColor;

void main() {
    float ambientStrength = 0.1;

    vec4 baseColor = useTexture ? texture(ourTexture, TexCoord) : vec4(objectColor, 1.0);
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * baseColor.xyz;
    FragColor = vec4(result, 1.0);
}
