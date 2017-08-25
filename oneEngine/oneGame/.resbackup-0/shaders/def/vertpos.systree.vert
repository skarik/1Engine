ov_vertex = vec4( mdl_Vertex, 1.0 );
ov_vertex.x += sys_SinTime.y * mdl_Color.z * mdl_Color.y * (1.0+mdl_Normal.z) * 0.004;
ov_vertex.y += sys_CosTime.z * mdl_Color.z * mdl_Color.x * (1.0+mdl_Normal.z) * 0.004;
ov_normal = vec4( mdl_Normal, 1.0 );