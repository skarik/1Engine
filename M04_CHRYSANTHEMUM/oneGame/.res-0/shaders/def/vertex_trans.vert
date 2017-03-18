
vec4 v_worldPosition	= sys_ModelTRS * ov_vertex;
vec4 v_worldNormal		= sys_ModelRS * ov_normal;
vec4 v_screenPosition	= sys_ModelViewProjectionMatrix * ov_vertex;

gl_Position = v_screenPosition;