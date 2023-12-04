#version 450 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;


struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct IntersectInfo
{
    // surface properties
    float distance;
    vec3  point;
    vec3  normal;
	
    // material properties
    int   materialType;
    vec3  albedo;
    float fuzz;
    float refractionIndex;
};

bool refractVec(vec3 v, vec3 n, float ni_over_nt, out vec3 refracted)
{
    vec3 uv = normalize(v);

    float dt = dot(uv, n);

    float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1.0f - dt * dt);

    if (discriminant > 0.0f)
    {
        refracted = ni_over_nt*(uv - n * dt) - n * sqrt(discriminant);

        return true;
    }
    else
        return false;
}

vec3 reflectVec(vec3 v, vec3 n)
{
     return v - 2.0f * dot(v, n) * n;
}

/*bool intersectScene(Ray ray, float t_min, float t_max, out IntersectInfo rec)
{
        IntersectInfo temp_rec;

        bool hit_anything = false;
        float closest_so_far = t_max;

        for (int i = 0; i < sceneList.length(); i++)
        {
            Sphere sphere = sceneList[i];

            if (Sphere_hit(sphere, ray, t_min, closest_so_far, temp_rec))
            {
                hit_anything   = true;
                closest_so_far = temp_rec.t;
                rec            = temp_rec;
            }
        }

        return hit_anything;
}*/
/*
bool intersectTriangle( const Ray & p_ray, float & p_t, Vec3f & p_n, float & p_u, float & p_v ) const
	{
		const Vec3f & o	 = p_ray.getOrigin();
		const Vec3f & d	 = p_ray.getDirection();
		const Vec3f & v0 = _refMesh->_vertices[ _v0 ];
		const Vec3f & v1 = _refMesh->_vertices[ _v1 ];
		const Vec3f & v2 = _refMesh->_vertices[ _v2 ];

		Vec3f edge1, edge2, tvec, pvec, qvec;
		float det, inv_det, t, u, v;
		edge1 = v1 - v0;
		edge2 = v2 - v0;
		pvec  = glm::cross( d, edge2 );
		det	  = glm::dot( edge1, pvec );

		
		if ( det > -_epsilon && det < _epsilon ) return 0;
		inv_det = 1.f / det;
		tvec	= o - v0;
		u		= glm::dot( tvec, pvec ) * inv_det;
		if ( u < 0 || u > 1 ) return false;
		qvec = glm::cross( tvec, edge1 );
		v	 = glm::dot( d, qvec ) * inv_det;
		if ( v < 0 || u + v > 1 ) return false;
		t = glm::dot( edge2, qvec ) * inv_det;
		
		p_t		 = t;
		p_u		 = u;
		p_v		 = v;
		Vec3f n0 = _refMesh->_normals[ _v0 ];
		Vec3f n1 = _refMesh->_normals[ _v1 ];
		Vec3f n2 = _refMesh->_normals[ _v2 ];
		Vec3f n	 = ( 1 - u - v ) * n0 + u * n1 + v * n2;
		p_n		 = n;

		return true;
	}
    */
vec3 radiance(Ray ray)
{
    IntersectInfo rec;

    vec3 col = vec3(1.0, 1.0, 1.0);
    int MAXDEPTH = 3;
    for(int i = 0; i < MAXDEPTH; i++)
    {
        // if (true)//intersectScene(ray, 0.001, MAXFLOAT, rec))
        // {
        //     Ray wi;
        //     vec3 attenuation;

        //     //bool wasScattered = Material_bsdf(rec, ray, wi, attenuation);

        //     ray.origin = wi.origin;
        //     ray.direction = wi.direction;

        //     if (wasScattered)
        //        col *= attenuation;
        //    else
        //     {
        //        col *= vec3(0.0f, 0.0f, 0.0f);
        //        break;
        //    }
        // }
        // else
        // {
        //     col *= skyColor(ray);
        //     break;
        // }
    }

    return col;
}

layout (binding = 0) uniform samplerBuffer triangleData;
layout (binding = 1) uniform samplerBuffer modelLocsData;
layout (binding = 2) uniform samplerBuffer modelScalesData;
layout (binding = 3) uniform samplerBuffer modelColorsData;
layout (binding = 4) uniform samplerBuffer modelRotationsData;
layout (binding = 5) uniform isamplerBuffer modelNumTrianglesData;
layout (binding = 7) uniform samplerBuffer modelInverseRotationsData;
layout (binding = 6) uniform samplerBuffer randNums;

// material parameters
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;
uniform mat4 projection;
uniform mat4 view;
uniform vec2 resolution;
uniform int samples;


// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 camPos;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float schlick(float cos_theta, float n2)
{
    const float n1 = 1.0f;  // indice de refraction de l'air

    float r0s = (n1 - n2) / (n1 + n2);
    float r0 = r0s * r0s;

    return r0 + (1.0f - r0) * pow((1.0f - cos_theta), 5.0f);
}

highp float rand(vec2 co)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy ,vec2(a,b));
    highp float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}
 
vec3 shade(vec3 point, vec3 normal, vec3 camPos, vec3 albedo, float metallic, float roughness,vec3 lightPositions[4], vec3 lightColors[4]){
    vec3 N = normalize(normal);
    vec3 V = normalize(camPos - point);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again 
    }   

    return Lo;
}


// ----------------------------------------------------------------------------
void main()
{  
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    vec3 wo = (camPos - WorldPos);
    vec3 vecteurIncident = (WorldPos - camPos);
    Ray rayonIncident = Ray(WorldPos, vecteurIncident);
    vec3 vecteurReflechi = reflectVec(vecteurIncident,Normal);
    Ray rayonReflechi = Ray(WorldPos,vecteurReflechi);

    vec3 vecteurRefracte;
    float refractionIndex = 1.33f; // indice de refraction de l eau 
    float ni_over_nt;
    float reflect_prob = 1.0f;
    vec3 outward_normal;
    float cosine;

    if (dot(wo, Normal) > 0.0f)
    {
        outward_normal = -Normal;
        ni_over_nt = refractionIndex;
    
        cosine = dot(wo,Normal) / length(wo);
        cosine = sqrt(1.0f - refractionIndex * refractionIndex * (1.0f - cosine * cosine));
    }
    else
    {
        outward_normal = Normal;
        ni_over_nt = 1.0f / refractionIndex;
        cosine = -dot(wo,Normal) / length(wo);
    }
    
    if(refractVec(vecteurIncident,outward_normal,ni_over_nt,vecteurRefracte)){
        reflect_prob = schlick(cosine, refractionIndex);
    }
    if (rand(uv) < reflect_prob)
    {
        // reflechi
    }
    else
    {
        // refracte
    }

    vec3 Lo = shade(WorldPos,Normal,camPos,albedo,metallic,roughness,lightPositions,lightColors);

    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
   
}

