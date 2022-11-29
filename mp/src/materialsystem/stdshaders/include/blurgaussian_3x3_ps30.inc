#pragma once
#include "shaderlib/cshader.h"
class blurgaussian_3x3_ps30_Static_Index
{
public:
	blurgaussian_3x3_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_blurgaussian_3x3_ps30 1


class blurgaussian_3x3_ps30_Dynamic_Index
{
public:
	blurgaussian_3x3_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_blurgaussian_3x3_ps30 1

