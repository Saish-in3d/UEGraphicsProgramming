// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/InstancedMeshBoidsActor.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"
#include "SceneRendering.h"
#include "RendererInterface.h"

// Sets default values
AInstancedMeshBoidsActor::AInstancedMeshBoidsActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Nothing to render through the component system - our own pass draws the whole flock - so the
	//root is just a transform for the simulation to be relative to.
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("BoidsRoot"));
	SetRootComponent(SceneRoot);

	OnBoidsInitialised.AddLambda([this](int a)
	{
		this->OnBoidsInitialisedFunction(a);
	});

}

bool AInstancedMeshBoidsActor::BuildBoidMeshVertices(UStaticMesh* inMesh, TArray<FBoidMeshVertex>& OutVertices)
{
	OutVertices.Reset();

	if (!inMesh){return false;}

	FStaticMeshRenderData* RenderData = inMesh->GetRenderData();
	if (!RenderData || RenderData->LODResources.Num() == 0){return false;}

	const FStaticMeshLODResources& LOD = RenderData->LODResources[0];

	const FPositionVertexBuffer& PositionBuffer = LOD.VertexBuffers.PositionVertexBuffer;
	const FStaticMeshVertexBuffer& StaticMeshVertexBuffer = LOD.VertexBuffers.StaticMeshVertexBuffer;

	const uint32 NumPositions = PositionBuffer.GetNumVertices();
	const uint32 NumTangents = StaticMeshVertexBuffer.GetNumVertices();

	if (NumPositions == 0){return false;}

	TArray<uint32> Indices;
	LOD.IndexBuffer.GetCopy(Indices);

	if (Indices.Num() < 3){return false;}

	//De-index into a flat triangle list. This costs roughly 3x the memory of the indexed mesh,
	//which is nothing for a boid, and in exchange the draw needs no index buffer at all - the
	//vertex shader can use SV_VertexID directly as an index into this array.
	OutVertices.Reserve(Indices.Num());

	for (int32 i = 0; i + 2 < Indices.Num(); i += 3)
	{
		for (int32 Corner = 0; Corner < 3; Corner++)
		{
			const uint32 SourceIndex = Indices[i + Corner];

			if (SourceIndex >= NumPositions){return false;}

			FBoidMeshVertex& NewVertex = OutVertices.AddDefaulted_GetRef();
			NewVertex.Position = PositionBuffer.VertexPosition(SourceIndex);
			NewVertex.Padding0 = 0.f;
			NewVertex.Normal = (SourceIndex < NumTangents)
				? FVector3f(StaticMeshVertexBuffer.VertexTangentZ(SourceIndex))
				: FVector3f(0.f, 0.f, 1.f);
			NewVertex.Padding1 = 0.f;
		}
	}

	return OutVertices.Num() >= 3;
}

// Called when the game starts or when spawned
void AInstancedMeshBoidsActor::BeginPlay()
{
	Super::BeginPlay();

	//Latch the count once. From here on the simulation and the draw agree, whatever happens to
	//the editable property.
	ActiveBoidCount = FMath::Max(BoidCount, 0);

	if (ActiveBoidCount <= 0){return;}

	if (!BoidStaticMesh)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("AInstancedMeshBoidsActor: no BoidStaticMesh assigned, nothing to draw."));
		return;
	}

	TArray<FBoidMeshVertex> BoidMeshVertices;
	if (!BuildBoidMeshVertices(BoidStaticMesh, BoidMeshVertices))
	{
		//Almost always means the CPU copy of the vertex data was stripped. Ticking "Allow CPU
		//Access" on the static mesh asset keeps it around.
		UE_LOG(LogTemp, Warning,
			TEXT("AInstancedMeshBoidsActor: could not read geometry from %s. Enable Allow CPU Access on the mesh."),
			*BoidStaticMesh->GetName());
		return;
	}

	BoidSimulationProxy = MakeShared<FBoidSimulationProxy, ESPMode::ThreadSafe>();

	FS3L2_InstancedMeshBoidsCS::InitShaderData(BoidSimulationProxy,
	                                           ActiveBoidCount,
	                                           SpawnRadius,
	                                           MoveTemp(BoidMeshVertices),
	                                           OnBoidsInitialised);
}

