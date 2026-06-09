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



//Shader declaration

class UEGRAPHICSPROGRAMMING_API FS3L1_BasicParticleEffectCS : public FGlobalShader
{
public:
	
	DECLARE_GLOBAL_SHADER(FS3L1_BasicParticleEffectCS)
	
	SHADER_USE_PARAMETER_STRUCT(FS3L1_BasicParticleEffectCS,FGlobalShader)
	
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
	SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<FParticle>, FS3L1_ParticleBuffer)
	SHADER_PARAMETER(uint32, ParticleCount)
	SHADER_PARAMETER(FVector3f, Focus)
	SHADER_PARAMETER(float, Deltatime)
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
	
	
	static void InitShaderData(TRefCountPtr<FRDGPooledBuffer>& inPooledParticleBufffer,
								uint32 ParticleCount
								);
	
	//Shader run function

	static void FS3L1_BasicParticleEffectCSRun(TRefCountPtr<FRDGPooledBuffer> inPooledParticleBufffer,
												const float inDeltatime,
												uint32 ParticleCount,
												FVector3f Focus);
	
	static void FS3L1_BasicParticleEffectVSRun(TArray<FParticle> inParticleArray,
												const float inDeltatime,
												uint32 ParticleCount,
												FVector3f Focus);
};

