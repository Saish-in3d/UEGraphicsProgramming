// Fill out your copyright notice in the Description page of Project Settings.


#include "S3_ParticleEffectsShaders.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"


#include "PipelineStateCache.h"      // SetGraphicsPipelineState
#include "CommonRenderResources.h"   // GEmptyVertexDeclaration
#include "RHIStaticStates.h"         // TStaticBlendState / RasterizerState / DepthStencilState
#include "ShaderParameterStruct.h"   // SetShaderParameters

#include "Animation/InputScaleBias.h"
#include "Chaos/AABBTree.h"
#include "Chaos/AABBTree.h"
#include "Engine/TextureRenderTarget2D.h"





void FS3L1_BasicParticleEffectCS::InitShaderData(TRefCountPtr<FRDGPooledBuffer>& inPooledParticleBufffer,
                                                 uint32 ParticleCount, FParticlesInitialised& inDelegate, FVector3d ActorLocation)
{
	
	//Passing &inPooledParticleBufffer as refrence so need to make sure that pooled particle buffer is safe on game thread
	ENQUEUE_RENDER_COMMAND(InitParticleBuffer)(
		[&inPooledParticleBufffer,ParticleCount,inDelegate,ActorLocation](FRHICommandListImmediate& RHICmdList)
		{
			
			//iF IT bLOCKS RENDER THREAD, MOVE data creation loop on a worker thread or use parallel for
			
			
			/*Particle setting logic start*/
			TArray<FParticle> InitialParticleArray;
			InitialParticleArray.SetNum(ParticleCount);
			
			for (uint32 i = 0 ;i < ParticleCount; i++)
			{
				FVector3f TempInitialParticleLocation = FVector3f()  ;
				TempInitialParticleLocation.X = FMath::FRand() * 2.f - 1.f;
				TempInitialParticleLocation.Y = FMath::FRand() * 2.f - 1.f;
				TempInitialParticleLocation.Z = FMath::FRand() * 2.f - 1.f;
				
				TempInitialParticleLocation.Normalize();
				
				TempInitialParticleLocation *= FMath::FRand() * 0.5f;
				
				
				InitialParticleArray[i].Position.X = TempInitialParticleLocation.X;
				InitialParticleArray[i].Position.Y = TempInitialParticleLocation.Y;
				InitialParticleArray[i].Position.Z = TempInitialParticleLocation.Z + 3;
				
				//InitialParticleArray[i].Position += FVector3f(ActorLocation);
				
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
			
			AsyncTask(ENamedThreads::GameThread,[inDelegate]
			{
				inDelegate.Broadcast(100);
			});
			
			
		});
	
	
}

IMPLEMENT_GLOBAL_SHADER(FS3L1_BasicParticleEffectCS, "/Plugins/UEGraphicsProgramming/FS3L1_BasicParticleEffectCS.usf",
						"FS3L1_BasicParticleEffectCS", SF_Compute);

void FS3L1_BasicParticleEffectCS::FS3L1_BasicParticleEffectCSRun(TRefCountPtr<FRDGPooledBuffer> inPooledParticleBufffer,
                                                                 const float inDeltatime,
                                                                 uint32 ParticleCount,
                                                                 FVector3f Focus)
{
	if (!inPooledParticleBufffer.IsValid()){return;}
	
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

void FS3L1_BasicParticleEffectCS::FS3L1_BasicParticleEffectVSRun(TRefCountPtr<FRDGPooledBuffer> inPooledParticleBufffer,
	const float inDeltatime, uint32 ParticleCount, FVector3f Focus)
{
	/*if(!IsInRenderingThread()){return;}
	
	
	// Executing logic in render thread while passing OutputRender target to render thread via this lambda function
	ENQUEUE_RENDER_COMMAND(FS3L1_BasicParticleEffectVSRun)(
		[inPooledParticleBufffer,inDeltatime,ParticleCount,Focus](FRHICommandListImmediate& RHICmdList)
		{
			
			if (!inPooledParticleBufffer.IsValid()){return;}
			
			//Getting new graph builder each frame
			FRDGBuilder GraphBuilder(RHICmdList);
			const FSceneView& View    = *Parameters.View;
			
			
			
			
		});*/
	
	
	
}

IMPLEMENT_GLOBAL_SHADER(FS3L1_BasicParticleEffectVS,"/Plugins/UEGraphicsProgramming/FS3L1_BasicParticleEffectVSPS.usf",
	"FS3L1_BasicParticleEffectVS", SF_Vertex);
IMPLEMENT_GLOBAL_SHADER(FS3L1_BasicParticleEffectPS,"/Plugins/UEGraphicsProgramming/FS3L1_BasicParticleEffectVSPS.usf",
	"FS3L1_BasicParticleEffectPS", SF_Pixel);

void FS3L1_BasicParticleEffectPS::AddPass(
	FRDGBuilder& GraphBuilder,
	//creates a list commands that are fed to gpu
	const FSceneView& View,
	// contains camera location rotations, projection matrix, viewport window etc
	FRDGTextureRef ColorTexture,
	//contains scene color (unreal render pass output) we will overlay our pass over it
	FRDGTextureRef DepthTexture,
	// scene depth
	const FIntRect& ViewportRect,
	// in case of splid screen or editor window
	const TRefCountPtr<FRDGPooledBuffer>& PooledParticleBuffer,
	//shared gpu compute buffer
	int32 ParticleCount,
	float PointSize, FVector3f ActorLocation,
	FRHITexture* ParticleTextureRHI)
{
	check(IsInRenderingThread());
	check(PooledParticleBuffer.IsValid());
	if (!PooledParticleBuffer.IsValid()){return;}
	//Registering external gpuffer in the pass
	FRDGBufferRef ParticleBuffer = GraphBuilder.RegisterExternalBuffer(
		PooledParticleBuffer, TEXT("ParticleBufferr"));

	FParameters* P =
		GraphBuilder.AllocParameters<FParameters>();
	P->FS3L1_ParticleBuffer = GraphBuilder.CreateSRV(ParticleBuffer);
	P->WorldToClip    = FMatrix44f(View.ViewMatrices.GetViewProjectionMatrix());//see vertex shader
	P->PointSize      = PointSize * 0.001f;
	P->ParticleCount  = ParticleCount;
	if (!ParticleTextureRHI->IsValid()){P->ParticleTexture = GWhiteTexture->TextureRHI;}
	P->ParticleTexture = ParticleTextureRHI;
	P->ActorLocation = ActorLocation;
	P->ParticleSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	P->RenderTargets[0] = FRenderTargetBinding(ColorTexture, ERenderTargetLoadAction::ELoad); //this pass will write output of the pass on this RT
	P->RenderTargets.DepthStencil = FDepthStencilBinding( //binds depth buffer in the pass
		DepthTexture, ERenderTargetLoadAction::ELoad, 
		ERenderTargetLoadAction::ELoad,
		FExclusiveDepthStencil::DepthRead_StencilNop);// This makes sure that this depth buffer is readonly

	TShaderMapRef<FS3L1_BasicParticleEffectVS> VertexShader(GetGlobalShaderMap(View.GetFeatureLevel()));//fetches the previously compiled shaderr
	TShaderMapRef<FS3L1_BasicParticleEffectPS> PixelShader (GetGlobalShaderMap(View.GetFeatureLevel()));//fetches the previously compiled shaderr

	const uint32 NumPrimitives = (uint32)ParticleCount * 2; //Here, primitive is a tringle, particle is drawn on a quad that has two triangles

	GraphBuilder.AddPass(
		RDG_EVENT_NAME("ParticleDraw"), P, ERDGPassFlags::Raster, //ERDGPassFlags::Raster means draw pass
		[P, VertexShader, PixelShader, ViewportRect, NumPrimitives]
		(FRHICommandList& RHICmdList)
		{
			//A PSO (Pipeline State Object) bundles all fixed-function GPU state (blend mode, depth test,
			//rasterizer settings, shaders, render target formats) into one immutable object the GPU can validate and switch quickly
			FGraphicsPipelineStateInitializer PSOInit;
			RHICmdList.ApplyCachedRenderTargets(PSOInit); // picks up RenderTargets from P

			// additive-ish alpha blend
			PSOInit.BlendState = TStaticBlendState<//Blending controls how the pixel shader's output combines with what's already in the color buffer
													CW_RGBA, //we will write rgba from this pass
													BO_Add, //BO = blend operation (we will add our pass output over exusting buffer) finalColor = srcColor × srcAlpha + destColor × (1 − srcAlpha)
													BF_SourceAlpha,//Blend factor A
													BF_InverseSourceAlpha,//Blend factor B 
													BO_Add, //These decides what happens with alpha 
													BF_One, //dstAlpha = srcAlpha × 1 + dstAlpha × 0
													BF_Zero
													>::GetRHI();

			//Rasterizer converts triangles into pixels
			//FM_Solid (Fill mode) = fill triangles (no wireframe)
			//CM_None (Cull mode)= no backface culling (Show particles facing opposite direction to camera)
			PSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();

			// Unreal uses reversed-Z: "closer" passes with GreaterEqual.
			// Depth test on, depth write OFF (we bound depth read-only above).
			PSOInit.DepthStencilState =
				TStaticDepthStencilState<false, CF_GreaterEqual>::GetRHI();

			PSOInit.PrimitiveType = PT_TriangleList; //Defining primitive type is triangle
			//A vertex declaration normally describes the layout of vertex buffer data (position at offset 0, UV at offset 12...).
			//Here it's empty because there is no vertex buffer at all. The vertex shader will instead use the built-in SV_VertexID 
			//(a counter: 0, 1, 2, ...) to compute everything itself.
			PSOInit.BoundShaderState.VertexDeclarationRHI =
				GEmptyVertexDeclaration.VertexDeclarationRHI; // no vertex stream; VS uses SV_VertexID
			//Plugs the two shader programs into the pipeline.
			PSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
			PSOInit.BoundShaderState.PixelShaderRHI  = PixelShader.GetPixelShader();

			//Looks up (or compiles and caches) the actual PSO matching this description and binds it on the command list.
			//From now on, draws use this complete pipeline configuration.
			SetGraphicsPipelineState(RHICmdList, PSOInit, 0);

			//Viewport and parameters
			RHICmdList.SetViewport(
				(float)ViewportRect.Min.X, (float)ViewportRect.Min.Y, 0.0f,
				(float)ViewportRect.Max.X, (float)ViewportRect.Max.Y, 1.0f);

			SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), *P);
			SetShaderParameters(RHICmdList, PixelShader,  PixelShader.GetPixelShader(),  *P);

			// base vertex 0, NumPrimitives triangles, 1 instance.
			// The VS reads ParticleBuffer[SV_VertexID / 6] and expands the quad.
			//Actual draw call
			RHICmdList.DrawPrimitive(0, NumPrimitives, 1);
		});
	
	
	
}