void AInstancedMeshBoidsActor::OnBoidsInitialisedFunction(int a)
{
	bBoidsInitialised = true;

	if (!FModuleManager::Get().IsModuleLoaded("UEGraphicsProgramming")){return;}

	FUEGraphicsProgrammingModule& Module =
		FModuleManager::GetModuleChecked<FUEGraphicsProgrammingModule>("UEGraphicsProgramming");

	//Hook our draw into the scene renderer, after opaque geometry has been laid down so the depth
	//buffer we test and write against is already populated.
	BoidRenderDelegate.BindLambda([this](FPostOpaqueRenderParameters& Params)
	{
		if (!this->BoidSimulationProxy.IsValid()){return;}

		const FSceneView& View = *Params.View;

		FS3L2_InstancedMeshBoidsPS::AddPass(
			*Params.GraphBuilder,
			View,
			Params.ColorTexture,
			Params.DepthTexture,
			Params.ViewportRect,
			this->BoidSimulationProxy,
			this->ActiveBoidCount,
			FVector3f(GetActorLocation()),
			FVector3f(this->BoidScale),
			FVector3f(this->LightDirection),
			this->BoidColor);
	});

	Module.BoundRendererDelegate(RendererHandle, BoidRenderDelegate);
}

// Called every frame
void AInstancedMeshBoidsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SectionIndex != 3 || LessonIndex != 2){return;}

	//The buffers are allocated and seeded on the render thread, OnBoidsInitialised tells us on the
	//game thread when that has finished.
	if (!bBoidsInitialised){return;}

	if (!BoidSimulationProxy.IsValid()){return;}

	FS3L2_BoidSimulationSettings BoidSimulationSettings;
	BoidSimulationSettings.NeighbourRadius = NeighbourRadius;
	BoidSimulationSettings.SeparationRadius = SeparationRadius;
	BoidSimulationSettings.SeparationWeight = SeparationWeight;
	BoidSimulationSettings.AlignmentWeight = AlignmentWeight;
	BoidSimulationSettings.CohesionWeight = CohesionWeight;
	BoidSimulationSettings.ContainmentRadius = ContainmentRadius;
	BoidSimulationSettings.ContainmentWeight = ContainmentWeight;
	BoidSimulationSettings.MinSpeed = MinSpeed;
	BoidSimulationSettings.MaxSpeed = MaxSpeed;

	//The vertex shader adds ActorLocation to every boid position before projecting, so the whole
	//simulation runs in actor local space and the zero vector IS the actor's location. Moving the
	//actor carries the flock with it, exactly like the S3L0/S3L1 particles.
	BoidSimulationSettings.Focus = FVector3f::ZeroVector;

	FS3L2_InstancedMeshBoidsCS::FS3L2_InstancedMeshBoidsCSRun(BoidSimulationProxy,
	                                                          bReadFromA,
	                                                          DeltaTime,
	                                                          ActiveBoidCount,
	                                                          BoidSimulationSettings);

	//This frame's output is next frame's input.
	bReadFromA = !bReadFromA;
}

void AInstancedMeshBoidsActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//Stop Tick from queueing any more work before we let go of the buffers.
	bBoidsInitialised = false;

	if (FModuleManager::Get().IsModuleLoaded("UEGraphicsProgramming"))
	{
		FUEGraphicsProgrammingModule& Module =
			FModuleManager::GetModuleChecked<FUEGraphicsProgrammingModule>("UEGraphicsProgramming");

		if (RendererHandle.IsValid())
		{
			Module.UnboundRendererHandle(RendererHandle, BoidRenderDelegate);
		}

		BoidRenderDelegate.Unbind();
	}

	//Drain any post opaque pass already in flight that may still touch this actor.
	FlushRenderingCommands();

	if (BoidSimulationProxy.IsValid())
	{
		ENQUEUE_RENDER_COMMAND(ReleaseBoidBuffers)(
			[Proxy = BoidSimulationProxy](FRHICommandListImmediate&)
			{
				Proxy->bBuffersReady = false;
				Proxy->BoidBufferA.SafeRelease();
				Proxy->BoidBufferB.SafeRelease();
				Proxy->MeshVertexBuffer.SafeRelease();
			});

		FlushRenderingCommands();

		BoidSimulationProxy.Reset();
	}

	Super::EndPlay(EndPlayReason);
}
