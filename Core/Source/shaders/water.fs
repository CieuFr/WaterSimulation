#version 450 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

struct MeshGPUQuad {
   vec3 albedo;
   float metallic;
   float roughness;
   vec3 vertices[4];
   vec3 normals[4];
   vec2 uvs[4];
   mat4 model;
};

struct MeshGPUSphere {
   vec3 albedo;
   float metallic;
   float roughness;
   vec3 vertices[100];
   vec3 normals[100];
   vec2 uvs[100];
   int indices[500];
   mat4 model;
   int nb_indices;
   int nb_vertices;
};

// TODO MODIFIER LE NOMBRE DE MESH
const int n_meshes = 5;
uniform MeshGPUQuad meshQuads[n_meshes];

const int n_meshes_sphere = 1;
uniform MeshGPUSphere meshSphere[n_meshes_sphere];

uniform mat4 modelSphere;

uniform sampler2D water;
uniform samplerCube skybox;

bool debug = false;

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



 float raySphereIntersection(vec3 rayOrigin, vec3 rayDirection,
            vec3 sphereCenter, float sphereRadius) {
            vec3 oc = rayOrigin - sphereCenter;
            float a = dot(rayDirection, rayDirection);
            float b = 2.0f * dot(oc, rayDirection);
            float c = dot(oc, oc) - sphereRadius * sphereRadius;
            float discriminant = b * b - 4 * a * c;
            if (discriminant < 0) {
                return -1;
            }
            else {    
                float t1 = (-b - sqrt(discriminant)) / (2.0 * a);
                float t2 = (-b + sqrt(discriminant)) / (2.0 * a);
                
                float t = min(t1, t2);
                
                return t;  
            }
        }


bool intersectTriangle(Ray p_ray, vec3 v0, vec3 v1, vec3 v2, vec3 n0, vec3 n1, vec3 n2 , out float p_t, out vec3 p_n, out float p_u, out float p_v )
	{
       
        float _epsilon = 0.001;
		vec3 o	 = p_ray.origin;
		vec3 d	 = p_ray.direction;

		vec3 edge1, edge2, tvec, pvec, qvec;
		float det, inv_det, t, u, v;
		edge1 = v1 - v0;
		edge2 = v2 - v0;
		pvec  = cross( d, edge2 ); 
		det	  = dot( edge1, pvec );

		if ( det > -_epsilon && det < _epsilon ) return false;
		inv_det = 1.f / det;
		tvec	= o - v0;
		u		= dot( tvec, pvec ) * inv_det;
		if ( u < 0 || u > 1 ) return false;
		qvec = cross( tvec, edge1 );
		v	 = dot( d, qvec ) * inv_det;
		if ( v < 0 || u + v > 1 ) return false;
		t = dot( edge2, qvec ) * inv_det;
		
		p_t		 = t;
		p_u		 = u;
		p_v		 = v;
		
		vec3 n	 = ( 1 - u - v ) * n0 + u * n1 + v * n2;
		p_n		 = n;
		return true;
}

    

bool intersectScene(Ray ray, float t_min, float t_max, out IntersectInfo rec)
{
        bool hit_anything = false;
        float closest_so_far = t_max;
        MeshGPUQuad meshes[n_meshes] = meshQuads;
        // INTERSECT QUADS ONLY
        for(int i = 0; i < n_meshes;i++){
            MeshGPUQuad mesh = meshes[i];
            vec4 world_p0 = mesh.model * vec4(mesh.vertices[0],1.0);
            vec4 world_p1 = mesh.model * vec4(mesh.vertices[1],1.0);
            vec4 world_p2 = mesh.model * vec4(mesh.vertices[2],1.0);
            vec4 world_p3 = mesh.model * vec4(mesh.vertices[3],1.0);
 
            vec3 po_p2 = world_p2.rgb-world_p0.rgb; 
            vec3 po_p1 = world_p1.rgb-world_p0.rgb;
            vec3 p1_p3 = world_p3.rgb-world_p1.rgb;
            vec3 p3_p2 = world_p2.rgb-world_p3.rgb;

            vec3 world_n0 = cross(po_p1,po_p2);
            vec3 world_n1 = cross(p1_p3,-po_p1);
            vec3 world_n2 = cross(-po_p2,-p3_p2);
            vec3 world_n3 = cross(p3_p2,-p1_p3);

            float t,u,v;
            vec3 n;

            if(intersectTriangle(ray,world_p0.rgb,world_p1.rgb,world_p2.rgb,world_n0,world_n1,world_n2,t,n,u,v)){
                if ( t >= t_min && t <= closest_so_far){

                    hit_anything = true;
                    rec.albedo = mesh.albedo;
                    rec.point = ray.origin + ray.direction * t;
                    closest_so_far = t;
                    rec.distance = t;
                    rec.normal = n;
                }
                // TODO
            }

            if(intersectTriangle(ray,world_p3.rgb,world_p1.rgb,world_p2.rgb,world_n3,world_n1,world_n2,t,n,u,v)){
                if ( t >= t_min && t <= closest_so_far){
                    hit_anything = true;
                    rec.albedo = mesh.albedo;
                    rec.point = ray.origin + ray.direction *t;
                    closest_so_far = t;
                    rec.distance = t;
                    rec.normal = n;
                }
            }

        }
        
        MeshGPUSphere meshesSphere[n_meshes_sphere] = meshSphere;

        for(int i = 0; i < n_meshes_sphere; i++ ){
            for(int j = 0; j < meshesSphere[i].nb_indices; j++){

                int index1 = meshesSphere[i].indices[j*3];
                int index2 = meshesSphere[i].indices[j*3+1];
                int index3 = meshesSphere[i].indices[j*3+2];

                

                vec3 v1 = vec4(meshesSphere[i].model * vec4( meshesSphere[i].vertices[index1],1)).rgb;
                vec3 v2 = vec4(meshesSphere[i].model * vec4( meshesSphere[i].vertices[index2],1)).rgb;
                vec3 v3 = vec4(meshesSphere[i].model * vec4( meshesSphere[i].vertices[index3],1)).rgb;

                vec3 n1 = vec4( vec4(meshesSphere[i].normals[index1] ,1)).rgb;
                vec3 n2 = vec4( vec4(meshesSphere[i].normals[index2] ,1)).rgb;
                vec3 n3 = vec4( vec4(meshesSphere[i].normals[index3] ,1)).rgb;
                 float t,u,v;
                vec3 n;
                if(intersectTriangle(ray,v1,v2,v3,n1,n2,n3,t,n,u,v))
                {
                    if ( t >= t_min && t <= closest_so_far){
                        hit_anything = true;
                        rec.albedo = vec3(1,0,0);
                        rec.point = ray.origin + ray.direction *t;
                        closest_so_far = t;
                        rec.distance = t;
                        rec.normal = n;
                    }
                }
            }
        }
    return hit_anything;
}


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

