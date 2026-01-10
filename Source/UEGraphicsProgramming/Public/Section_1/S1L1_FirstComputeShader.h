// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "RenderGraphUtils.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "ShaderParameterStruct.h"

/**
 * 
 */

//Shader declaration

class UEGRAPHICSPROGRAMMING_API FS1L1_FirstComputeShaderCS : public FGlobalShader
{
	public:
	
	DECLARE_GLOBAL_SHADER(FS1L1_FirstComputeShaderCS)
	
	SHADER_USE_PARAMETER_STRUCT(FS1L1_FirstComputeShaderCS,FGlobalShader)
	
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
	SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, FS1L1_Output)
	END_SHADER_PARAMETER_STRUCT()
	
	// Basic shader initialization
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	
	// Define environment variables used by compute shader
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		OutEnvironment.SetDefine(TEXT("THREADS_X"), 8);
		OutEnvironment.SetDefine(TEXT("THREADS_Y"), 8);
		OutEnvironment.SetDefine(TEXT("THREADS_Z"), 1);
	}
	
	//Shader run function

	static void FS1L1_FirstCSRun(UTextureRenderTarget2D* OutputRenderTarget);
	
};






