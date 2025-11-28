#version 330 core
out vec4 FragColor;
in vec3 FragPos;
uniform vec3 viewPos;
uniform vec3 viewRotation; // pitch, yaw, roll in radians
uniform float aspectRatio; // width / height
uniform float fov;
uniform int iFrame;
uniform sampler2D previousFrame; // Previous frame's accumulated color
uniform sampler2D trianglesTex;
uniform int numTriangles;

struct SMaterialInfo
{
    vec3 albedo;
    vec3 emissive;
    float percentSpecular;
    float roughness;
    vec3 specularColor;
};

struct SRayHitInfo
{
    float dist;
    vec3 normal;
    SMaterialInfo material;
};

struct Triangle {
    vec3 v0;
    vec3 v1;
    vec3 v2;
    vec3 color;
    vec3 emissive;
    float percentSpecular;
    float roughness;
    vec3 specularColor;
};

Triangle loadTriangle(int triIndex)
{
    Triangle t;

    // Layout: width=5 pixels per row
    // Pixel 0: [v0.xyz, v1.x]
    vec4 p0 = texelFetch(trianglesTex, ivec2(0, triIndex), 0);
    t.v0 = p0.xyz;

    // Pixel 1: [v1.yz, v2.xy]
    vec4 p1 = texelFetch(trianglesTex, ivec2(1, triIndex), 0);
    t.v1 = vec3(p0.w, p1.xy);

    // Pixel 2: [v2.z, color.xyz]
    vec4 p2 = texelFetch(trianglesTex, ivec2(2, triIndex), 0);
    t.v2 = vec3(p1.zw, p2.x);
    t.color = p2.yzw;

    // Pixel 3: [emissive.xyz, percentSpecular]
    vec4 p3 = texelFetch(trianglesTex, ivec2(3, triIndex), 0);
    t.emissive = p3.xyz;
    t.percentSpecular = p3.w;

    // Pixel 4: [roughness, specularColor.xyz]
    vec4 p4 = texelFetch(trianglesTex, ivec2(4, triIndex), 0);
    t.roughness = p4.x;
    t.specularColor = p4.yzw;

    return t;
}

const float c_epsilon = 0.0001f;
const float c_pi = 3.14159265359f;
const float c_twopi = 2.0f * c_pi;
const float c_rayPosNormalNudge = 0.01f;
const int c_numBounces = 10;
const float c_superFar = 10000.0f;
const float c_minimumRayHitTime = 0.1f;
const int c_numRendersPerFrame = 1;

float ScalarTriple(vec3 u, vec3 v, vec3 w)
{
    return dot(cross(u, v), w);
}

bool TestQuadTrace(in vec3 rayPos, in vec3 rayDir, inout SRayHitInfo info, in vec3 a, in vec3 b, in vec3 c, in vec3 d)
{
    // calculate normal and flip vertices order if needed
    vec3 normal = normalize(cross(c-a, c-b));
    if (dot(normal, rayDir) > 0.0f)
    {
        normal *= -1.0f;

		vec3 temp = d;
        d = a;
        a = temp;

        temp = b;
        b = c;
        c = temp;
    }

    vec3 p = rayPos;
    vec3 q = rayPos + rayDir;
    vec3 pq = q - p;
    vec3 pa = a - p;
    vec3 pb = b - p;
    vec3 pc = c - p;

    // determine which triangle to test against by testing against diagonal first
    vec3 m = cross(pc, pq);
    float v = dot(pa, m);
    vec3 intersectPos;
    if (v >= 0.0f)
    {
        // test against triangle a,b,c
        float u = -dot(pb, m);
        if (u < 0.0f) return false;
        float w = ScalarTriple(pq, pb, pa);
        if (w < 0.0f) return false;
        float denom = 1.0f / (u+v+w);
        u*=denom;
        v*=denom;
        w*=denom;
        intersectPos = u*a+v*b+w*c;
    }
    else
    {
        vec3 pd = d - p;
        float u = dot(pd, m);
        if (u < 0.0f) return false;
        float w = ScalarTriple(pq, pa, pd);
        if (w < 0.0f) return false;
        v = -v;
        float denom = 1.0f / (u+v+w);
        u*=denom;
        v*=denom;
        w*=denom;
        intersectPos = u*a+v*d+w*c;
    }

    float dist;
    if (abs(rayDir.x) > 0.1f)
    {
        dist = (intersectPos.x - rayPos.x) / rayDir.x;
    }
    else if (abs(rayDir.y) > 0.1f)
    {
        dist = (intersectPos.y - rayPos.y) / rayDir.y;
    }
    else
    {
        dist = (intersectPos.z - rayPos.z) / rayDir.z;
    }

	if (dist > c_minimumRayHitTime && dist < info.dist)
    {
        info.dist = dist;
        info.normal = normal;
        return true;
    }

    return false;
}

