#version 450
#extension GL_ARB_separate_shader_objects : enable

//Bone Code from https://github.com/SaschaWillems/Vulkan/blob/master/data/shaders/skeletalanimation/mesh.vert
#define MAX_BONES 64

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 bones[MAX_BONES];
} ubo;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inBoneWeights;
layout(location = 4) in ivec4 inBoneIDs;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;

void main()
{
    mat4 boneTransform = ubo.bones[inBoneIDs[0]] * inBoneWeights[0];
	boneTransform     += ubo.bones[inBoneIDs[1]] * inBoneWeights[1];
 
    gl_Position = ubo.proj * ubo.view * ubo.model * boneTransform * vec4(inPosition, 1.0);
    fragNormal = inNormal;
    fragTexCoord = inTexCoord;
}
