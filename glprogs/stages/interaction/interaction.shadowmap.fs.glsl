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
#extension GL_ARB_texture_gather: enable
#extension GL_ARB_texture_query_lod: enable

#define STGATILOV_USEGATHER 1

out vec4 FragColor;

#pragma tdm_include "stages/interaction/interaction.common.fs.glsl"
#define TDM_allow_ARB_texture_gather STGATILOV_USEGATHER
#pragma tdm_include "tdm_shadowmaps.glsl"

uniform bool 	u_shadowMapCullFront;
uniform vec4	u_shadowRect;
uniform sampler2D u_shadowMap;
uniform bool	u_parallaxOffsetExternalShadows;

in vec3 var_LightDirWorld;

void main() {
	InteractionGeometry props;
	vec3 worldParallax;
	FragColor.rgb = computeInteraction(props, worldParallax);

	vec3 worldNormal = mat3(u_modelMatrix) * var_TangentBitangentNormalMatrix[2];

	if (u_shadows) {
		vec3 dirToLight = -var_LightDirWorld;
		if (u_parallaxOffsetExternalShadows)
			dirToLight += worldParallax;
		float shadowsCoeff = computeShadowMapCoefficient(
			dirToLight, worldNormal,
			u_shadowMap, u_shadowRect,
			u_softShadowsQuality, u_softShadowsRadius, u_shadowMapCullFront
		);
		FragColor.rgb *= shadowsCoeff;
	}
	FragColor.a = 1.0;
}
