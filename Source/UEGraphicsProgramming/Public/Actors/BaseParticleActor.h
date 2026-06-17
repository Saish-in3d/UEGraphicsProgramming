// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "S3_ParticleEffectsShaders.h"
#include "UEGraphicsProgramming.h"
#include "BaseParticleActor.generated.h"

class FParticleProxy
{
public:
	TRefCountPtr<FRDGPooledBuffer> PooledParticleBuffer;
};



UCLASS()
class UEGRAPHICSPROGRAMMING_API ABaseParticleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseParticleActor();
	
	UPROPERTY(EditAnywhere)
	int32 ParticleCount =1;
	
	UPROPERTY(EditAnywhere)
	float PointSize = 100;
	
	UPROPERTY(EditAnywhere)
	int32 LessonIndex = 0;
	
	UPROPERTY(EditAnywhere)
	UTexture2D* ParticleSpriteTexture;
	
	//Sideways push that carries the particles around the actor. Terminal orbit speed settles at
	//roughly 1.44 * this value, in world units per second.
	UPROPERTY(EditAnywhere)
	float OrbitSpeed = 150.f;
	
	//Distance from the actor the ring settles at, in world units.
	UPROPERTY(EditAnywhere)
	float OrbitRadius = 100.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
	
	private:
	TSharedPtr<FParticleProxy, ESPMode::ThreadSafe> PooledParticleBufferProxy;
	
	FDelegateHandle RendererHandle;
	
	FPostOpaqueRenderDelegate D;
	
	FParticlesInitialised OnParticlesInitialised;
	
	void OnPointParticlesInitialisedFunction(int a);
	
	void OnQuadParticlesInitialisedFunction(int a);
	
	//The particle buffer is filled on the render thread. This is set from the game thread once
	//that has finished, so Tick knows when it is safe to start dispatching the simulation.
	bool bParticlesInitialised = false;
	
	void EndPlay(EEndPlayReason::Type EndPlayReason);
	
	
};
