/*****************************************************************************
The Dark Mod GPL Source Code

This file is part of the The Dark Mod Source Code, originally based
on the Doom 3 GPL Source Code as published in 2011.

The Dark Mod Source Code is free software: you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation, either version 3 of the License,
or (at your option) any later version. For details, see LICENSE.TXT.

Project: The Dark Mod (http://www.thedarkmod.com/)

******************************************************************************/
// Contains common formulas for computing interaction.
// Includes: illumination model, fetching surface and light properties
// Excludes: shadows

#pragma tdm_include "tdm_lightproject.glsl"
#pragma tdm_include "tdm_interaction.glsl"
#pragma tdm_include "tdm_compression.glsl"
#pragma tdm_include "tdm_parallax.glsl"

in vec2 var_TexDiffuse;
in vec2 var_TexNormal;
in vec2 var_TexSpecular;
in vec2 var_TexCoord;
in vec4 var_TexLight;
in vec4 var_Color;
in mat3 var_TangentBitangentNormalMatrix; 
in vec3 var_LightDirLocal;
in vec3 var_ViewDirLocal;

uniform sampler2D u_normalTexture;
uniform sampler2D u_diffuseTexture;
uniform sampler2D u_specularTexture;
uniform sampler2D u_parallaxTexture;

uniform sampler2D u_lightFalloffTexture;
uniform sampler2D u_lightProjectionTexture;
uniform bool u_cubic;
uniform samplerCube	u_lightProjectionCubemap;

uniform bool u_shadows;
uniform int u_softShadowsQuality;
uniform float u_softShadowsRadius;

uniform mat4 u_modelViewMatrix;
uniform mat4 u_modelMatrix;
uniform mat4 u_projectionMatrix;
uniform vec4 u_lightTextureMatrix[2];
uniform vec4 u_diffuseColor;
uniform vec4 u_specularColor;
uniform vec4 u_hasTextureDNSP;
uniform int u_useBumpmapLightTogglingFix;
uniform float u_RGTC;

uniform vec2 u_parallaxHeightScale;
uniform ivec3 u_parallaxIterations;
uniform float u_parallaxGrazingAngle;
uniform float u_parallaxShadowSoftness;


vec3 computeInteraction(out InteractionGeometry props) {
	vec2 texDiffuse = var_TexDiffuse;
	vec2 texSpecular = var_TexSpecular;
	vec2 texNormal = var_TexNormal;
	float parallaxSelfShadow = 1.0;
	if (u_hasTextureDNSP[3] != 0.0) {
		vec3 offset = computeParallaxOffset(
			u_parallaxTexture, u_parallaxHeightScale,
			var_TexCoord, var_ViewDirLocal,
			u_parallaxGrazingAngle, u_parallaxIterations.xy
		);
		texDiffuse += offset.xy;
		texSpecular += offset.xy;
		texNormal += offset.xy;
		parallaxSelfShadow = computeParallaxShadow(
			u_parallaxTexture, u_parallaxHeightScale,
			vec3(var_TexCoord, 0.0) + offset, var_LightDirLocal,
			u_parallaxIterations.z, u_parallaxShadowSoftness
		);
	}

	vec3 lightColor;
	if (u_cubic)
		lightColor = projFalloffOfCubicLight(u_lightProjectionCubemap, var_TexLight).rgb;
	else
		lightColor = projFalloffOfNormalLight(u_lightProjectionTexture, u_lightFalloffTexture, u_lightTextureMatrix, var_TexLight).rgb;

	vec3 localNormal = fetchSurfaceNormal(texNormal, u_hasTextureDNSP[1] != 0.0, u_normalTexture, u_RGTC != 0.0);
	props = computeInteractionGeometry(var_LightDirLocal, var_ViewDirLocal, localNormal);

	vec3 interactionColor = computeAdvancedInteraction(
		props,
		u_diffuseTexture, u_diffuseColor.rgb, texDiffuse,
		u_specularTexture, u_specularColor.rgb, texSpecular,
		var_Color.rgb,
		u_useBumpmapLightTogglingFix != 0
	);

	return interactionColor * lightColor * parallaxSelfShadow;
}
