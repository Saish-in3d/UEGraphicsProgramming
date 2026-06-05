// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DynamicMeshActor_GP.h"
#include "GeometryScript/MeshAssetFunctions.h"
#include "DynamicMesh/MeshNormals.h"
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

	//UGeometryScriptLibrary_StaticMeshFunctions::CopyMeshFromStaticMesh();
	
	

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
	
	
	ConvertStaticMeshToDynamicMesh(StaticMeshComponent_temp,DynamicMeshComp, this->GetGameTimeSinceCreation());

}

void ADynamicMeshActor_GP::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	ConvertStaticMeshToDynamicMesh(StaticMeshComponent_temp,DynamicMeshComp,1.f);
	
}

void ADynamicMeshActor_GP::ConvertStaticMeshToDynamicMesh(UStaticMesh* StaticMesh,UDynamicMeshComponent* inDynamicMeshComp
,float Time)
{
	if (!StaticMesh || !inDynamicMeshComp)
	{
		return;
	}

	/*// Get LOD 0 mesh description
	const FMeshDescription* MeshDesc = StaticMesh->GetMeshDescription(0);
	if (!MeshDesc)
	{
		return;
	}*/
	
	UDynamicMesh* DynMesh = inDynamicMeshComp->GetDynamicMesh();
	check(DynMesh);
	
	EGeometryScriptOutcomePins Outcome;
	FGeometryScriptCopyMeshFromAssetOptions AssetOptions;
	FGeometryScriptMeshReadLOD RequestedLOD;  
	
	UGeometryScriptLibrary_StaticMeshFunctions::CopyMeshFromStaticMesh(
	StaticMesh, DynMesh, AssetOptions, RequestedLOD, Outcome);

	

	DynMesh->EditMesh([&](UE::Geometry::FDynamicMesh3& EditMesh)
	{
		//EditMesh.Clear();
		
		UE::Geometry::TDynamicVector<FVector3d> Vertices;
		
		Vertices = EditMesh.GetVerticesBuffer();
		
		for (int32 vid : EditMesh.VertexIndicesItr())
		{
			FVector3d Pos = EditMesh.GetVertex(vid);
			Pos.Z += FMath::Sin(Pos.X * 0.05 + Time) * 20.0;
			EditMesh.SetVertex(vid, Pos);
			
		}
		
		// Positions moved -> normals are now wrong. Recompute them.
		if (EditMesh.HasAttributes())
		{
			UE::Geometry::FMeshNormals::QuickRecomputeOverlayNormals(EditMesh);
		}
		
		
	}, EDynamicMeshChangeType::DeformationEdit);

	inDynamicMeshComp->NotifyMeshUpdated();
}

