#version 330 core

#define MAX_POINT_LIGHTS 16
#define MAX_SPOT_LIGHTS 16
#define MAX_DIRECTIONAL_LIGHTS 16

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emissive;
    float shininess;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float ke;
    float kl;
    float kq;
};

struct DirectionalLight {
    vec3 color;
    vec3 direction;
};

struct SpotLight {
    vec3 position;
    vec3 color;
    vec3 direction;
    float ke;
    float kl;
    float kq;
    float p;
};

struct LightOutput {
    vec3 L;
    vec3 V;
    vec3 H;
    vec3 color;
};

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D ourTexture;
uniform bool useTexture;

uniform int filterMode;
uniform vec2 texelSize;
uniform int toneMappingMode;
uniform float toneExposure;
const vec3 LUMA_WEIGHTS = vec3(0.2126, 0.7152, 0.0722);

uniform int lightingModel;
uniform vec3 viewPosition;
uniform vec3 ambientLightColor;
uniform Material objectMaterial;

uniform int NB_DIR_LIGHTS;
uniform int NB_POINT_LIGHTS;
uniform int NB_SPOT_LIGHTS;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform DirectionalLight directionalLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_POINT_LIGHTS];

vec3 applyKernel(const float kernel[9])
{
    vec3 accum = vec3(0.0);
    vec2 offsets[9] = vec2[](
        vec2(-1.0, -1.0), vec2(0.0, -1.0), vec2(1.0, -1.0),
        vec2(-1.0,  0.0), vec2(0.0,  0.0), vec2(1.0,  0.0),
        vec2(-1.0,  1.0), vec2(0.0,  1.0), vec2(1.0,  1.0)
    );

    for (int i = 0; i < 9; ++i) {
        vec2 sampleOffset = offsets[i] * texelSize;
        accum += kernel[i] * texture(ourTexture, TexCoord + sampleOffset).rgb;
    }
    return accum;
}

vec3 applyFilter(vec3 baseColor)
{
    if (!useTexture || filterMode == 0) {
        return baseColor;
    }

    if (filterMode == 1) {
        float luminance = dot(baseColor, LUMA_WEIGHTS);
        return vec3(luminance);
    }

    if (texelSize.x <= 0.0 || texelSize.y <= 0.0) {
        return baseColor;
    }

    if (filterMode == 2) {
        const float sharpenKernel[9] = float[](
            0.0, -1.0, 0.0,
           -1.0,  5.0, -1.0,
            0.0, -1.0, 0.0
        );
        return clamp(applyKernel(sharpenKernel), 0.0, 1.0);
    }

    if (filterMode == 3) {
        const float edgeKernel[9] = float[](
           -1.0, -1.0, -1.0,
           -1.0,  8.0, -1.0,
           -1.0, -1.0, -1.0
        );
        return clamp(applyKernel(edgeKernel), 0.0, 1.0);
    }

    if (filterMode == 4) {
        const float blurKernel[9] = float[](
            1.0/9.0, 1.0/9.0, 1.0/9.0,
            1.0/9.0, 1.0/9.0, 1.0/9.0,
            1.0/9.0, 1.0/9.0, 1.0/9.0
        );
        return clamp(applyKernel(blurKernel), 0.0, 1.0);
    }

    return baseColor;
}

vec3 applyToneMapping(vec3 color)
{
    vec3 mapped = color * toneExposure;

    if (toneMappingMode == 1) { // Reinhard
        mapped = mapped / (mapped + vec3(1.0));
        mapped = pow(mapped, vec3(1.0 / 2.2));
    } else if (toneMappingMode == 2) { // ACES
        const float A = 2.51;
        const float B = 0.03;
        const float C = 2.43;
        const float D = 0.59;
        const float E = 0.14;
        mapped = clamp(
            (mapped * (A * mapped + B)) / (mapped * (C * mapped + D) + E),
            0.0, 1.0);
        mapped = pow(mapped, vec3(1.0 / 2.2));
    }

    return mapped;
}

LightOutput prepareDirLight(DirectionalLight L)
{
    LightOutput Loutput;

    Loutput.L = normalize(-L.direction);
    Loutput.V = normalize(viewPosition - FragPos);
    Loutput.color = L.color;
    Loutput.H = normalize(Loutput.L + Loutput.V);
    return Loutput;
}

LightOutput preparePointLight(PointLight L)
{
    LightOutput Loutput;

    vec3 lightV = L.position - FragPos;
    float dist = length(lightV);
    float attenuation = 1.0 / (L.ke + L.kl * dist + L.kq * (dist * dist));

    Loutput.L = normalize(lightV);
    Loutput.V = normalize(viewPosition - FragPos);
    Loutput.color = L.color * attenuation;
    Loutput.H = normalize(Loutput.L + Loutput.V);

    return Loutput;
}

vec3 modelLambert(vec3 N, LightOutput LOutput)
{
    float diffuse = max(dot(N, LOutput.L), 0.0);
    return LOutput.color * diffuse;
}

vec3 calculateDirectLight(vec3 N, LightOutput LOutput, vec3 diffuseTextureColor)
{
    vec3 lightReflectedColor = vec3(0.0);

    if (lightingModel == 0) {
        lightReflectedColor = modelLambert(N, LOutput);
    }

    if (lightingModel == 0) {
        return lightReflectedColor * (objectMaterial.diffuse * diffuseTextureColor);
    }

    return vec3(0.0);
}

void main()
{
    vec3 norm = normalize(Normal);

    vec4 sampledColor
        = useTexture ? texture(ourTexture, TexCoord) : vec4(1.0, 1.0, 1.0, 1.0);
    vec3 diffuseTextureColor = applyFilter(sampledColor.rgb);

    vec3 ambient = ambientLightColor * objectMaterial.ambient * diffuseTextureColor;
    vec3 emissive = objectMaterial.emissive;
    vec3 totalDirectLight = vec3(0.0);

    LightOutput LOutput;
    for(int i = 0; i < NB_DIR_LIGHTS; i++) {
        LOutput = prepareDirLight(directionalLights[i]);
        totalDirectLight += calculateDirectLight(norm, LOutput, diffuseTextureColor);
    }

    for(int i = 0; i < NB_POINT_LIGHTS; i++) {
        LOutput = preparePointLight(pointLights[i]);
        totalDirectLight += calculateDirectLight(norm, LOutput, diffuseTextureColor);
    }

    vec3 shaded = ambient + emissive + totalDirectLight;

    vec3 toneMapped = applyToneMapping(shaded);
    vec3 finalColor = (toneMappingMode == 0) ? shaded : toneMapped;
    FragColor = vec4(finalColor, 1.0);
}