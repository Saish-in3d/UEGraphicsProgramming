// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DynamicMeshActor_GP.h"

/*#include "GeometryScript/"*/

#include "GeometryScript/MeshPrimitiveFunctions.h"

// Sets default values
ADynamicMeshActor_GP::ADynamicMeshActor_GP()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DynamicMeshComp = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("DynamicMeshComp"));
	SetRootComponent(DynamicMeshComp);

	DynamicMeshComp->SetMobility(EComponentMobility::Movable);
	DynamicMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	
	Mesh = DynamicMeshComp->GetDynamicMesh();

	
	
	

	DynamicMeshComp->NotifyMeshUpdated();
}

// Called when the game starts or when spawned
void ADynamicMeshActor_GP::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void ADynamicMeshActor_GP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADynamicMeshActor_GP::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	ConvertStaticMeshToDynamicMesh(StaticMeshComponent_temp,DynamicMeshComp);
	
}

void ADynamicMeshActor_GP::ConvertStaticMeshToDynamicMesh(UStaticMesh* StaticMesh,UDynamicMeshComponent* inDynamicMeshComp
)
{
	if (!StaticMesh || !inDynamicMeshComp)
	{
		return;
	}

	// Get LOD 0 mesh description
	const FMeshDescription* MeshDesc = StaticMesh->GetMeshDescription(0);
	if (!MeshDesc)
	{
		return;
	}

	UDynamicMesh* DynMesh = inDynamicMeshComp->GetDynamicMesh();
	check(DynMesh);

	DynMesh->EditMesh([&](UE::Geometry::FDynamicMesh3& EditMesh)
	{
		EditMesh.Clear();

		FMeshDescriptionToDynamicMesh Converter;
		Converter.Convert(MeshDesc, EditMesh);
	});

	inDynamicMeshComp->NotifyMeshUpdated();
}

