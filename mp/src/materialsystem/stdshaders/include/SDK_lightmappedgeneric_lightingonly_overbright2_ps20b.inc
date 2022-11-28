#pragma once
#include "shaderlib/cshader.h"
class SDK_lightmappedgeneric_lightingonly_overbright2_ps20b_Static_Index
{
public:
	SDK_lightmappedgeneric_lightingonly_overbright2_ps20b_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_SDK_lightmappedgeneric_lightingonly_overbright2_ps20b 1


class SDK_lightmappedgeneric_lightingonly_overbright2_ps20b_Dynamic_Index
{
public:
	SDK_lightmappedgeneric_lightingonly_overbright2_ps20b_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_SDK_lightmappedgeneric_lightingonly_overbright2_ps20b 1

