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
#version 330 core
#extension GL_ARB_texture_query_lod: enable

#pragma tdm_include "tdm_lightproject.glsl"
#pragma tdm_include "tdm_interaction.glsl"
#pragma tdm_include "tdm_compression.glsl"
#pragma tdm_include "tdm_parallax.glsl"

in vec2 var_TexDiffuse;
in vec2 var_TexSpecular;
in vec2 var_TexNormal;
in vec2 var_TexCoord;
in vec4 var_TexLight;
in vec4 var_Color;
in mat3 var_TangentBitangentNormalMatrix;
in vec3 var_worldViewDir;
in vec3 var_LightDirLocal;
in vec3 var_ViewDirLocal;

out vec4 FragColor;

uniform sampler2D u_normalTexture;
uniform sampler2D u_diffuseTexture;
uniform sampler2D u_specularTexture;
uniform sampler2D u_parallaxTexture;

uniform sampler2D u_lightProjectionTexture;
uniform sampler2D u_lightFalloffTexture;
uniform bool u_cubic;
uniform samplerCube u_lightProjectionCubemap;   // TODO: is this needed?
uniform bool u_useNormalIndexedDiffuse, u_useNormalIndexedSpecular;
uniform samplerCube u_lightDiffuseCubemap;
uniform samplerCube u_lightSpecularCubemap;

uniform float u_gamma, u_minLevel;

uniform vec2 u_renderResolution;
uniform sampler2D u_ssaoTexture;
uniform int u_ssaoEnabled;

uniform vec4 u_lightTextureMatrix[2];
uniform vec4 u_diffuseColor;
uniform vec4 u_specularColor;
uniform vec4 u_hasTextureDNSP;
uniform float u_RGTC;
uniform mat4 u_modelMatrix;

uniform vec2 u_parallaxHeightScale;
uniform ivec3 u_parallaxIterations;
uniform float u_parallaxGrazingAngle;

void main() {
	vec4 diffuseTexColor, specularTexColor, normalTexColor;

	if (u_hasTextureDNSP[3] != 0.0) {
		vec3 offset = computeParallaxOffset(
			u_parallaxTexture, u_parallaxHeightScale,
			var_TexCoord, var_ViewDirLocal,
			u_parallaxGrazingAngle, u_parallaxIterations.xy
		);
		vec2 texDiffuse = var_TexDiffuse + offset.xy;
		vec2 texSpecular = var_TexSpecular + offset.xy;
		vec2 texNormal = var_TexNormal + offset.xy;

		// use original gradients to avoid artifacts on relief silhouette
		vec2 derTcX = dFdx(var_TexCoord);
		vec2 derTcY = dFdy(var_TexCoord);
		diffuseTexColor = textureGrad(u_diffuseTexture, texDiffuse, derTcX, derTcY);
		specularTexColor = textureGrad(u_specularTexture, texSpecular, derTcX, derTcY);
		normalTexColor = textureGrad(u_normalTexture, texNormal, derTcX, derTcY);
	}
	else {
		diffuseTexColor = texture(u_diffuseTexture, var_TexDiffuse);
		specularTexColor = texture(u_specularTexture, var_TexSpecular);
		normalTexColor = texture(u_normalTexture, var_TexNormal);
	}

	vec3 lightColor;
	if (u_cubic)
		lightColor = projFalloffOfCubicLight(u_lightProjectionCubemap, var_TexLight).rgb;
	else
		lightColor = projFalloffOfNormalLight(u_lightProjectionTexture, u_lightFalloffTexture, u_lightTextureMatrix, var_TexLight).rgb;

	vec3 localNormal = unpackSurfaceNormal(normalTexColor, u_hasTextureDNSP[1] != 0.0, u_RGTC != 0.0);
	AmbientGeometry props = computeAmbientGeometry(var_worldViewDir, localNormal, var_TangentBitangentNormalMatrix, mat3(u_modelMatrix));

	vec4 interactionColor = computeAmbientInteraction(
		props,
		u_diffuseColor.rgb, diffuseTexColor,
		u_specularColor.rgb, specularTexColor,
		var_Color.rgb,
		u_useNormalIndexedDiffuse, u_useNormalIndexedSpecular, u_lightDiffuseCubemap, u_lightSpecularCubemap,
		u_minLevel, u_gamma
	);

	float ssao = 1;
	if (u_ssaoEnabled == 1) {
		ssao = texture(u_ssaoTexture, gl_FragCoord.xy / u_renderResolution).r;
	}

	FragColor = vec4(lightColor * ssao * interactionColor.rgb, interactionColor.a);
}