void FS3L0_BasicPointParticleEffectCS::InitShaderData(TRefCountPtr<FRDGPooledBuffer>& inPooledParticleBufffer,
	uint32 ParticleCount, FParticlesInitialised& inDelegate, FVector3d ActorLocation)
{
	
	//Passing &inPooledParticleBufffer as refrence so need to make sure that pooled particle buffer is safe on game thread
	ENQUEUE_RENDER_COMMAND(InitParticleBuffer)(
		[&inPooledParticleBufffer,ParticleCount,inDelegate,ActorLocation](FRHICommandListImmediate& RHICmdList)
		{
			
			//iF IT bLOCKS RENDER THREAD, MOVE data creation loop on a worker thread or use parallel for
			
			
			/*Particle setting logic start*/
			TArray<FParticle> InitialParticleArray;
			InitialParticleArray.SetNum(ParticleCount);
			
			for (uint32 i = 0 ;i < ParticleCount; i++)
			{
				FVector3f TempInitialParticleLocation = FVector3f()  ;
				TempInitialParticleLocation.X = FMath::FRand() * 2.f - 1.f;
				TempInitialParticleLocation.Y = FMath::FRand() * 2.f - 1.f;
				TempInitialParticleLocation.Z = FMath::FRand() * 2.f - 1.f;
				
				TempInitialParticleLocation.Normalize();
				
				TempInitialParticleLocation *= FMath::FRand() * 0.5f;
				
				
				InitialParticleArray[i].Position.X = TempInitialParticleLocation.X;
				InitialParticleArray[i].Position.Y = TempInitialParticleLocation.Y;
				InitialParticleArray[i].Position.Z = TempInitialParticleLocation.Z + 3;
				
				//InitialParticleArray[i].Position += FVector3f(ActorLocation);
				
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
			
			AsyncTask(ENamedThreads::GameThread,[inDelegate]
			{
				inDelegate.Broadcast(100);
			});
			
			
		});
	
}


IMPLEMENT_GLOBAL_SHADER(FS3L0_BasicPointParticleEffectCS, "/Plugins/UEGraphicsProgramming/FS3L0_BasicPointParticleEffectCS.usf",
						"FS3L0_BasicPointParticleEffectCS", SF_Compute);

void FS3L0_BasicPointParticleEffectCS::FS3L0_BasicPointParticleEffectCSRun(
	TRefCountPtr<FRDGPooledBuffer> inPooledParticleBufffer, const float inDeltatime, uint32 ParticleCount,
	FVector3f Focus)
{
	
	if (!inPooledParticleBufffer.IsValid()){return;}
	
	// Executing logic in render thread while passing OutputRender target to render thread via this lambda function
	ENQUEUE_RENDER_COMMAND(FS3L0_BasicPointParticleEffectCSRun)(
		[inPooledParticleBufffer,inDeltatime,ParticleCount,Focus](FRHICommandListImmediate& RHICmdList)
		{
			
			//Getting new graph builder each frame
			FRDGBuilder GraphBuilder(RHICmdList);
			
			//Registering Buffer in RDG 
			FRDGBufferRef FS3L0_ParticleBufferRDGRef = GraphBuilder.RegisterExternalBuffer(inPooledParticleBufffer,
																	TEXT("FS3L0_ParticlePointBufferRDGRef")
																	);
			
			//ShaderParameterSetup
			FS3L1_BasicParticleEffectCS::FParameters* FS3L0_Parameters = GraphBuilder.AllocParameters<
				FS3L1_BasicParticleEffectCS::FParameters>();
			
			FS3L0_Parameters->FS3L1_ParticleBuffer =  GraphBuilder.CreateUAV(FS3L0_ParticleBufferRDGRef)  ; //Compute Shader directly writes on this RT
			FS3L0_Parameters->ParticleCount = ParticleCount;
			FS3L0_Parameters->Focus = Focus; //Assuming that RT is a squaree
			FS3L0_Parameters->Deltatime = inDeltatime;
			
			
			//Dispatching shader
			//Initialising compute shader object
			TShaderMapRef<FS3L0_BasicPointParticleEffectCS> FS3L0_ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
			
			//Setting up Group Count
			FIntVector GroupCount = FIntVector(ParticleCount+ ThreadsX - 1/ThreadsX,1,1);
			
			//Actually dispatching the shader
			
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FS3L0"),
				FS3L0_ComputeShader,
				FS3L0_Parameters,
				GroupCount
				);
			
			GraphBuilder.Execute();
			
		}
	);
	
}


