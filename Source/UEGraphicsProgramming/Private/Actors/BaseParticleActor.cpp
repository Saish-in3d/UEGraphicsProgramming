// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseParticleActor.h"
#include "SceneRendering.h"
#include "RendererInterface.h"
// Sets default values
ABaseParticleActor::ABaseParticleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	OnParticlesInitialised.AddLambda([this](int a)
	{
		switch (LessonIndex)
		{
		default:
			this->OnQuadParticlesInitialisedFunction(a);
			break;
		case 0:
			this->OnQuadParticlesInitialisedFunction(a);
			break;
		case 1:
			this->OnPointParticlesInitialisedFunction(a);
			break;
			
		}
		
	});
	

}

// Called when the game starts or when spawned
void ABaseParticleActor::BeginPlay()
{
	Super::BeginPlay();
	
	PooledParticleBufferProxy = MakeShared<FParticleProxy, ESPMode::ThreadSafe>();
	
	FS3L1_BasicParticleEffectCS::InitShaderData(PooledParticleBufferProxy->PooledParticleBuffer,ParticleCount,OnParticlesInitialised, GetActorLocation());
	
	
}

// Called every frame
void ABaseParticleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseParticleActor::OnPointParticlesInitialisedFunction(int a)
{
	if (FModuleManager::Get().IsModuleLoaded("UEGraphicsProgramming"))
	{
		FUEGraphicsProgrammingModule& Module =
			FModuleManager::GetModuleChecked<FUEGraphicsProgrammingModule>(
				"UEGraphicsProgramming");
		
		//Module.BoundRendererDelegate();
		
		D.BindLambda([this](FPostOpaqueRenderParameters& Params)
	{
		//check(this && this->PooledParticleBufferProxy.IsValid() &&  this->PooledParticleBufferProxy->PooledParticleBuffer.IsValid());
		bool isPooledBufferValid = this->PooledParticleBufferProxy->PooledParticleBuffer.IsValid();
			
			
		if (!isPooledBufferValid){return;}
		
		const FSceneView& View = *Params.View;
		
		FS3L0_BasicParticlePointEffectPS::FS3L0_BasicPointParticleEffectCSVSRun(
											*Params.GraphBuilder,
											 View,
											 Params.ColorTexture,
											 Params.DepthTexture,
											 Params.ViewportRect,
											 this->PooledParticleBufferProxy->PooledParticleBuffer,
											 this->ParticleCount,
											 this->PointSize, 
											 FVector3f(GetActorLocation()));
		
	});
		Module.BoundRendererDelegate(RendererHandle,D);
	
		
		
	}
	
}

void ABaseParticleActor::OnQuadParticlesInitialisedFunction(int a)
{
	
	if (FModuleManager::Get().IsModuleLoaded("UEGraphicsProgramming"))
	{
		FUEGraphicsProgrammingModule& Module =
			FModuleManager::GetModuleChecked<FUEGraphicsProgrammingModule>(
				"UEGraphicsProgramming");
		
		//Module.BoundRendererDelegate();
		
		D.BindLambda([this](FPostOpaqueRenderParameters& Params)
	{
			//check(this->PooledParticleBufferProxy->PooledParticleBuffer.IsValid());
		bool isPooledBufferValid = this->PooledParticleBufferProxy->PooledParticleBuffer.IsValid();
			
			
		if (!isPooledBufferValid){return;}
		
		const FSceneView& View = *Params.View;
		
		FS3L1_BasicParticleEffectPS::AddPass(*Params.GraphBuilder,
		                                     View,
		                                     Params.ColorTexture,
		                                     Params.DepthTexture,
		                                     Params.ViewportRect,
		                                     this->PooledParticleBufferProxy->PooledParticleBuffer,
		                                     this->ParticleCount,
		                                     this->PointSize, 
		                                     FVector3f(GetActorLocation()));
		
	});
	Module.BoundRendererDelegate(RendererHandle,D);
	
		
		
	}
	
	
}


void ABaseParticleActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (FModuleManager::Get().IsModuleLoaded("UEGraphicsProgramming"))
	{
		FUEGraphicsProgrammingModule& Module =
			FModuleManager::GetModuleChecked<FUEGraphicsProgrammingModule>("UEGraphicsProgramming");

		if (RendererHandle.IsValid())
		{
			Module.UnboundRendererHandle(RendererHandle,D);
			
		}
		D.Unbind();
	}

	// Drain any post-opaque pass already in flight that may still touch this actor
	FlushRenderingCommands();

	// Release the GPU buffer on the render thread
	if (PooledParticleBufferProxy.IsValid())
	{
		ENQUEUE_RENDER_COMMAND(ReleaseParticleBuffer)(
			[Proxy = PooledParticleBufferProxy](FRHICommandListImmediate&)
			{
				Proxy->PooledParticleBuffer.SafeRelease();
			});
		FlushRenderingCommands();
	}

	Super::EndPlay(EndPlayReason);
}

