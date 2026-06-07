// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "ShaderParameterStruct.h"
#include "RHIGPUReadback.h"
#include "CommonStructs.h"
#include "Chaos/AABBTree.h"
#include "Chaos/AABBTree.h"
#include "Chaos/AABBTree.h"
#include "Chaos/AABBTree.h"
#include "Containers/Queue.h"



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
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,
		FShaderCompilerEnvironment& OutEnvironment)
	{
		OutEnvironment.SetDefine(TEXT("THREADS_X"), 32);
		OutEnvironment.SetDefine(TEXT("THREADS_Y"), 1);
		OutEnvironment.SetDefine(TEXT("THREADS_Z"), 1);
	}
	
	//Shader run function

	static void FS2L1_SingleThreadRingCSRun(UTextureRenderTarget2D* OutputRenderTarget, const float inDeltatime);
	
};


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
		
		static constexpr int ThreadsX = 32;

	
	
		// Define environment variables used by compute shader
		static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,
			FShaderCompilerEnvironment& OutEnvironment)
		{
			
			OutEnvironment.SetDefine(TEXT("THREADS_X"), ThreadsX);
			OutEnvironment.SetDefine(TEXT("THREADS_Y"), 1);
			OutEnvironment.SetDefine(TEXT("THREADS_Z"), 1);
		}
	
	
		//Init Buffer
		static TArray<FCircleData> InitCircleDataBuffer(int32 inTotalCircleCount = 32, int RenderTargetSize = 256);
	
		//Shader run function
		static void FS2L2_CustomStructBufferCSRun(UTextureRenderTarget2D* OutputRenderTarget, const float inDeltatime,
			TArray<FCircleData> inCircleDatBuffer, int CircleCount);
	
	};


//Shader declaration

class UEGRAPHICSPROGRAMMING_API FS2L3_CustomStructBufferReadBackCS : public FGlobalShader
{
public:
	
	DECLARE_GLOBAL_SHADER(FS2L3_CustomStructBufferReadBackCS)
	
	SHADER_USE_PARAMETER_STRUCT(FS2L3_CustomStructBufferReadBackCS,FGlobalShader)
	
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
	SHADER_PARAMETER(float, Deltatime)
	SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<FVector3f>,FS2L3StructDataBufferOutPut)
	END_SHADER_PARAMETER_STRUCT()
	
	// Basic shader initialization
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	
	static constexpr int ThreadsX = 64;
	
	// Define environment variables used by compute shader
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,
		FShaderCompilerEnvironment& OutEnvironment)
	{
		
		OutEnvironment.SetDefine(TEXT("THREADS_X"), ThreadsX);
		OutEnvironment.SetDefine(TEXT("THREADS_Y"), 1);
		OutEnvironment.SetDefine(TEXT("THREADS_Z"), 1);
	}
	
	

	
	//Shader run function
	static void FS2L3_CustomStructBufferReadBackCSRun(const float inDeltatime,
	                                                  const int inStarCount,
							TQueue<TUniquePtr<FRHIGPUBufferReadback>, EQueueMode::Spsc>& inFRHIGPUBufferReadbackQueue);
	
	
	static void PollReadBackandDownloadtoGameThread(TArray<FTransform>& StarTransformsArray, const int inStarCount,
	                        TQueue<TUniquePtr<FRHIGPUBufferReadback>, EQueueMode::Spsc>& inFRHIGPUBufferReadbackQueue);
	
	
	
private:
	
};

//FS2L4_BasicNoiseCS
class UEGRAPHICSPROGRAMMING_API FS2L4_BasicNoiseCS : public FGlobalShader
{
public:
	
	DECLARE_GLOBAL_SHADER(FS2L4_BasicNoiseCS)
	
	SHADER_USE_PARAMETER_STRUCT(FS2L4_BasicNoiseCS,FGlobalShader)
	
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
	SHADER_PARAMETER(float, Deltatime)
	SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, FS2L4_Output)
	SHADER_PARAMETER(int, TextureResolution)
	END_SHADER_PARAMETER_STRUCT()
	
	// Basic shader initialization
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	
	static constexpr int ThreadsX = 8;
	static constexpr int ThreadsY = 8;
	
	// Define environment variables used by compute shader
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,
		FShaderCompilerEnvironment& OutEnvironment)
	{
		
		OutEnvironment.SetDefine(TEXT("THREADS_X"), ThreadsX);
		OutEnvironment.SetDefine(TEXT("THREADS_Y"), ThreadsY);
		OutEnvironment.SetDefine(TEXT("THREADS_Z"), 1);
	}
	
	

	
	//Shader run function
	static void FS2L4_BasicNoiseCSRun(UTextureRenderTarget2D* OutputRenderTarget,const float inDeltatime);
	
};


//FS2L5_ProceduralWoodCS
class UEGRAPHICSPROGRAMMING_API FS2L5_ProceduralWoodCS : public FGlobalShader
{
public:
	
	DECLARE_GLOBAL_SHADER(FS2L5_ProceduralWoodCS)
	
