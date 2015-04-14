/*****************************************************************************
	The Dark Mod GPL Source Code

	This file is part of the The Dark Mod Source Code.

	The Dark Mod Source Code is free software: you can redistribute it
	and/or modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version. For details, see LICENSE.TXT.

	Project: The Dark Mod (http://www.thedarkmod.com/)

	$Revision$ (Revision of last commit)
	$Date$ (Date of last commit)
	$Author$ (Author of last commit)

	******************************************************************************/

#ifndef __SOFT_SHADOWS__
#define __SOFT_SHADOWS__



/*  ---+-+-+-+-+-+-+-+|  SOFT SHADOW RESOURCE MANAGER  |+-+-+-+-+-+-+-+---  */

class SoftShadowManager {
public:
	SoftShadowManager();										// Does not init resources, as gl context probably won't be valid yet
	~SoftShadowManager() { UnInit(); }

private:
	SoftShadowManager(const SoftShadowManager&);				// disallow copying
	SoftShadowManager& operator=(const SoftShadowManager&);		// disallow copying

public:
	void		NewFrame();										// Call before use in each new frame. Initializes resources.
	void		SetLightPosition( const idVec4* pos );
	void		SetDepthNormalVertexAttribArray( const float* ptr );
	void		DepthPass( drawSurf_t **drawSurfs, int numDrawSurfs ); // Wraps usual depth pass.
	void		DrawInteractions( const viewLight_t* vLight );
	void		UnInit();										// Releases all resources
	
	void		DrawDebugOutput( const viewLight_t* vLight );

	static const int MINISCALE = 8;								// Scale of image downsampling for the penumbra-spread technique.
	static const int JITTERMAPSIZE = 64;

private:
	void		Init();
	void		InitRenderTargets();
	void		InitShaders();
	void		InitFBOs();
	void		InitVBOs();
	GLuint		CreateShader( GLuint type, const GLchar* src );
	GLuint		CreateShaderProg( GLuint vp, GLuint fp );

	idVec2		MaxTexcoord( const bool powerOfTwo ) const;
	void		DrawQuad( idImage* tex, const GLuint vertexLoc );
	void		ResetLightScissor( const viewLight_t* vLight );
	void		MakeShadowStencil( const viewLight_t* vLight, const drawSurf_s* shadows, const bool clearStencil );

	int			width, height;									// Screen/viewport dimensions
	int			potWidth, potHeight;							// Enlarged to power-of-two
	int			smallwidth, smallheight;						// Smaller screensize used by the penumbra-spread technique
	bool		initialized;
	uint		lastUsedSpreadTarget;
	bool		spamConsole;
	uint		localShadowDrawCounter;							// Soft shadow passes for shadows cast by no-self-shadow models
	uint		globalShadowDrawCounter;						// Soft shadow passes for global shadows, i.e. cast by self-shadowing models
	uint		lightCounter;

	/* Resources */	
	// Frame buffers
	enum {
		normal_fb,
		penumbraSize_fb,
		colorStencil_fb,
		penumbraSpread_fb,
		shadowBlur_fb,
		NumFramebuffers
	};
	GLuint fbo[NumFramebuffers];

	// Render targets
	enum {
		normal_tx,
		penumbraSize_tx,
		colorStencil_tx,
		penumbraSpread1_tx,
		penumbraSpread2_tx,
		shadowBlur_tx,
		jitterMap_tx,
		NumTextures
	};
	idImage* tex[NumTextures];
	GLuint depthRbo;

	// Shaders and programs
	enum {
		quad_vp,
		quad_fp,
		normal_vp,
		normal_fp,
		shadow_vp,
		shadow_fp,
		mini_vp,
		mini_fp, 
		minp_vp,
		minp_fp,
		spread_vp,
		spread_fp,
		blur_vp,
		blur_fp,
		copyback_fp,
		avg_vp,
		avg_fp,
		NumShaders
	};
	GLuint shaders[NumShaders];
	enum {
		quad_pr,
		normal_pr,
		stencilShadow_pr,
		mini_pr,
		minp_pr,
		spread_pr,
		blur_pr,
		copyback_pr,
		avg_pr,
		NumGLSLPrograms
	};
	GLuint glslProgs[NumGLSLPrograms];

	// Uniform / attribute locations
	GLuint		UNF_QUAD_pos;		// Vertex data
	GLuint		UNF_NORM_normal;	// Vertex data
	GLuint		UNF_SHADOW_lightPos;
	GLuint		UNF_SHADOW_lightRadius;
	GLuint		UNF_SHADOW_lightReach;
	GLuint		UNF_SHADOW_invDepthImageSize;
	GLuint		UNF_SHADOW_threshold; 
	GLuint		UNF_SPREAD_PIXELRANGE;
	GLuint		UNF_MINI_pos;
	GLuint		UNF_SPREAD_pos;
	GLuint		UNF_SPREAD_amount;
	GLuint		UNF_BLUR_pos;
	GLuint		UNF_BLUR_projMatrix;
	GLuint		UNF_BLUR_lightScissor;
	GLuint		UNF_COPYBACK_pos;	
	GLuint		UNF_AVG_pos;
	GLuint		UNF_MINP_pos;

	
	// VBOs for drawing screen quads
	vertCache_t* ScreenQuadVerts;
	vertCache_t* ScreenQuadIndexes;
};

extern SoftShadowManager* softShadowMgr;

#endif // !__SOFT_SHADOWS__