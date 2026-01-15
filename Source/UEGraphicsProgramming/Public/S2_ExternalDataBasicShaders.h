// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "ShaderParameterStruct.h"
#include "CommonStructs.h"


//Shader declaration

class UEGRAPHICSPROGRAMMING_API FS2L1_SingleThreadRingCS : public FGlobalShader
{
public:
	
	DECLARE_GLOBAL_SHADER(FS2L1_SingleThreadRingCS)
	
	SHADER_USE_PARAMETER_STRUCT(FS2L1_SingleThreadRingCS,FGlobalShader)
	
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
	SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, FS2L1_Output)
	SHADER_PARAMETER(int, Centre)
	SHADER_PARAMETER(int, CircleRadius)
	SHADER_PARAMETER(float, Deltatime)
	END_SHADER_PARAMETER_STRUCT()
	
	// Basic shader initialization
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	
	// Define environment variables used by compute shader
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		OutEnvironment.SetDefine(TEXT("THREADS_X"), 32);
		OutEnvironment.SetDefine(TEXT("THREADS_Y"), 1);
		OutEnvironment.SetDefine(TEXT("THREADS_Z"), 1);
	}
	
	//Shader run function

	static void FS2L1_SingleThreadRingCSRun(UTextureRenderTarget2D* OutputRenderTarget, const float inDeltatime);
	
};

//namespace CircleDataNamespace
//S{
	//Shader declaration

	class UEGRAPHICSPROGRAMMING_API FS2L2_CustomStructBufferCS : public FGlobalShader
	{
	public:
	
		DECLARE_GLOBAL_SHADER(FS2L2_CustomStructBufferCS)
	
		SHADER_USE_PARAMETER_STRUCT(FS2L2_CustomStructBufferCS,FGlobalShader)
	
		BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, FS2L2_Output)
		SHADER_PARAMETER(int, TextureResolution)
		SHADER_PARAMETER(float, Deltatime)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FCircleData>,CircleDataBuffer)
		END_SHADER_PARAMETER_STRUCT()
	
		// Basic shader initialization
		static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
		{
			return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
		}
	
	
	
		// Define environment variables used by compute shader
		static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,
			FShaderCompilerEnvironment& OutEnvironment)
		{
			int32 TotalCircleCount = 32;
			OutEnvironment.SetDefine(TEXT("THREADS_X"), TotalCircleCount);
			OutEnvironment.SetDefine(TEXT("THREADS_Y"), 1);
			OutEnvironment.SetDefine(TEXT("THREADS_Z"), 1);
		}
	
	
		//Init Buffer
		static TArray<FCircleData> InitCircleDataBuffer(int32 inTotalCircleCount = 32, int RenderTargetSize = 256);
	
		//Shader run function
		static void FS2L2_CustomStructBufferCSRun(UTextureRenderTarget2D* OutputRenderTarget, const float inDeltatime,TArray<FCircleData> inCircleDatBuffer );
	
	};
//}
