#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../cbuffers.glsli"
#include "../deferred_surface.glsli"
#include "../colorspaces.glsli"

// Output to screen
layout(location = 0) out vec4 FragColor;

// Inputs from vertex shader
layout(location = 0) in vec4 v2f_position;
layout(location = 1) in vec2 v2f_texcoord0;
layout(location = 2) in flat int v2f_lightIndex;

// Previous forward rendered output
layout(binding = 0, location = 25) uniform sampler2D textureSamplerColor;
layout(binding = 1, location = 26) uniform sampler2D textureSamplerDepth;


void GetPoissonDiskCentered ( inout vec2 samples[16] )
{
	// TODO: make a better distribution
	samples[0] = vec2( 0.0f, 0.0f );
	samples[1] = vec2( 0.37938176825979075f, 0.9197961450483562f );
	samples[2] = vec2( 0.44376077488621757f, -0.8681705017844068f );
	samples[3] = vec2( -0.9770041668388132f, -0.14352717652490568f );
	samples[4] = vec2( -0.5072300050925068f, 0.8173721868214128f );
	samples[5] = vec2( 0.9705969658196754f, -0.07411190125385869f );
	samples[6] = vec2( -0.5447278385494391f, -0.7330150126693368f );
	samples[7] = vec2( 0.7924953615781184f, 0.4744373857303271f );
	samples[8] = vec2( -0.04774441730255103f, 0.528506747258193f );
	samples[9] = vec2( 0.4669766355018519f, -0.2961978681469587f );
	samples[10] = vec2( -0.6828021954665753f, 0.3004207569186437f );
	samples[11] = vec2( -0.12308177890302552f, -0.988754387788333f );
	samples[12] = vec2( -0.4287891798409814f, -0.08872259098105253f );
	samples[13] = vec2( -0.09824971501658032f, -0.4723684027780304f );
	samples[14] = vec2( 0.4029932102673596f, 0.20294659628372153f );
	samples[15] = vec2( -0.10681304298418018f, 0.9445536419747136f );
}

void GetPoissonDisk ( inout vec2 samples[16] )
{
	samples[0] = vec2( -0.7451572426064842f, 0.5965690130573613f );
	samples[1] = vec2( -0.25413562199622775f, 0.5649694749537443f );
	samples[2] = vec2( 0.0811950194344549f, 0.9878880966855352f );
	samples[3] = vec2( 0.2934032692264268f, 0.540281458026134f );
	samples[4] = vec2( 0.7624257004722369f, 0.6047629314931843f );
	samples[5] = vec2( -0.528382512974986f, 0.017211548645211475f );
	samples[6] = vec2( -0.9760839504024266f, 0.03282596767803959f );
	samples[7] = vec2( 0.09509074784161684f, 0.11531192154757579f );
	samples[8] = vec2( 0.6019533729678995f, 0.1301509661193808f );
	samples[9] = vec2( -0.8853574363497273f, -0.46040592488140675f );
	samples[10] = vec2( -0.18226921262609838f, -0.46271446524867643f );
	samples[11] = vec2( 0.40425013610644256f, -0.35170806548224487f );
	samples[12] = vec2( 0.9727771296742238f, -0.22639479503867072f );
	samples[13] = vec2( -0.36219378727350315f, -0.9288666773955668f );
	samples[14] = vec2( 0.2992998212781267f, -0.9295434259658686f );
	samples[15] = vec2( 0.6890030873181967f, -0.7045467584605423f );
}


void main ( void )
{
	// Sample the center pixel's depth.
	float referenceDepth = LinearizeZBufferDepth(texture(textureSamplerDepth, v2f_texcoord0).x);
	
	// Need correct aspect in order to make sure the outlines are consistent sizes in both X and Y
	const vec2 aspectCorrectOffset = vec2(1.0, sys_ScreenSize.x / sys_ScreenSize.y);
	
	vec2 samples [16];
	GetPoissonDisk(samples);
	
	const float kMinThickness = 1.3 / 1280.0;
	const float kMaxThickness = 3.3 / 1280.0;
	
	float darkening = 0.0;
	
	[[unroll]]
	for (int i = 0; i < 16; ++i)
	{
		const vec2 sampleOffset = samples[i] * aspectCorrectOffset * kMaxThickness;
		const float sampleWidth = length(samples[i]) * kMaxThickness;
		
		// Sample from the zbuffer
		float sampleDepth = LinearizeZBufferDepth(texture(textureSamplerDepth, v2f_texcoord0 + sampleOffset).x);
		// Get a delta that gets smaller as we get further from the camera.
		// Smaller delta at distance means smaller edges don't get an outline.
		float sampleDepthDelta = (referenceDepth - sampleDepth) / sqrt(referenceDepth);
		
		// Create a bias's delta. It has an amount subtracted so that there's a minimum depth difference for any darkening to occur.
		float sampleDepthDeltaBias = max(0.0, sampleDepthDelta - min(10.0, 0.1 + sampleDepth / 5.0)); // By using the closer depth, we can get darker outlines on close-ups
		
		if (sampleDepthDeltaBias > 0.0)
		{
			float lineThickness = min(kMaxThickness, kMinThickness + sampleDepthDeltaBias / 1200.0);
			
			// If the sample distance is smaller than the line thickness, we darken.
			if (sampleWidth < lineThickness)
			{
				float sampleDarkening = min(100.0, sampleDepthDeltaBias);
				
				darkening = max(darkening, sampleDarkening);
			}
		}
	}
	
	vec4 pixelColor = texture(textureSamplerColor, v2f_texcoord0);
	
	vec3 pixelColorHSV = RGBtoHSV(pixelColor.rgb);
	
	bool bUseDarkerOutlines = pixelColorHSV.z > 0.1;
	vec3 outline0HSV = vec3(pixelColorHSV.x,
							bUseDarkerOutlines ? min(1.0, pixelColorHSV.y * 2.00 + 0.10)	: max(0.0, pixelColorHSV.y * 0.25),
							bUseDarkerOutlines ? min(0.5, pixelColorHSV.z * 0.7)			: min(1.0, pixelColorHSV.z * 1.5 + 0.2)
							);
	vec3 outline1HSV = vec3(pixelColorHSV.x,
							bUseDarkerOutlines ? min(1.0, pixelColorHSV.y * 2.50 + 0.25)	: max(0.0, pixelColorHSV.y * 0.10),
							bUseDarkerOutlines ? pixelColorHSV.z * 0.4 						: min(1.0, pixelColorHSV.z * 2.0 + 0.4)
							);
	
	// Darken first to a deeper color
	pixelColor.rgb = mix(pixelColor.rgb, HSVtoRGB(outline0HSV), clamp(darkening * 4.0, 0.0, 1.0));
	// Then darken to even DEEPER
	pixelColor.rgb = mix(pixelColor.rgb, HSVtoRGB(outline1HSV), clamp((darkening - 15.0) * 0.5, 0.0, 1.0));
	
	FragColor = pixelColor;
}