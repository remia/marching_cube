#version 400

subroutine vec3 shadeModelType(vec4 position, vec3 normal);
subroutine uniform shadeModelType shadeModel;

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexColor;
layout (location = 2) in vec3 VertexNormal;
layout (location = 3) in vec2 VertexTexCoord;

out vec3 LightIntensity;
out vec3 Color;
out vec2 TexCoord;

struct LightInfo
{
	vec4 Position;	// Light position in eye coords
	vec3 Intensity;	// Light intensity (ads)
};
uniform LightInfo Light;

struct MaterialInfo
{
	vec3 Ka;			// Ambient reflectivity
	vec3 Kd;			// Diffuse reflectivity
	vec3 Ks;			// Specular reflectivity
	float Shininess;	// Specular shininess factor
};
uniform MaterialInfo Material;

layout (std140) uniform GlobalMatrices
{
	mat4 ViewMatrix;
};

uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;

void getEyeSpace(out vec3 normal, out vec4 position)
{
	normal = normalize(NormalMatrix * VertexNormal);
	position = ModelViewMatrix * vec4(VertexPosition, 1.0);
}

subroutine(shadeModelType)
vec3 ShadingSolid(vec4 position, vec3 normal)
{
	return Light.Intensity;
}

subroutine(shadeModelType)
vec3 ShadingDiffuse(vec4 position, vec3 normal)
{
	vec4 lv = ViewMatrix * Light.Position;
	vec3 s = normalize( vec3(lv - position) );
	return Light.Intensity * Material.Kd * max( dot(s, normal), 0.0);
}

subroutine(shadeModelType)
vec3 ShadingADS(vec4 position, vec3 normal)
{
	vec4 lv = ViewMatrix * Light.Position;
	vec3 s = normalize( vec3(lv - position) );
	vec3 v = normalize(-position.xyz);
	vec3 r = reflect(-s, normal);
	
	vec3 ambient = Light.Intensity * Material.Ka;
	float sDotN = max( dot(s, normal), 0.0);
	vec3 diffuse = Light.Intensity * Material.Kd * sDotN;
	vec3 spec = vec3(0.0);
	if(sDotN > 0.0)
		spec = Light.Intensity * Material.Ks *
			pow( max( dot(r,v), 0.0), Material.Shininess );

	return ambient + diffuse + spec;
}

void main()
{
	vec3 eyeNormal;
	vec4 eyePosition;

	// Get position & normal into eye space (view coordinate)
	getEyeSpace(eyeNormal, eyePosition);
 
	// Light according to subroutine uniform value
	LightIntensity = shadeModel(eyePosition, eyeNormal);// * vec3(1.0, 1.0, VertexPosition.y * 11.0);

	Color = VertexColor;
	TexCoord = VertexTexCoord;
	gl_Position = MVP * vec4(VertexPosition, 1.0);
}


