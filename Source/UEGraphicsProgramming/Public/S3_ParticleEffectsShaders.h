// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "ShaderParameterStruct.h"
#include "CommonStructs.h"
#include "RHIGPUReadback.h"
#include "CommonStructs.h"
#include "Chaos/AABBTree.h"
#include "Chaos/AABBTree.h"
#include "Chaos/AABBTree.h"
#include "Chaos/AABBTree.h"
#include "Containers/Queue.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FParticlesInitialised, int32);
	
	
BEGIN_SHADER_PARAMETER_STRUCT(FS3L1_BasicParticleEffectPSVSParametersStruct,)
	SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FParticle>, FS3L1_ParticleBuffer)
	SHADER_PARAMETER(FMatrix44f, WorldToClip)
	SHADER_PARAMETER(float, PointSize)
	SHADER_PARAMETER(uint32, ParticleCount)
	SHADER_PARAMETER(FVector3f, ActorLocation)
	SHADER_PARAMETER_TEXTURE(Texture2D, ParticleTexture)
	SHADER_PARAMETER_SAMPLER(SamplerState, ParticleSampler)
	RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()


	BEGIN_SHADER_PARAMETER_STRUCT(FS3L1_BasicParticleEffectCSParametersStruct,)
	SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<FParticle>, FS3L1_ParticleBuffer)
	SHADER_PARAMETER(uint32, ParticleCount)
	SHADER_PARAMETER(FVector3f, Focus)
	SHADER_PARAMETER(float, Deltatime)
	END_SHADER_PARAMETER_STRUCT()

//Shader declaration

class UEGRAPHICSPROGRAMMING_API FS3L1_BasicParticleEffectCS : public FGlobalShader
{
public:
	
	DECLARE_GLOBAL_SHADER(FS3L1_BasicParticleEffectCS)
	
	using FParameters = FS3L1_BasicParticleEffectCSParametersStruct;
	SHADER_USE_PARAMETER_STRUCT(FS3L1_BasicParticleEffectCS,FGlobalShader)
	

	
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
	
	
	static void InitShaderData(TRefCountPtr<FRDGPooledBuffer>& inPooledParticleBufffer,
	                           uint32 ParticleCount,
	                           FParticlesInitialised& inDelegate, FVector3d ActorLocation
	);
	
	//Shader run function

	static void FS3L1_BasicParticleEffectCSRun(TRefCountPtr<FRDGPooledBuffer> inPooledParticleBufffer,
												const float inDeltatime,
												uint32 ParticleCount,
												FVector3f Focus);
	
	
	
	
	
	static void FS3L1_BasicParticleEffectVSRun(TRefCountPtr<FRDGPooledBuffer> inPooledParticleBufffer,
												const float inDeltatime,
												uint32 ParticleCount,
												FVector3f Focus);
	
};

class FS3L1_BasicParticleEffectVS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FS3L1_BasicParticleEffectVS);
	
	/*FS3L1_BasicParticleEffectVS() = default;
	FS3L1_BasicParticleEffectVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{}*/
	
	using FParameters = FS3L1_BasicParticleEffectPSVSParametersStruct;
	SHADER_USE_PARAMETER_STRUCT(FS3L1_BasicParticleEffectVS, FGlobalShader);
};

class FS3L1_BasicParticleEffectPS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FS3L1_BasicParticleEffectPS);
	
	using FParameters = FS3L1_BasicParticleEffectPSVSParametersStruct;
	SHADER_USE_PARAMETER_STRUCT(FS3L1_BasicParticleEffectPS, FGlobalShader);
	
	
	
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return !IsMobilePlatform(Parameters.Platform);
	}
	
	static void ModifyCompilationEnvironment(
	   const FGlobalShaderPermutationParameters& Parameters,
	   FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}
	
	static void AddPass(
		FRDGBuilder& GraphBuilder,
		const FSceneView& View,
		FRDGTextureRef ColorTexture,
		FRDGTextureRef DepthTexture,
		const FIntRect& ViewportRect,
		const TRefCountPtr<FRDGPooledBuffer>& PooledParticleBuffer,
		int32 ParticleCount,
		float PointSize,
		FVector3f ActorLocation,
		FRHITexture* ParticleTextureRHI);
	
	
};






//Shader declaration

class UEGRAPHICSPROGRAMMING_API FS3L0_BasicPointParticleEffectCS : public FGlobalShader
{
public:
	
	DECLARE_GLOBAL_SHADER(FS3L0_BasicPointParticleEffectCS)
	
	using FParameters = FS3L1_BasicParticleEffectCSParametersStruct;
	SHADER_USE_PARAMETER_STRUCT(FS3L0_BasicPointParticleEffectCS,FGlobalShader)
	

	
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
	
	
	static void InitShaderData(TRefCountPtr<FRDGPooledBuffer>& inPooledParticleBufffer,
	                           uint32 ParticleCount,
	                           FParticlesInitialised& inDelegate, FVector3d ActorLocation
	);
	
	//Shader run function

	static void FS3L0_BasicPointParticleEffectCSRun(TRefCountPtr<FRDGPooledBuffer> inPooledParticleBufffer,
												const float inDeltatime,
												uint32 ParticleCount,
												FVector3f Focus);
	
	
	
	
	

	
};

class FS3L0_BasicParticlePointEffectVS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FS3L0_BasicParticlePointEffectVS);

	using FParameters = FS3L1_BasicParticleEffectPSVSParametersStruct;
	SHADER_USE_PARAMETER_STRUCT(FS3L0_BasicParticlePointEffectVS, FGlobalShader);
};

class FS3L0_BasicParticlePointEffectPS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FS3L0_BasicParticlePointEffectPS);
	
	using FParameters = FS3L1_BasicParticleEffectPSVSParametersStruct;
	SHADER_USE_PARAMETER_STRUCT(FS3L0_BasicParticlePointEffectPS, FGlobalShader);
	
	
	
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return !IsMobilePlatform(Parameters.Platform);
	}
	
	static void ModifyCompilationEnvironment(
	   const FGlobalShaderPermutationParameters& Parameters,
	   FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}
	
	static void FS3L0_BasicPointParticleEffectCSVSRun(
													FRDGBuilder& GraphBuilder,
													const FSceneView& View,
													FRDGTextureRef ColorTexture,
													FRDGTextureRef DepthTexture,
													const FIntRect& ViewportRect,
													const TRefCountPtr<FRDGPooledBuffer>& PooledParticleBuffer,
													int32 ParticleCount,
													float PointSize, FVector3f ActorLocation);
	
	
};