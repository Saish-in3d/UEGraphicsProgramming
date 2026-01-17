// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/HISMActor.h"
#include "S2_ExternalDataBasicShaders.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

// Sets default values
AHISMActor::AHISMActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HISM"));
	SetRootComponent(HISM);

	// Optional but recommended
	HISM->SetMobility(EComponentMobility::Static);
	HISM->NumCustomDataFloats = 0;
	
	StarTransformReadback = MakeUnique<FRHIGPUBufferReadback>(TEXT("StarTransformReadback"));
	
}

// Called when the game starts or when spawned
void AHISMActor::BeginPlay()
{
	Super::BeginPlay();
	
	

	
}

bool AHISMActor::InitHISM(int instanceCount)
{
	if (!HISM->GetStaticMesh())
	{
		if (!HISMStaticMesh){return true;}
		
		HISM->SetStaticMesh(HISMStaticMesh);
	}
	
	HISM->ClearInstances();
	
	const float Spacing = 100.f;

	for (int32 i = 0; i < InstanceCount; ++i)
	{
		const FVector Location(
			(i % 50) * Spacing,
			(i / 50) * Spacing,
			0.f
		);

		const FTransform InstanceTransform(
			FRotator::ZeroRotator,
			Location,
			FVector(1.f)
		);

		HISM->AddInstance(InstanceTransform);
		
		
		
	}
	
	TransformsArray.Reserve(InstanceCount);
	return false;
}

void AHISMActor::UpdateHISMTransforms(TArray<FTransform> inTransformsArray)
{
	HISM->BatchUpdateInstancesTransforms(
		0,
		inTransformsArray,
		false,
		true,
		true);
	
}

void AHISMActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	
	InitHISM(InstanceCount);
	
}

// Called every frame
void AHISMActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
	
	FS2L3_CustomStructBufferReadBackCS::FS2L3_CustomStructBufferReadBackCSRun(DeltaTime,
	TransformsArray,
		InstanceCount,
	StarTransformReadback
		);
	
	
	
	//Switch cal shader get transform array and use UpdateHISMTransforms() to update locations
	UpdateHISMTransforms(TransformsArray);
}

