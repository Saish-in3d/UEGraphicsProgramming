// Fill out your copyright notice in the Description page of Project Settings.


#include "S3_ParticleEffectsShaders.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "Animation/InputScaleBias.h"
#include "Chaos/AABBTree.h"
#include "Chaos/AABBTree.h"
#include "Engine/TextureRenderTarget2D.h"


IMPLEMENT_GLOBAL_SHADER(FS3L1_BasicParticleEffectCS, "/Plugins/UEGraphicsProgramming/FS3L1_BasicParticleEffectCS.usf",
						"FS3L1_BasicParticleEffectCS", SF_Compute);


void FS3L1_BasicParticleEffectCS::InitShaderData(TRefCountPtr<FRDGPooledBuffer>& inPooledParticleBufffer,
	uint32 ParticleCount)
{
	
	//Passing &inPooledParticleBufffer as refrence so need to make sure that pooled particle buffer is safe on game thread
	ENQUEUE_RENDER_COMMAND(InitParticleBuffer)(
		[&inPooledParticleBufffer,ParticleCount](FRHICommandListImmediate& RHICmdList)
		{
			
			//iF IT bLOCKS RENDER THREAD, MOVE data creation loop on a worker thread or use parallel for
			
			
			/*Particle setting logic start*/
			TArray<FParticle> InitialParticleArray;
			InitialParticleArray.SetNum(ParticleCount);
			
			for (int i = 0 ;i < ParticleCount; i++)
			{
				FVector3f TempInitialParticleLocation = FVector3f();
				TempInitialParticleLocation.X = FMath::FRand() * 2.f - 1.f;
				TempInitialParticleLocation.Y = FMath::FRand() * 2.f - 1.f;
				TempInitialParticleLocation.Z = FMath::FRand() * 2.f - 1.f;
				
				TempInitialParticleLocation.Normalize();
				
				TempInitialParticleLocation *= FMath::FRand() * 0.5f;
				
				
				InitialParticleArray[i].Position.X = TempInitialParticleLocation.X;
				InitialParticleArray[i].Position.Y = TempInitialParticleLocation.Y;
				InitialParticleArray[i].Position.Z = TempInitialParticleLocation.Z + 3;
				
				InitialParticleArray[i].Velocity.X = 0;
				InitialParticleArray[i].Velocity.Y = 0;
				InitialParticleArray[i].Velocity.Z = 0;
				
				InitialParticleArray[i].Life = FMath::FRand();
				
			}
			/*Particle setting logic end*/
			
			
			//loading up data for FRDGPooledBuffer buffer in GPU
			
			//Creating buffer desc
			
			FRDGBufferDesc PooledParticleBufferDesc = FRDGBufferDesc::CreateStructuredDesc(sizeof(FParticle),
																						ParticleCount
																						);
			
			//Allocating space on GPU for buffer with random data
			AllocatePooledBuffer(PooledParticleBufferDesc,
								inPooledParticleBufffer,
								TEXT("ParticleEffectBuffer"),
								ERDGPooledBufferAlignment::None);			
			
			
			
			
			//Locking allocated space to make sure it is not accessable to other threads or gpy devices.
			void* Data =  RHICmdList.LockBuffer(inPooledParticleBufffer->GetRHI(),
				0,
				InitialParticleArray.GetTypeSize()* ParticleCount,
				RLM_WriteOnly
				);
			
			//Uploading/copying data from render thread memory context to GPU memory
			FMemory::Memcpy(
			Data,
			InitialParticleArray.GetData(),
			ParticleCount * sizeof(FParticle)
			);
			
			RHICmdList.UnlockBuffer(
			inPooledParticleBufffer->GetRHI()
			);
			
			
		});
	
	
}

void FS3L1_BasicParticleEffectCS::FS3L1_BasicParticleEffectCSRun(TRefCountPtr<FRDGPooledBuffer> inPooledParticleBufffer,
                                                                 const float inDeltatime,
                                                                 uint32 ParticleCount,
                                                                 FVector3f Focus)
{
	// Executing logic in render thread while passing OutputRender target to render thread via this lambda function
	ENQUEUE_RENDER_COMMAND(FS3L1_BasicParticleEffectCSRun)(
		[inPooledParticleBufffer,inDeltatime,ParticleCount,Focus](FRHICommandListImmediate& RHICmdList)
		{
			
			//Getting new graph builder each frame
			FRDGBuilder GraphBuilder(RHICmdList);
			
			//Registering Buffer in RDG 
			FRDGBufferRef FS3L1_ParticleBufferRDGRef = GraphBuilder.RegisterExternalBuffer(inPooledParticleBufffer,
																	TEXT("FS3L1_ParticleBufferRDGRef")
																	);
			
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
	
	// Executing logic in render thread while passing OutputRender target to render thread via this lambda function
	ENQUEUE_RENDER_COMMAND(FS3L1_BasicParticleEffectVSRun)(
		[inDeltatime,ParticleCount,Focus](FRHICommandListImmediate& RHICmdList)
		{
			//Getting new graph builder each frame
			FRDGBuilder GraphBuilder(RHICmdList);
			
			
			
			
		});
	
	
	
}
