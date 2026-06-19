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
	SHADER_PARAMETER(float, AspectRatio)
	RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()


	BEGIN_SHADER_PARAMETER_STRUCT(FS3L1_BasicParticleEffectCSParametersStruct,)
	SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<FParticle>, FS3L1_ParticleBuffer)
	SHADER_PARAMETER(uint32, ParticleCount)
	SHADER_PARAMETER(FVector3f, Focus)
	SHADER_PARAMETER(float, Deltatime)
	SHADER_PARAMETER(float, OrbitSpeed)
	SHADER_PARAMETER(float, OrbitRadius)
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
												FVector3f Focus,
												const float inOrbitSpeed,
												const float inOrbitRadius);
	
	
	
	
	
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






//Everything the S3L2 boid simulation owns, held by one shared pointer.
//Shared ownership is the point: a dispatch or a draw enqueued this frame can still be in flight on
//the render thread after the actor has been destroyed, and it needs somewhere alive to land.
class FBoidSimulationProxy
{
public:
	//Ping pong pair - see the .usf for why the input cannot also be the output.
	TRefCountPtr<FRDGPooledBuffer> BoidBufferA;
	TRefCountPtr<FRDGPooledBuffer> BoidBufferB;

	//The boid mesh, de-indexed into a flat triangle list. Uploaded once at init.
	TRefCountPtr<FRDGPooledBuffer> MeshVertexBuffer;
	uint32 MeshTriangleCount = 0;

	//Which half of the pair holds the newest simulation output. Written by the dispatch and read
	//by the draw - both run on the render thread, so a plain bool is enough here.
	bool bLatestIsB = false;

	//Set once the buffers exist. The draw delegate fires every frame regardless of whether init
	//has finished, so it needs a cheap way to know there is nothing to draw yet.
	bool bBuffersReady = false;
};


BEGIN_SHADER_PARAMETER_STRUCT(FS3L2_InstancedMeshBoidsCSParametersStruct,)
	//Ping pong pair, see the .usf for why the input cannot also be the output.
	SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FBoid>, FS3L2_BoidsIn)
	SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<FBoid>, FS3L2_BoidsOut)
	SHADER_PARAMETER(uint32, BoidCount)
	SHADER_PARAMETER(float, Deltatime)
	SHADER_PARAMETER(FVector3f, Focus)
	SHADER_PARAMETER(float, NeighbourRadius)
	SHADER_PARAMETER(float, SeparationRadius)
	SHADER_PARAMETER(float, SeparationWeight)
	SHADER_PARAMETER(float, AlignmentWeight)
	SHADER_PARAMETER(float, CohesionWeight)
	SHADER_PARAMETER(float, ContainmentRadius)
	SHADER_PARAMETER(float, ContainmentWeight)
	SHADER_PARAMETER(float, MinSpeed)
	SHADER_PARAMETER(float, MaxSpeed)
	END_SHADER_PARAMETER_STRUCT()


BEGIN_SHADER_PARAMETER_STRUCT(FS3L2_InstancedMeshBoidsVSPSParametersStruct,)
	SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FBoidMeshVertex>, FS3L2_MeshVertexBuffer)
	SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FBoid>, FS3L2_BoidBuffer)
	SHADER_PARAMETER(FMatrix44f, WorldToClip)
	SHADER_PARAMETER(FVector3f, ActorLocation)
	SHADER_PARAMETER(FVector3f, BoidScale)
	SHADER_PARAMETER(uint32, BoidCount)
	SHADER_PARAMETER(FVector3f, LightDirection)
	SHADER_PARAMETER(FLinearColor, BoidColor)
	RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

//The boid rule tuning, kept in one struct so the actor can hand its whole tuning block over in a
//single argument instead of ten loose floats.
struct FS3L2_BoidSimulationSettings
{
	float NeighbourRadius = 600.f;
	float SeparationRadius = 200.f;
	float SeparationWeight = 20000.f;
	float AlignmentWeight = 3.f;
	float CohesionWeight = 1.f;
	float ContainmentRadius = 800.f;
	float ContainmentWeight = 4.f;
	float MinSpeed = 250.f;
	float MaxSpeed = 700.f;
	FVector3f Focus = FVector3f::ZeroVector;
};

//Shader declaration

class UEGRAPHICSPROGRAMMING_API FS3L2_InstancedMeshBoidsCS : public FGlobalShader
{
public:

	DECLARE_GLOBAL_SHADER(FS3L2_InstancedMeshBoidsCS)

	using FParameters = FS3L2_InstancedMeshBoidsCSParametersStruct;
	SHADER_USE_PARAMETER_STRUCT(FS3L2_InstancedMeshBoidsCS,FGlobalShader)

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


	//Allocates both halves of the ping pong pair, seeds the first one with a randomised flock, and
	//uploads the de-indexed boid mesh. inMeshVertices is a flat triangle list, so its length must
	//be a multiple of three.
	static void InitShaderData(TSharedPtr<FBoidSimulationProxy, ESPMode::ThreadSafe> inBoidSimulationProxy,
	                           uint32 BoidCount,
	                           float SpawnRadius,
	                           TArray<FBoidMeshVertex> inMeshVertices,
	                           FParticlesInitialised& inDelegate);

	//Shader run function. bReadFromA picks which half of the pair is this frame's input, the
	//actor flips it every tick.
	static void FS3L2_InstancedMeshBoidsCSRun(TSharedPtr<FBoidSimulationProxy, ESPMode::ThreadSafe> inBoidSimulationProxy,
	                                          const bool bReadFromA,
	                                          const float inDeltatime,
	                                          uint32 BoidCount,
	                                          const FS3L2_BoidSimulationSettings inSettings);

};


class FS3L2_InstancedMeshBoidsVS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FS3L2_InstancedMeshBoidsVS);

	using FParameters = FS3L2_InstancedMeshBoidsVSPSParametersStruct;
	SHADER_USE_PARAMETER_STRUCT(FS3L2_InstancedMeshBoidsVS, FGlobalShader);
};

class FS3L2_InstancedMeshBoidsPS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FS3L2_InstancedMeshBoidsPS);

	using FParameters = FS3L2_InstancedMeshBoidsVSPSParametersStruct;
	SHADER_USE_PARAMETER_STRUCT(FS3L2_InstancedMeshBoidsPS, FGlobalShader);

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

	//Draws every boid with ONE instanced draw call. Nothing here reads back to the CPU: the
	//simulation output stays in GPU memory and the vertex shader indexes it with SV_InstanceID.
	static void AddPass(
		FRDGBuilder& GraphBuilder,
		const FSceneView& View,
		FRDGTextureRef ColorTexture,
		FRDGTextureRef DepthTexture,
		const FIntRect& ViewportRect,
		const TSharedPtr<FBoidSimulationProxy, ESPMode::ThreadSafe>& inBoidSimulationProxy,
		uint32 BoidCount,
		FVector3f ActorLocation,
		FVector3f BoidScale,
		FVector3f LightDirection,
		FLinearColor BoidColor);

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
												FVector3f Focus,
												const float inOrbitSpeed,
												const float inOrbitRadius);
	
	
	
	
	

	
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