	SHADER_USE_PARAMETER_STRUCT(FS2L5_ProceduralWoodCS,FGlobalShader)
	
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
	SHADER_PARAMETER(float, Deltatime)
	SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, FS2L5_Output)
	//SHADER_PARAMETER_STRUCT_REF can be used bekow
	SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FProceduralWoodData>, ProceduralWoodData)//Not using StructuredBuffer<> as we have one dimensional data.
	SHADER_PARAMETER(int, TextureResolution)
	END_SHADER_PARAMETER_STRUCT()
	
	// Basic shader initialization
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	
	static constexpr int ThreadsX = 8;
	static constexpr int ThreadsY = 8;
	
	// Define environment variables used by compute shader
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,
		FShaderCompilerEnvironment& OutEnvironment)
	{
		
		OutEnvironment.SetDefine(TEXT("THREADS_X"), ThreadsX);
		OutEnvironment.SetDefine(TEXT("THREADS_Y"), ThreadsY);
		OutEnvironment.SetDefine(TEXT("THREADS_Z"), 1);
	}
	
	
	//Shader run function
	static void FS2L5_ProceduralWoodCSRun(UTextureRenderTarget2D* OutputRenderTarget,
		const FProceduralWoodData inProceduralWoodData);
	
	/*//Basic implementation of SHADER_PARAMETER_STRUCT_REF (RENDER ONLY STRUCT)
	
	BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FProceduralWoodUniforms, )
	SHADER_PARAMETER(FLinearColor, PaleColor)
	SHADER_PARAMETER(FLinearColor, DarkColour)
	SHADER_PARAMETER(float, Frequency)
	SHADER_PARAMETER(float, NoiseScale)
	SHADER_PARAMETER(float, RingScale)
	SHADER_PARAMETER(float, Contrast)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FProceduralWoodUniforms,"ProceduralWoodDataUnif"); //"ProceduralWoodDataUnif" WILL BE USED IN hlsl
	
	Reference it in your shader parameters
	SHADER_PARAMETER_STRUCT_REF(FProceduralWoodUniforms, ProceduralWood)
	
	Create & fill the uniform buffer (render thread)
	FProceduralWoodUniforms WoodUniforms;
	WoodUniforms.PaleColor  = inProceduralWoodData.PaleColor;
	WoodUniforms.DarkColour = inProceduralWoodData.DarkColour;
	WoodUniforms.Frequency  = inProceduralWoodData.Frequency;
	WoodUniforms.NoiseScale = inProceduralWoodData.NoiseScale;
	WoodUniforms.RingScale  = inProceduralWoodData.RingScale;
	WoodUniforms.Contrast   = inProceduralWoodData.Contrast;

	FS2L5_Parameters->ProceduralWood =
		CreateUniformBufferImmediate(
			WoodUniforms,
			UniformBuffer_SingleDraw
		);
	
	HLSL side
	
	//use "ProceduralWoodDataUnif" directly*/
	
	
};


//FS2L6_ProceduralMarbleCS
class UEGRAPHICSPROGRAMMING_API FS2L6_ProceduralMarbleCS : public FGlobalShader
{
public:
	
	DECLARE_GLOBAL_SHADER(FS2L6_ProceduralMarbleCS)
	
	
	SHADER_USE_PARAMETER_STRUCT(FS2L6_ProceduralMarbleCS,FGlobalShader)
	
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
	SHADER_PARAMETER(uint32, Marble)
	SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, FS2L6_Output)
	SHADER_PARAMETER(int, TextureResolution)
	END_SHADER_PARAMETER_STRUCT()
	
	// Basic shader initialization
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	
	static constexpr int ThreadsX = 8;
	static constexpr int ThreadsY = 8;
	
	// Define environment variables used by compute shader
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,
		FShaderCompilerEnvironment& OutEnvironment)
	{
		
		OutEnvironment.SetDefine(TEXT("THREADS_X"), ThreadsX);
		OutEnvironment.SetDefine(TEXT("THREADS_Y"), ThreadsY);
		OutEnvironment.SetDefine(TEXT("THREADS_Z"), 1);
	}
	
	
	//Shader run function
	static void FS2L6_ProceduralMarbleCSRun(UTextureRenderTarget2D* OutputRenderTarget);
	
};





//Shader declaration

class UEGRAPHICSPROGRAMMING_API FS2L7_ReadbackDeformMeshCS : public FGlobalShader
{
public:
	
	DECLARE_GLOBAL_SHADER(FS2L7_ReadbackDeformMeshCS)
	
	SHADER_USE_PARAMETER_STRUCT(FS2L7_ReadbackDeformMeshCS,FGlobalShader)
	
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
	SHADER_PARAMETER(float, SphereRadius)
	SHADER_PARAMETER(float, Alpha)
	SHADER_PARAMETER_RDG_BUFFER_SRV(RWStructuredBuffer<Vector3f>,FS2L7_InitialMeshVerticeBuffer)
	SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<FVector3f>,FS2L7VerticesBufferOutput)
	END_SHADER_PARAMETER_STRUCT()
	
	// Basic shader initialization
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	
	static constexpr int ThreadsX = 64;
	
	// Define environment variables used by compute shader
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,
		FShaderCompilerEnvironment& OutEnvironment)
	{
		
		OutEnvironment.SetDefine(TEXT("THREADS_X"), ThreadsX);
		OutEnvironment.SetDefine(TEXT("THREADS_Y"), 1);
		OutEnvironment.SetDefine(TEXT("THREADS_Z"), 1);
	}
	
	

	
	//Shader run function
	static void FS2L7_ReadbackDeformMeshCSRun(const float inAlpha,
	                                          const int inVerticeCount,
	                                          TArray<FVector3f>& initialVerticesBuffer,
	                                          TQueue<TUniquePtr<FRHIGPUBufferReadback>, EQueueMode::Spsc>& inFRHIGPUBufferReadbackQueue);
	
	
	static void PollReadBackandDownloadtoGameThread(TArray<FVector3f>& VertReadbackArray, const int inVerticeCount,
	                                                TQueue<TUniquePtr<FRHIGPUBufferReadback>, EQueueMode::Spsc>& inFRHIGPUBufferReadbackQueue);
	

};