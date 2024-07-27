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

#pragma once

class GLSLProgram;

namespace Attributes {
	// attribute indexes and GLSL names are the same for all shaders
	enum Index {
		Position  = 0,
		Normal	  = 2,
		Color	  = 3,
		TexCoord  = 8,
		Tangent	  = 9,
		Bitangent = 10,
		Count,
	};
};

enum VertexFormat {
	VF_REGULAR,		// idDrawVert
	VF_SHADOW,		// shadowCache_t
	VF_IMMEDIATE,	// ImmediateRendering::VertexData
	VF_COUNT,
};

/**
 * This is the only code which manages OpenGL's "Vertex Array Object" and vertex attributes format.
 * Also it supports overriding an attribute with a given constant value.
 * Vertex format and overrides are independent: you can mix changing them in any order.
 * 
 * Note that which VBOs to read is logically not the part of this class; it is handled by idVertexCache.
 * However, we can't use "separate attribute format" (GL 4.3),
 * so user must call UpdateVertexBuffers after VBO rebinds!
 */
class VertexArrayState {
public:
	VertexArrayState();
	~VertexArrayState();

	void Init();
	void Shutdown();

	// reset all settings to defaults
	void SetDefaultState();

	// respecify vertex attributes with currently bound VBOs
	// must be called: after VBO binds have changed, before any rendering
	void UpdateVertexBuffers();

	// set current vertex format
	void SetVertexFormat(VertexFormat format);
	// same as calling SetVertexFormat + UpdateVertexBuffers
	void SetVertexFormatAndUpdateVertexBuffers(VertexFormat format);

	// override some attribute value with given value (thus don't read attribute from buffer)
	// note: overrides are global state, they persist after vertex format of vertex buffer bind is changed
	void SetOverrideEnabled(Attributes::Index attrib, bool enabled);
	void SetOverrideValuef(Attributes::Index attrib, float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f);

	// connect GLSL program to attribute indexes
	// (called automatically for all GLSL programs)
	void BindAttributesToProgram(GLSLProgram *program);

private:
	void ApplyAttribEnabled(Attributes::Index attrib);

	VertexFormat vertexFormat;
	bool overriden[Attributes::Count];

	unsigned vao;

	// const data
	int formatAttribMasks[VF_COUNT];
	static const Attributes::Index allAttribsList[];
};

extern VertexArrayState vaState;
