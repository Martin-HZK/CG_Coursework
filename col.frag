#version 330 core

in vec3 col;
in vec3 nor;
in vec3 FragPos;
in vec2 tex;

uniform vec3 lightColour;
uniform vec3 lightDirection;
uniform vec3 lightPos;
uniform vec3 camPos;
uniform sampler2D Texture;
uniform float light_decider;
//uniform sampler2D Texture2;

uniform int useTexture1;

out vec4 fragColour;


/*float CalculateDirectionIllumination()
{
	float amb = 0.1f;

	vec3 Nnor = normalize(nor);
	vec3 Ntolight = normalize(-lightDirection);
	float diff = max(dot(Nnor, Ntolight), 0.f);

	vec3 Nfromlight = normalize(lightDirection);
	vec3 refDir = reflect(Ntolight, Nnor);
	vec3 camDir = normalize(camPos - FragPos);
	float spec = pow(max(dot(camDir, refDir), 0.f), 128);
	
	float i = amb + diff + spec;

	return i;
}*/
float CalculateDirectionIllumination()
{
    float amb = 0.1f;

    vec3 Nnor = normalize(nor);
    vec3 Ntolight = normalize(-lightDirection);
    float diff = max(dot(Nnor, Ntolight), 0.f);

    vec3 refDir = reflect(-Ntolight, Nnor);
    vec3 camDir = normalize(camPos - FragPos);
    float spec = pow(max(dot(camDir, refDir), 0.f), 128); // 调整高光指数为32
    
    float i = amb + diff + spec;

    return i;
}

float CalculatePositionalIllumination()
{
	float amb = 0.1f;

	vec3 Nnor = normalize(nor);
	vec3 NToLight = normalize(lightPos - FragPos);
	float diff = max(dot(Nnor, NToLight), 0.f);

	vec3 NFromLight = -NToLight;
	vec3 refDir = reflect(NFromLight, Nnor);
	vec3 NcamDir = normalize(camPos - FragPos);
	float spec = pow(max(dot(NcamDir, refDir), 0.f), 128);

	float d = length(lightPos - FragPos);
	float c = 1.5f;
	float l = .05f;
	float q = .02f;
	float att = 1.f / (c + (l*d) + (q*(d*d)));

	float i = (amb + diff + spec) * att;

	return i;
}


float CalculateSpotIllumination()
{
	float amb = 0.1f;

	vec3 Nnor = normalize(nor);
	vec3 NToLight = normalize(lightPos - FragPos);
	float diff = max(dot(Nnor, NToLight), 0.f);

	vec3 NFromLight = -NToLight;
	vec3 refDir = reflect(NFromLight, Nnor);
	vec3 NcamDir = normalize(camPos - FragPos);
	float spec = pow(max(dot(NcamDir, refDir), 0.f), 128);

	float d = length(lightPos - FragPos);
	float c = 1.5f;
	float l = .05f;
	float q = .02f;
	float att = 1.f / (c + (l*d) + (q*(d*d)));

	float phi = cos(radians(15.f));
	vec3 NSpotDir = normalize(lightDirection);
	float theta = dot(NFromLight, NSpotDir);

	float i;
	if(theta > phi)
	{
		i = (amb + diff + spec) * att;
	}
	else
	{
		i = (amb) * att;
	}

	return i;
}

void main()
{
	 //fragColour = vec4(col, 1.f);
	 float phong;
	 if (light_decider == 1)
		phong = CalculateDirectionIllumination();
	if (light_decider == 2)
		phong = CalculatePositionalIllumination();
	if (light_decider == 3)
		phong = CalculateSpotIllumination();
	//fragColour = vec4(phong * col * lightColour, 1.f);
	//vec4 textureColour;
	//if (useTexture1 == 1) {
		//fragColour = texture(Texture1, tex);
	//} else {
		//fragColour = texture(Texture2, tex);
	//}
	//fragColour = texture(Texture, tex);
	
     // Sample texture color
    vec4 textureColour = texture(Texture, tex);

    // Combine texture color with lighting
    vec3 finalColour = phong * textureColour.rgb * lightColour;

    // Output final color
    fragColour = vec4(finalColour, 1.0f);

}
