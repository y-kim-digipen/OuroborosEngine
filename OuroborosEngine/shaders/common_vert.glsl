layout(set = 0, binding = 0) uniform global_data {
    mat4 projection;
    mat4 view;
    vec3 cam_pos;
    mat4 inv_view;
    mat4 inv_proj;
} global_ubo;


layout(push_constant) uniform constants {
    mat4 model;
    mat4 normal_matrix;
} object_ubo;