float fresnelProp(vec3 I, vec3 N, float ior)
{
    float kr;
    float cosi = clamp(dot(I, N),-1, 1 );
    float etai = 1, etat = ior;
    if (cosi > 0) { 
        float temp = etai;
        etai = etat;
        etat = temp;
}

    // Compute sini using Snell's law
    float sint = etai / etat * sqrt(max(0.f, 1 - cosi * cosi));
    // Total internal reflection
    if (sint >= 1) {
        kr = 1;
    }
    else {
        float cost = sqrt(max(0.f, 1 - sint * sint));
        cosi = abs(cosi);
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        kr = (Rs * Rs + Rp * Rp) / 2;
    }
    // As a consequence of the conservation of energy, the transmittance is given by:
    //kt = 1 - kr;
    return kr;
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

vec3 skyColor(Ray ray)
{
    vec3 unit_direction = normalize(ray.direction);
    float t = 0.5 * (unit_direction.y + 1.0);

    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
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

    vec3 normalF = Normal;
    
    vec3 reflectedColor = vec3(0,0,0);
    vec3 refractedColor = vec3(0,0,0);
    
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    vec3 vecteurIncident = normalize(WorldPos - camPos);
    Ray rayonIncident = Ray(WorldPos, vecteurIncident);
    vec3 vecteurReflechi = reflect(vecteurIncident,normalF);
 vec3 bias = 0.001 * normalF;
    bool outside = dot(normalF, -vecteurIncident) < 0;
    vec3 originReflect = outside ? WorldPos + bias : WorldPos - bias;

    Ray rayonReflechi = Ray(originReflect,vecteurReflechi);
    IntersectInfo rec;

    if(intersectScene(rayonReflechi, 0.001, 100000, rec)){
        reflectedColor = shade(rec.point, rec.normal, camPos, rec.albedo, metallic, roughness, lightPositions, lightColors);    
    } else {
        reflectedColor = texture(skybox, rayonReflechi.direction).rgb;
    }
 
    vec3 vecteurRefracte;

    vec3 Lo;
    float n1 = 1.f;
    float n2 = 1.33f;

    float cosTheta;
   
   
    float fresnel = fresnelProp(vecteurIncident,normalF,n2);
    vec3 Nrefr = Normal;
    float NdotI = dot(Nrefr,vecteurIncident);
    float etai = 1, etat = n2; 
    if (NdotI < 0) {
        // we are outside the surface, we want cos(theta) to be positive
        NdotI = -NdotI;
        Nrefr = Normal;
    }
    else {
        // we are inside the surface, cos(theta) is already positive but reverse normal direction
        Nrefr = -Normal;
        // swap the refraction indices
        float temp = etai;
        etai = etat;
        etat = temp;
    }
    
    float eta = etai / etat; 

    
    if(fresnel < 1) {

        vecteurRefracte = refract(vecteurIncident,Nrefr,eta);
   vec3 originRefract = outside ? WorldPos + bias : WorldPos - bias;
        Ray rayonRefracte = Ray(originRefract,vecteurRefracte);

         if(intersectScene(rayonRefracte, 0.001, 100000, rec)){
            refractedColor = shade(rec.point, rec.normal, camPos, rec.albedo, metallic, roughness, lightPositions, lightColors);
         } else {
             refractedColor = texture(skybox, rayonRefracte.direction).rgb;
         }

          Lo = reflectedColor * fresnel + refractedColor * (1 - fresnel);

    } else {

         Lo = reflectedColor;
    }

   
      

    

     

   

    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 


if(debug){

   
    

} else {
    FragColor = vec4(color, 1.0);
    
}
    


   
}

