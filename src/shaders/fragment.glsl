#version 330 core

out vec4 color;

in vec2 texCoords;
in vec3 toCameraVec;
in vec3 toLightVec;
in vec3 surfaceNormal;

uniform sampler2D tex;
uniform float shineDamper;
uniform float reflectivity;

void main() {

	vec3 unitNormal = normalize(surfaceNormal);
	vec3 unitVecToCamera = normalize(toCameraVec);
	vec3 unitVecToLight = normalize(toLightVec);

	float nDot1 = dot(unitNormal, unitVecToLight);
	float brightness = max(nDot1, 0.0);
	vec3 lightDirection = -unitVecToLight;
	vec3 reflectedLightDirection = reflect(lightDirection, unitNormal);
	float specularFactor = dot(reflectedLightDirection, unitVecToCamera);
	specularFactor = max(specularFactor, 0.0);
	float dampedFactor = pow(specularFactor, shineDamper);
	vec3 totalDiffuse = brightness * vec3(1, 1, 1);
	vec3 totalSpecular = dampedFactor * reflectivity * vec3(1, 1, 1);

	totalDiffuse = max(totalDiffuse, 0.5);

	vec4 texColor = texture(tex, texCoords);

	color = vec4(totalDiffuse, 1.0) * texColor + vec4(totalSpecular, 1.0);
}