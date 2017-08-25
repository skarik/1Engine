
vec4 colorDiffuse = mainDiffuse();
vec4 colorGlowmap = mainGlowmap();
vec4 colorNormals = v2f_normals;
//vec4 colorPosition= v2f_position;
//colorNormals.w = v2f_position.w;
colorNormals.w = 1.0;
vec4 colorLighting= mainLighting();

FragDiffuse = colorDiffuse;
FragNormals = colorNormals;
//gl_FragData[2] = colorPosition;
FragLighting = colorLighting;
FragGlowmap = colorGlowmap;