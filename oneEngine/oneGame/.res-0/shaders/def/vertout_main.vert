v2f_normals = v_worldNormal;
v2f_colors	= mdl_Color;
v2f_position= v_worldPosition;

// set the vertex position W as the screen-space Z
//v2f_position.w = v_screenPosition.w;//v_screenPosition.w;///gl_DepthRange.diff;
v2f_texcoord= mdl_TexCoord;