#version 330 core

in Interpolators {
    vec2 uv;
    vec3 view;
    vec3 normal;
} fs_in;

in float visibility;
vec3 skyColour = vec3(0.859375,0.809375,0.889375);

struct Material {
    sampler2D ambient_occlusion;
    sampler2D emissive;
    vec3 emissive_tint;
    //TODO: Complete The Material Struct. Find the variable names in the scene code. 
    sampler2D albedo;
    sampler2D specular;
    sampler2D roughness;
    vec3 albedo_tint;
    vec3 specular_tint;
    float roughness_scale;
};
uniform Material material;

struct DirectionalLight {
    vec3 color;
    vec3 direction;
};
uniform DirectionalLight light;

uniform vec3 ambient;

out vec4 color;

float lambert(vec3 n, vec3 l){
    return max(0, dot(n,l));
}

float phong(vec3 n, vec3 l, vec3 v, float shininess){
    //TODO: Change Model from Phong to Blinn Phong
    
    vec3  halfway = normalize(l+v); 
    return pow(max(0, dot(n,halfway)), shininess);
    //return pow(max(0, dot(v,reflect(-l, n))), shininess);
}

void main()
{
    float ao = texture(material.ambient_occlusion, fs_in.uv).r;
    vec3 emissive = material.emissive_tint * texture(material.emissive, fs_in.uv).rgb;
    
    //TODO: fix albedo, specular and roughness from textures and apply tint and/or scale 
    /*vec3 albedo = texture(material.albedo,Interpolators.uv);
    vec3 specular = material.specular;
    float roughness = material.roughness;
*/
    vec3 albedo =  material.albedo_tint * texture(material.albedo,fs_in.uv). b;
    vec3 specular = material.specular_tint *texture(material.specular,fs_in.uv).rgb;
    float roughness =  material.roughness_scale *texture(material.roughness,fs_in.uv).r;

    vec3 n = normalize(fs_in.normal);
    vec3 v = normalize(fs_in.view);
    vec3 l = -light.direction;
    float shininess = 2/pow(max(0.01f,roughness), 2) - 2;
    color = vec4(
        emissive +
        albedo*ao*ambient + 
        albedo*light.color*lambert(n, l) + 
        specular*light.color*phong(n, l, v, shininess),
        1.0f
    );
    color = mix(vec4(skyColour,1.0), color, visibility);
}