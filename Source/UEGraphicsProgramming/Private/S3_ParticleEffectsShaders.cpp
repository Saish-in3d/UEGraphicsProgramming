// Fill out your copyright notice in the Description page of Project Settings.


#include "S3_ParticleEffectsShaders.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "Chaos/AABBTree.h"
#include "Chaos/AABBTree.h"
#include "Engine/TextureRenderTarget2D.h"


IMPLEMENT_GLOBAL_SHADER(FS3L1_BasicParticleEffectCS, "/Plugins/UEGraphicsProgramming/FS3L1_BasicParticleEffectCS.usf",
						"FS3L1_BasicParticleEffectCS", SF_Compute);


void FS3L1_BasicParticleEffectCS::InitShaderData(TRefCountPtr<FRDGPooledBuffer> inPooledParticleBufffer,
	uint32 ParticleCount)
{
	ENQUEUE_RENDER_COMMAND(InitParticleBuffer)(
		[inPooledParticleBufffer,ParticleCount](FRHICommandListImmediate& RHICmdList)
		{
			//iF IT bLOCKS RENDER THREAD, MOVE data creation loop on a worker thread or use parallel for
			
			
			/*Particle setting logic start*/
			TArray<FParticle> InitialParticleArray;
			InitialParticleArray.SetNum(ParticleCount);
			
			//Assign values for each individual element
			
			/*Particle setting logic end*/
			
			
			//loading up data for FRDGPooledBuffer buffer in GPU
			
			//Creating buffer desc
			
			FRDGBufferDesc PooledParticleBufferDesc = FRDGBufferDesc::CreateBufferDesc(sizeof(FParticle),
																						ParticleCount
																						);
			
			//Actually creating a pooled buffer 
			
			RHICmdList.LockBuffer(inPooledParticleBufffer->GetRHI(),
				0,
				InitialParticleArray.GetTypeSize()* ParticleCount,
				RLM_WriteOnly
				);
			
			
		});
	
	
}

void FS3L1_BasicParticleEffectCS::FS3L1_BasicParticleEffectCSRun(TArray<FParticle> inParticleArray,
                                                                 const float inDeltatime, uint32 ParticleCount, FVector3f Focus)
{
	// Executing logic in render thread while passing OutputRender target to render thread via this lambda function
	ENQUEUE_RENDER_COMMAND(DispatchMyCompute)(
		[inParticleArray,inDeltatime,ParticleCount,Focus](FRHICommandListImmediate& RHICmdList)
		{
			
			//Getting new graph builder each frame
			FRDGBuilder GraphBuilder(RHICmdList);
			
			//Registering render target in RDG 
			FRDGBufferRef FS3L1_ParticleBufferRDGRef = CreateStructuredBuffer(GraphBuilder,
				TEXT("FS3L1_ParticleBufferRDGRef"),
				sizeof(FParticle),
				ParticleCount,
				&inParticleArray,
				sizeof(FParticle));
			
			//ShaderParameterSetup
			FS3L1_BasicParticleEffectCS::FParameters* FS3L1_Parameters = GraphBuilder.AllocParameters<
				FS3L1_BasicParticleEffectCS::FParameters>();
			
			FS3L1_Parameters->FS3L1_ParticleBuffer =  GraphBuilder.CreateUAV(FS3L1_ParticleBufferRDGRef)  ; //Compute Shader directly writes on this RT
			FS3L1_Parameters->ParticleCount = ParticleCount;
			FS3L1_Parameters->Focus = Focus; //Assuming that RT is a squaree
			FS3L1_Parameters->Deltatime = inDeltatime;
			
			
			//Dispatching shader
			//Initialising compute shader object
			TShaderMapRef<FS3L1_BasicParticleEffectCS> FS3L1_ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
			
			//Setting up Group Count
			FIntVector GroupCount = FIntVector(ParticleCount+ ThreadsX - 1/ThreadsX,1,1);
			
			//Actually dispatching the shader
			
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FS3L1"),
				FS3L1_ComputeShader,
				FS3L1_Parameters,
				GroupCount
				);
			
			GraphBuilder.Execute();
			
		}
	);
	
	
	
	
}

void FS3L1_BasicParticleEffectCS::FS3L1_BasicParticleEffectVSRun(TArray<FParticle> inParticleArray,
	const float inDeltatime, uint32 ParticleCount, FVector3f Focus)
{
	
	FPostOpaqueRenderParameters RenderParameters;
	
	RenderParameters;
	
	
}
