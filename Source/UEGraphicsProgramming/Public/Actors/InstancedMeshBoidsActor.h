// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "S3_ParticleEffectsShaders.h"
#include "UEGraphicsProgramming.h"
#include "InstancedMeshBoidsActor.generated.h"

/**
 * S3L2 - Instanced mesh boids, drawn by our own instanced pass.
 *
 * The flocking runs in a compute shader and the result is drawn straight out of GPU memory by a
 * custom vertex/pixel shader pair, hooked into the scene renderer through the post opaque delegate.
 * The boid positions never touch the CPU.
 *
 * Compared with driving a UInstancedStaticMeshComponent instead: that route has to copy the
 * simulation back to the game thread every frame so the component can be told where to put its
 * instances, which costs a GPU to CPU round trip and about a frame of latency. It buys you the
 * engine's material system, shadows, LODs and culling for free. Doing the pass by hand, as here,
 * costs none of that bandwidth but means anything you want - lighting, shadows, LODs - you write
 * yourself. The pixel shader below does one hardcoded lambert term and that is the whole
 * lighting model.
 *
 * Builds on S3L1: same "no vertex buffer, no vertex declaration, pull everything from a
 * StructuredBuffer with SV_VertexID" trick, but with SV_InstanceID added so one draw call covers
 * the whole flock, and real mesh geometry instead of a generated quad.
 */
UCLASS()
class UEGRAPHICSPROGRAMMING_API AInstancedMeshBoidsActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInstancedMeshBoidsActor();

	//Geometry every boid is drawn with. LOD0 is de-indexed into a flat triangle list once at
	//BeginPlay, so keep it low poly - every triangle here is paid for BoidCount times.
	//Needs "Allow CPU Access" ticked on the asset if this is ever run outside the editor.
	UPROPERTY(EditInstanceOnly, meta = (category = "ActorDefaults"))
	UStaticMesh* BoidStaticMesh;

	UPROPERTY(EditInstanceOnly, meta = (category = "ActorDefaults"))
	int SectionIndex = 3;

	UPROPERTY(EditInstanceOnly, meta = (category = "ActorDefaults"))
	int LessonIndex = 2;

	//Read once when play begins, editing it mid simulation does nothing until the next play.
	UPROPERTY(EditInstanceOnly, meta = (category = "ActorDefaults"))
	int32 BoidCount = 512;

	UPROPERTY(EditAnywhere, Category = "Boid Spawn")
	FVector BoidScale = FVector(1.f);

	//Radius of the sphere the flock is seeded inside, in world units.
	UPROPERTY(EditAnywhere, Category = "Boid Spawn")
	float SpawnRadius = 400.f;

	//How far a boid can see. Alignment and cohesion average over everything inside this radius.
	UPROPERTY(EditAnywhere, Category = "Boid Rules")
	float NeighbourRadius = 600.f;

	//The closer band inside which boids actively push each other away. Keep it well under
	//NeighbourRadius, otherwise separation cancels cohesion and the flock never forms.
	UPROPERTY(EditAnywhere, Category = "Boid Rules")
	float SeparationRadius = 200.f;

	//Large because the separation term is divided by squared distance, so the raw value it
	//produces is tiny compared to the other two rules.
	UPROPERTY(EditAnywhere, Category = "Boid Rules")
	float SeparationWeight = 20000.f;

	UPROPERTY(EditAnywhere, Category = "Boid Rules")
	float AlignmentWeight = 3.f;

	UPROPERTY(EditAnywhere, Category = "Boid Rules")
	float CohesionWeight = 1.f;

	//Nothing in the three flocking rules references the world, so without containment the flock
	//slowly random walks out of the level. This pulls it back towards the actor.
	UPROPERTY(EditAnywhere, Category = "Boid Containment")
	float ContainmentRadius = 800.f;

	UPROPERTY(EditAnywhere, Category = "Boid Containment")
	float ContainmentWeight = 4.f;

	//Boids are birds, not billiard balls - they always move, and never faster than they can.
	UPROPERTY(EditAnywhere, Category = "Boid Speed")
	float MinSpeed = 250.f;

	UPROPERTY(EditAnywhere, Category = "Boid Speed")
	float MaxSpeed = 700.f;

	//Our pass does its own shading, so the scene's actual lights do not affect the boids at all.
	//This is the only light they have.
	UPROPERTY(EditAnywhere, Category = "Boid Shading")
	FVector LightDirection = FVector(-0.5f, -0.5f, -1.f);

	UPROPERTY(EditAnywhere, Category = "Boid Shading")
	FLinearColor BoidColor = FLinearColor(0.15f, 0.45f, 0.95f, 1.f);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//De-indexes LOD0 of inMesh into a flat triangle list. Returns false if the mesh has no usable
	//render data, which is the common failure when CPU access has been stripped.
	static bool BuildBoidMeshVertices(UStaticMesh* inMesh, TArray<FBoidMeshVertex>& OutVertices);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	//Owns the ping pong buffers and the uploaded mesh geometry. Shared so that render commands
	//still in flight after this actor dies have somewhere valid to land.
	TSharedPtr<FBoidSimulationProxy, ESPMode::ThreadSafe> BoidSimulationProxy;

	FDelegateHandle RendererHandle;

	FPostOpaqueRenderDelegate BoidRenderDelegate;

	FParticlesInitialised OnBoidsInitialised;

	void OnBoidsInitialisedFunction(int a);

	//The buffers are allocated and seeded on the render thread. This is set from the game thread
	//once that has finished, so Tick knows when it is safe to start dispatching.
	bool bBoidsInitialised = false;

	//Which half of the ping pong pair is this frame's input. Flipped every tick.
	bool bReadFromA = true;

	//BoidCount is latched when play begins so the simulation and the draw cannot disagree if the
	//property is edited while running.
	int32 ActiveBoidCount = 0;

};