bool TestTriangleTrace(in vec3 rayPos, in vec3 rayDir, inout SRayHitInfo info, in vec3 a, in vec3 b, in vec3 c)
{
    float hit;
    vec3 barycentricCoord;
    vec3 triangleNormal;

    vec3 e0 = b - a;
    vec3 e1 = a - c;
    triangleNormal = cross(e1 , e0);

    float valueDot = 1.0 / dot( triangleNormal, rayDir );

    vec3 e2 = ( valueDot ) * ( a - rayPos );
    vec3 i  = cross(rayDir , e2);

    barycentricCoord.y = dot( i, e1 );
    barycentricCoord.z = dot( i, e0 );
    barycentricCoord.x = 1.0 - (barycentricCoord.z + barycentricCoord.y);
    hit = dot( triangleNormal, e2 );

    bool hitTest = (hit > c_epsilon) && (barycentricCoord.x > 0 && barycentricCoord.y >0 && barycentricCoord.z > 0);

    if (hitTest)
    {
          if (hit > c_minimumRayHitTime && hit < info.dist)
        {
            info.dist = hit;
            info.normal = normalize(triangleNormal);
            return true;
        }
    }

    return false;
}

bool TestSphereTrace(in vec3 rayPos, in vec3 rayDir, inout SRayHitInfo info, in vec4 sphere)
{
	//get the vector from the center of this sphere to where the ray begins.
	vec3 m = rayPos - sphere.xyz;

    //get the dot product of the above vector and the ray's vector
	float b = dot(m, rayDir);

	float c = dot(m, m) - sphere.w * sphere.w;

	//exit if r's origin outside s (c > 0) and r pointing away from s (b > 0)
	if(c > 0.0 && b > 0.0)
		return false;

	//calculate discriminant
	float discr = b * b - c;

	//a negative discriminant corresponds to ray missing sphere
	if(discr < 0.0)
		return false;

	//ray now found to intersect sphere, compute smallest t value of intersection
    bool fromInside = false;
	float dist = -b - sqrt(discr);
    if (dist < 0.0f)
    {
        fromInside = true;
        dist = -b + sqrt(discr);
    }

	if (dist > c_minimumRayHitTime && dist < info.dist)
    {
        info.dist = dist;
        info.normal = normalize((rayPos+rayDir*dist) - sphere.xyz) * (fromInside ? -1.0f : 1.0f);
        return true;
    }

    return false;
}

mat3 getRotationMatrix(vec3 rotation)
{
    float pitch = rotation.x;
    float yaw = rotation.y;
    float roll = rotation.z;

    mat3 rotX = mat3(
        1.0, 0.0, 0.0,
        0.0, cos(pitch), -sin(pitch),
        0.0, sin(pitch), cos(pitch)
    );

    mat3 rotY = mat3(
        cos(yaw), 0.0, sin(yaw),
        0.0, 1.0, 0.0,
        -sin(yaw), 0.0, cos(yaw)
    );

    mat3 rotZ = mat3(
        cos(roll), -sin(roll), 0.0,
        sin(roll), cos(roll), 0.0,
        0.0, 0.0, 1.0
    );

    return rotZ * rotY * rotX;
}

uint wang_hash(inout uint seed)
{
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}

float RandomFloat01(inout uint state)
{
    return float(wang_hash(state)) / 4294967296.0;
}

