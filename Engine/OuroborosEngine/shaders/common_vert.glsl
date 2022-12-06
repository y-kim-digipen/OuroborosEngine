layout(set = 0, binding = 0) uniform global_data {
    mat4 projection;
    mat4 view;
    mat4 inv_view;
    vec3 cam_pos;
} global_ubo;


layout(push_constant) uniform constants {
    mat4 model;
    mat4 normal_matrix;
} object_ubo;