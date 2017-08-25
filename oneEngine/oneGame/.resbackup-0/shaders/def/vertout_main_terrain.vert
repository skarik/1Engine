v2f_normals = v_worldNormal;
v2f_colors	= mdl_Color;
v2f_position= v_worldPosition;
v2f_texcoord= (1+2*mdl_TexCoord.xyz)/(2*16);

v2f_ter_blends1 = ter_Blends1;
v2f_ter_blends1.xyz /= (ter_Blends1.x+ter_Blends1.y+ter_Blends1.z);
v2f_ter_blends2 = ter_Blends2;