vec3 RandomUnitVector(inout uint state)
{
    float z = RandomFloat01(state) * 2.0f - 1.0f;
    float a = RandomFloat01(state) * c_twopi;
    float r = sqrt(1.0f - z * z);
    float x = r * cos(a);
    float y = r * sin(a);
    return vec3(x, y, z);
}

void TestSceneTrace(in vec3 rayPos, in vec3 rayDir, inout SRayHitInfo hitInfo)
{
    for (int triIndex = 0; triIndex < numTriangles; ++triIndex) {
        Triangle tri = loadTriangle(triIndex);
        if (TestTriangleTrace(rayPos, rayDir, hitInfo,
              tri.v0,
              tri.v1,
              tri.v2))
        {
            hitInfo.material.albedo = tri.color;
            hitInfo.material.emissive = tri.emissive;
            hitInfo.material.percentSpecular = tri.percentSpecular;
            hitInfo.material.roughness = tri.roughness;
            hitInfo.material.specularColor = tri.specularColor;
        }
    }
}

vec3 GetColorForRay(in vec3 startRayPos, in vec3 startRayDir, inout uint rngState)
{
    // initialize
    vec3 ret = vec3(0.0f, 0.0f, 0.0f);
    vec3 throughput = vec3(1.0f, 1.0f, 1.0f);
    vec3 rayPos = startRayPos;
    vec3 rayDir = startRayDir;

    for (int bounceIndex = 0; bounceIndex <= c_numBounces; ++bounceIndex)
    {
        // shoot a ray out into the world
        SRayHitInfo hitInfo;
        hitInfo.dist = c_superFar;
        TestSceneTrace(rayPos, rayDir, hitInfo);

        // if the ray missed, we are done
        if (hitInfo.dist == c_superFar)
            break;

        // update the ray position
        rayPos = (rayPos + rayDir * hitInfo.dist) + hitInfo.normal * c_rayPosNormalNudge;

        // calculate whether we are going to do a diffuse or specular reflection ray
        float doSpecular = (RandomFloat01(rngState) < hitInfo.material.percentSpecular) ? 1.0f : 0.0f;

        vec3 diffuseRayDir = normalize(hitInfo.normal + RandomUnitVector(rngState));
        vec3 specularRayDir = reflect(rayDir, hitInfo.normal);
        specularRayDir = normalize(mix(specularRayDir, diffuseRayDir, hitInfo.material.roughness * hitInfo.material.roughness));
        rayDir = mix(diffuseRayDir, specularRayDir, doSpecular);

        // add in emissive lighting
        ret += hitInfo.material.emissive * throughput;

        // update the colorMultiplier
        throughput *= mix(hitInfo.material.albedo, hitInfo.material.specularColor, doSpecular);
    }

    // return pixel color
    return ret;
}

void main()
{
    vec2 pixelCoord = (vec2(FragPos.x, FragPos.y) + 1.0) * 1000;
    uint rngState = uint(pixelCoord.x) * uint(1973) + uint(pixelCoord.y) * uint(9277) + uint(iFrame) * uint(26699);
    rngState = rngState | uint(1);

    vec3 rayPosition = viewPos;

    // Calculate focal length from FOV
    float fovRadians = radians(fov);
    float focalLength = 1.0 / tan(fovRadians * 0.5);

    vec2 pixelTarget2D = vec2(FragPos.x * aspectRatio, FragPos.y);

    vec3 rayDirLocal = normalize(vec3(pixelTarget2D, -focalLength));

    mat3 rotationMatrix = getRotationMatrix(viewRotation);

    vec3 rayDir = rotationMatrix * rayDirLocal;

    vec3 currentColor = GetColorForRay(rayPosition, rayDir, rngState);

    vec2 uv = gl_FragCoord.xy / vec2(textureSize(previousFrame, 0));

    vec3 accumulatedColor;
    if (iFrame == 0) {
        accumulatedColor = currentColor;
    } else {
        vec3 previousColor = texture(previousFrame, uv).rgb;
        float weight = 1.0 / float(iFrame + 1);
        accumulatedColor = mix(previousColor, currentColor, weight);
    }

    FragColor = vec4(accumulatedColor, 1.0);
}
