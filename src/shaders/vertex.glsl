#version 330 core

layout(location = 0) in vec3 vertexPosition_modelSpace;
layout(location = 1) in vec2 uvCoords;
layout(location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view_projection;
uniform vec3 cameraPos;

out vec2 texCoords;
out vec3 toCameraVec;
out vec3 toLightVec;
out vec3 surfaceNormal;

void main() {
	vec4 worldPos = model * vec4(vertexPosition_modelSpace, 1.0);
	gl_Position = view_projection * worldPos;
	texCoords = uvCoords;
	toCameraVec = cameraPos - worldPos.xyz;
	vec3 lightPos = vec3(5, 10, 10);
	toLightVec = lightPos - worldPos.xyz;
	surfaceNormal = (model * vec4(normal, 0.0)).xyz;
}