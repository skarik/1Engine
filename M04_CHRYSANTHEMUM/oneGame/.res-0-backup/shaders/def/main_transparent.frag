
vec4 colorDiffuse = mainDiffuse();
vec4 colorGlowmap = mainGlowmap();
vec4 colorNormals = v2f_normals;
vec4 colorPosition= v2f_position;
vec4 colorLighting= mainLighting();

gl_FragData[0] = colorDiffuse;
gl_FragData[4] = colorGlowmap;
gl_FragData[1] = colorNormals * vec4( 1,1,1, colorDiffuse.a );
gl_FragData[2] = colorPosition * vec4( 1,1,1, colorDiffuse.a );
gl_FragData[3] = colorLighting * vec4( 1,1,1, colorDiffuse.a );
