#version 430
float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

// Based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float hash(float p) { p = fract(p * 0.011); p *= p + 7.5; p *= p + p; return fract(p); }
float hash(vec2 p) {vec3 p3 = fract(vec3(p.x, p.y, p.x) * 0.13); p3 += dot(p3, p3.yzx + 3.333); return fract((p3.x + p3.y) * p3.z); }

float noise(float x) {
    float i = floor(x);
    float f = fract(x);
    float u = f * f * (3.0 - 2.0 * f);
    return mix(hash(i), hash(i + 1.0), u);
}
float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}
float noise3(vec3 x) {
    const vec3 step = vec3(110, 241, 171);

    vec3 i = floor(x);
    vec3 f = fract(x);

    // For performance, compute the base input to a 1D hash from the integer part of the argument and the
    // incremental change to the 1D based on the 3D -> 1D wrapping
    float n = dot(i, step);

    vec3 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(mix( hash(n + dot(step, vec3(0, 0, 0))), hash(n + dot(step, vec3(1, 0, 0))), u.x),
                   mix( hash(n + dot(step, vec3(0, 1, 0))), hash(n + dot(step, vec3(1, 1, 0))), u.x), u.y),
               mix(mix( hash(n + dot(step, vec3(0, 0, 1))), hash(n + dot(step, vec3(1, 0, 1))), u.x),
                   mix( hash(n + dot(step, vec3(0, 1, 1))), hash(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z);
}

float fbm (in vec2 st) {
    // Initial values
    float value = 0.0;
    float amplitud = .5;
    float frequency = 0.;
    //
    // Loop of octaves
    for (int i = 0; i < 6; i++) {
        value += amplitud * noise(st);
        st *= 2.;
        amplitud *= .5;
    }
    return value;
}
float fbm3 (in vec3 st) {
    // Initial values
    float value = 0.0;
    float amplitud = .5;
    float frequency = 0.;
    //
    // Loop of octaves
    for (int i = 0; i < 6; i++) {
        value += amplitud * noise3(st);
        st *= 2.;
        amplitud *= .5;
    }
    return value;
}

vec2 fbmToVec2(in vec2 st) {
    return vec2(fbm(st), fbm(st + 100.0));
}
vec2 fbm3ToVec2(in vec3 st) {
    return vec2(fbm3(st), fbm3(st + 100.0));
}
vec3 fbmToVec3(in vec2 st) {
    return vec3(fbm(st), fbm(st + 18.0), fbm(st - 10.0));
}
vec3 fbm3ToVec3(in vec3 st) {
    return vec3(fbm3(st), fbm3(st + 18.0), fbm3(st - 10.0));
}

in vec4 varyingColor;
in vec3 varyingHalfH;

in vec3 varyingNormal;
in vec3 varyingLightDir;
in vec3 varyingVertPos;
in vec3 vertEyeSpacePos;
in vec3 initialVertPos;

uniform vec3 min_vertice_positions;
uniform vec3 max_vertice_positions;

uniform float fogNear;
uniform float fogFar;

out vec4 fragColor;

in vec3 realNormal;

struct PositionalLight {
    vec4 ambiant;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
};

struct Material {
    vec4 ambiant;
    vec4 diffuse;
    vec4 specular;
    float shininness;
};

uniform vec4 globalAmbiant;
uniform PositionalLight light;
uniform Material ground_material;
uniform Material grass_material;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;

void main(void)
{
    if (min_vertice_positions.x > initialVertPos.x || initialVertPos.x > max_vertice_positions.x || min_vertice_positions.y > initialVertPos.y || initialVertPos.y > max_vertice_positions.y || min_vertice_positions.z > initialVertPos.z || initialVertPos.z > max_vertice_positions.z)
        discard;
    Material material = ground_material;
    vec3 N = normalize(varyingNormal + fbm3ToVec3(initialVertPos)*0.5);
    vec3 L = normalize(varyingLightDir);
    vec3 V = normalize(-varyingVertPos);
    vec3 R = reflect(N, -L);
    vec3 H = normalize(varyingHalfH);
    float cosTheta = dot(L, N);
    float cosPhi = dot(H, N);

//    float upValue = 1 - clamp(dot(3 * normalize(realNormal + .1 * vec3(random(varyingVertPos.yz), random(varyingVertPos.xz), random(varyingVertPos.xy))), normalize(vec3(0.0, 1.0, 0.0))), 0.0, 1.0);
    float upValue = pow(clamp(dot(normalize(realNormal), normalize(vec3(0.0, 0.0, 1.0) + fbm3ToVec3(initialVertPos) * 0.5)), 0.0, 1.0), 2);
    vec4 material_ambiant = (ground_material.ambiant * (1 - upValue) + grass_material.ambiant * upValue) * .8;
    vec4 material_diffuse = (ground_material.diffuse * (1 - upValue) + grass_material.diffuse * upValue) * .8;
    vec4 material_specular = (ground_material.specular * (1 - upValue) + grass_material.specular * upValue) * 1.;
    float material_shininness = ground_material.shininness * (1 - upValue) + grass_material.shininness * upValue;

    vec4 ambiant = ((globalAmbiant * material_ambiant) + (light.ambiant * material_ambiant));
    vec4 diffuse = light.diffuse * material_diffuse * max(cosTheta, 0.0);
//    vec4 ambiantColor = vec4(varyingColor.xyz * .5, 1.0);
//    vec4 ambiant = ((globalAmbiant * ambiantColor) + (light.ambiant * ambiantColor));
//    vec4 diffuse = light.diffuse * (varyingColor*1.1) * max(cosTheta, 0.0);
    vec4 specular = light.specular * material_specular * pow(max(cosPhi, 0.0), material_shininness * 3.0);

    vec4 material_color = vec4((ambiant + diffuse + specular).xyz, 1.0);

    vec4 fogColor = vec4(0.2, 0.2, 0.2, 1.0);//vec4(0.7, 0.8, 0.9, 1.0);
    float fogStart = fogNear;
    float fogEnd = fogFar;
    float dist = length(vertEyeSpacePos);
    float fogFactor = clamp(((fogEnd - dist) / (fogEnd - fogStart)), 0.0, 1.0);

//    material_color = mix(material_color, varyingColor, .80);
    fragColor = vec4(mix(fogColor, material_color, fogFactor).xyz, 1.0);

//    fragColor = varyingColor; //vec4(1.0, 1.0, 1.0, 1.0);

}
