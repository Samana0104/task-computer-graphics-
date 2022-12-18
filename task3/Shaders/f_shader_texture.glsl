#version 330

in vec4 fs_color;
in vec2 fs_uv;
in vec3 fs_normal;
in vec3 fs_eye_dir;

uniform int shading_mode = 1;

layout (location = 0) out vec4 color;

struct Light
{
	int type;


	vec3 dir;		
	vec3 position;
	float intensity; // I_l
	float cos_cutoff;
};

uniform Light directionLight;		
uniform Light spotLight;		

uniform float ambient = 0.2f;
uniform float shininess_n;	
//uniform vec3 K_s;

uniform sampler2D tex0;
uniform bool flag_texture;
uniform int lightType = 1;


void main()
{
	//vec3 K_d = fs_color.rgb;
	float diffuse = 0.f;
	float specular = 0.f;
	float lightSum = 0.f;

	vec3 DL = normalize(directionLight.position-fs_eye_dir);
	vec3 SL = normalize(spotLight.position-fs_eye_dir);

	vec3 N = normalize(fs_normal);
	vec3 V = normalize(-fs_eye_dir);

	vec3 DH = (DL+V)/length(DL+V);
	vec3 SH = (SL+V)/length(SL+V);

	vec3 Sd = normalize(-spotLight.dir);


	if ( dot(Sd,SL) >= spotLight.cos_cutoff )
	{
		float d = length(spotLight.position-fs_eye_dir);
		diffuse += min(100.0f/(d*d), 1.5f) * spotLight.intensity * pow(max(0.f, dot(SL, N)), 2);
		specular += spotLight.intensity * 0.15f * pow(max(0.f, dot(N, SH)), 30);
	}

	if(lightType == 1)
	{
		diffuse += directionLight.intensity * 1.f * max(ambient, dot(DL, N));
		specular += directionLight.intensity * 1.f * pow(max(0.f, dot(N, DH)), 30);
	} 

	lightSum += diffuse + specular;

	if (flag_texture)
		color = vec4(lightSum, lightSum, lightSum, 1.f) * texture(tex0, fs_uv);
	else 
		color = vec4(lightSum, lightSum, lightSum, 1.f) * fs_color;
}