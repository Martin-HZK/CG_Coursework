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
//uniform sampler2D Texture2;

uniform int useTexture1;

out vec4 fragColour;

float CalculateDirectionIllumination()
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
}

void main()
{
	 //fragColour = vec4(col, 1.f);
	float phong = CalculateDirectionIllumination();
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
    fragColour = vec4(finalColour, textureColour.a);

}
