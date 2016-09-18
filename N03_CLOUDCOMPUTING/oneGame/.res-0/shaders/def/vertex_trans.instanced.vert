vec4 inst_pos = texelFetch( textureInstanceBuffer, gl_InstanceID*2 + 0 );
vec4 inst_rot = texelFetch( textureInstanceBuffer, gl_InstanceID*2 + 1 );

vec4 v_localScaledPos = sys_ModelMatrix * vec4(mdl_Vertex,1.0);
vec4 v_localRotatedPos = quat_rotate_point(inst_rot, v_localScaledPos.xyz);
vec4 v_worldPosition = v_localRotatedPos + inst_pos;
v_worldPosition.w = 1.0;

vec4 v_worldNormal		= quat_rotate_point(inst_rot, (sys_ModelRotationMatrix * ov_normal).xyz);
vec4 v_screenPosition	= sys_ViewProjectionMatrix * v_worldPosition;

gl_Position = v_screenPosition;
