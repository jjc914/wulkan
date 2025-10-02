#version 460
#extension GL_EXT_ray_tracing : require

layout(location = 0) rayPayloadInEXT vec3 payload;
hitAttributeEXT vec2 attribs; // required declaration for triangle hits (unused here)

void main() {
    // constant warm color on hit
    payload = vec3(1.0, 0.8, 0.2);
}