IMPLEMENT_GLOBAL_SHADER(FS3L0_BasicParticlePointEffectVS,"/Plugins/UEGraphicsProgramming/FS3L0_BasicParticlePointEffectVSPS.usf",
	"FS3L0_BasicParticlePointEffectVS", SF_Vertex);
IMPLEMENT_GLOBAL_SHADER(FS3L0_BasicParticlePointEffectPS,"/Plugins/UEGraphicsProgramming/FS3L0_BasicParticlePointEffectVSPS.usf",
	"FS3L0_BasicParticlePointEffectPS", SF_Pixel);

void FS3L0_BasicParticlePointEffectPS::FS3L0_BasicPointParticleEffectCSVSRun(
														FRDGBuilder& GraphBuilder,
														const FSceneView& View,
														FRDGTextureRef ColorTexture,
														FRDGTextureRef DepthTexture,
														const FIntRect& ViewportRect,
														const TRefCountPtr<FRDGPooledBuffer>& PooledParticleBuffer,
														int32 ParticleCount,
														float PointSize,
														FVector3f ActorLocation)
{
	check(IsInRenderingThread());
	check(PooledParticleBuffer.IsValid());
	if (!PooledParticleBuffer.IsValid()){return;}
	//Registering external gpuffer in the pass
	FRDGBufferRef ParticleBuffer = GraphBuilder.RegisterExternalBuffer(
		PooledParticleBuffer, TEXT("ParticleBufferr"));

	FParameters* P =
		GraphBuilder.AllocParameters<FParameters>();
	P->FS3L1_ParticleBuffer = GraphBuilder.CreateSRV(ParticleBuffer);
	P->WorldToClip    = FMatrix44f(View.ViewMatrices.GetViewProjectionMatrix());//see vertex shader
	P->PointSize      = PointSize * 100 /** 0.001f*/;
	P->ParticleCount  = ParticleCount;
	P->ActorLocation = ActorLocation;
	P->RenderTargets[0] = FRenderTargetBinding(ColorTexture, ERenderTargetLoadAction::ELoad); //this pass will write output of the pass on this RT
	P->RenderTargets.DepthStencil = FDepthStencilBinding( //binds depth buffer in the pass
		DepthTexture, ERenderTargetLoadAction::ELoad, 
		ERenderTargetLoadAction::ELoad,
		FExclusiveDepthStencil::DepthRead_StencilNop);// This makes sure that this depth buffer is readonly

	TShaderMapRef<FS3L0_BasicParticlePointEffectVS> VertexShader(GetGlobalShaderMap(View.GetFeatureLevel()));//fetches the previously compiled shaderr
	TShaderMapRef<FS3L0_BasicParticlePointEffectPS> PixelShader (GetGlobalShaderMap(View.GetFeatureLevel()));//fetches the previously compiled shaderr

	const uint32 NumPrimitives = (uint32)ParticleCount ; //Here, primitive is a point

	GraphBuilder.AddPass(
		RDG_EVENT_NAME("PointParticleDraw"), P, ERDGPassFlags::Raster, //ERDGPassFlags::Raster means draw pass
		[P, VertexShader, PixelShader, ViewportRect, NumPrimitives]
		(FRHICommandList& RHICmdList)
		{
			//A PSO (Pipeline State Object) bundles all fixed-function GPU state (blend mode, depth test,
			//rasterizer settings, shaders, render target formats) into one immutable object the GPU can validate and switch quickly
			FGraphicsPipelineStateInitializer PSOInit;
			RHICmdList.ApplyCachedRenderTargets(PSOInit); // picks up RenderTargets from P

			// additive-ish alpha blend
			PSOInit.BlendState = TStaticBlendState<//Blending controls how the pixel shader's output combines with what's already in the color buffer
													CW_RGBA, //we will write rgba from this pass
													BO_Add, //BO = blend operation (we will add our pass output over exusting buffer) finalColor = srcColor × srcAlpha + destColor × (1 − srcAlpha)
													BF_SourceAlpha,//Blend factor A
													BF_InverseSourceAlpha,//Blend factor B 
													BO_Add, //These decides what happens with alpha 
													BF_One, //dstAlpha = srcAlpha × 1 + dstAlpha × 0
													BF_Zero
													>::GetRHI();

			//Rasterizer converts triangles into pixels
			//FM_Solid (Fill mode) = fill triangles (no wireframe)
			//CM_None (Cull mode)= no backface culling (Show particles facing opposite direction to camera)
			PSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();

			// Unreal uses reversed-Z: "closer" passes with GreaterEqual.
			// Depth test on, depth write OFF (we bound depth read-only above).
			PSOInit.DepthStencilState =
				TStaticDepthStencilState<false, CF_GreaterEqual>::GetRHI();

			PSOInit.PrimitiveType = PT_PointList; //Defining primitive type is a point
			//A vertex declaration normally describes the layout of vertex buffer data (position at offset 0, UV at offset 12...).
			//Here it's empty because there is no vertex buffer at all. The vertex shader will instead use the built-in SV_VertexID 
			//(a counter: 0, 1, 2, ...) to compute everything itself.
			PSOInit.BoundShaderState.VertexDeclarationRHI =
				GEmptyVertexDeclaration.VertexDeclarationRHI; // no vertex stream; VS uses SV_VertexID
			//Plugs the two shader programs into the pipeline.
			PSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
			PSOInit.BoundShaderState.PixelShaderRHI  = PixelShader.GetPixelShader();

			//Looks up (or compiles and caches) the actual PSO matching this description and binds it on the command list.
			//From now on, draws use this complete pipeline configuration.
			SetGraphicsPipelineState(RHICmdList, PSOInit, 0);

			//Viewport and parameters
			RHICmdList.SetViewport(
				(float)ViewportRect.Min.X, (float)ViewportRect.Min.Y, 0.0f,
				(float)ViewportRect.Max.X, (float)ViewportRect.Max.Y, 1.0f);

			SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), *P);
			SetShaderParameters(RHICmdList, PixelShader,  PixelShader.GetPixelShader(),  *P);

			// base vertex 0, NumPrimitives triangles, 1 instance.
			// The VS reads ParticleBuffer[SV_VertexID / 6] and expands the quad.
			//Actual draw call
			RHICmdList.DrawPrimitive(0, NumPrimitives, 1);
		});
}


