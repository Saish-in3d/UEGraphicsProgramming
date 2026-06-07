// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DynamicMeshActor_GP.h"
#include "GeometryScript/MeshAssetFunctions.h"
#include "DynamicMesh/MeshNormals.h"
/*#include "GeometryScript/"*/

#include "GeometryScript/MeshPrimitiveFunctions.h"

#include "S2_ExternalDataBasicShaders.h"

// Sets default values
ADynamicMeshActor_GP::ADynamicMeshActor_GP()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DynamicMeshComp = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("DynamicMeshComp"));
	SetRootComponent(DynamicMeshComp);

	DynamicMeshComp->SetMobility(EComponentMobility::Movable);
	DynamicMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	

	DynamicMeshComp->NotifyMeshUpdated();
}

// Called when the game starts or when spawned
void ADynamicMeshActor_GP::BeginPlay()
{
	Super::BeginPlay();
	ConvertStaticMeshToDynamicMesh(StaticMeshComponent_temp,DynamicMeshComp,1.f);

}

// Called every frame
void ADynamicMeshActor_GP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
	//ConvertStaticMeshToDynamicMesh(StaticMeshComponent_temp,DynamicMeshComp, this->GetGameTimeSinceCreation());
	DeformMesh();
	

}

void ADynamicMeshActor_GP::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	
	
}

void ADynamicMeshActor_GP::ConvertStaticMeshToDynamicMesh(UStaticMesh* StaticMesh,UDynamicMeshComponent* inDynamicMeshComp
,float Time)
{
	if (!StaticMesh || !inDynamicMeshComp)
	{
		return;
	}
	
	

	DynMesh = inDynamicMeshComp->GetDynamicMesh();
	check(DynMesh);
	
	EGeometryScriptOutcomePins Outcome;
	FGeometryScriptCopyMeshFromAssetOptions AssetOptions;
	FGeometryScriptMeshReadLOD RequestedLOD;  
	
	UGeometryScriptLibrary_StaticMeshFunctions::CopyMeshFromStaticMesh(
	StaticMesh, DynMesh, AssetOptions, RequestedLOD, Outcome);

	DynMesh->EditMesh([&](UE::Geometry::FDynamicMesh3& EditMesh)
	{
		InitialVertices.SetNum(EditMesh.VertexCount());
		
		for (int32 vid : EditMesh.VertexIndicesItr())
		{
			//Saving Compute shader friendly initial verts in InitialVertices array
			InitialVertices[vid] = (FVector3f)EditMesh.GetVertex(vid);
		}
		
	}, EDynamicMeshChangeType::DeformationEdit);

	DynamicMeshComp->NotifyMeshUpdated();

	
}



void ADynamicMeshActor_GP::DeformMesh()
{
	
	//Calculate alpha
	
	float Alpha = sin(GetGameTimeSinceCreation());
	
	//Run compute shader
	
	if (InitialVertices.IsEmpty()){return;}
	
	FS2L7_ReadbackDeformMeshCS::FS2L7_ReadbackDeformMeshCSRun(Alpha,
		InitialVertices.Num(),
		InitialVertices,
		GPUBufferReadbackQueue
		);
	
	
	
	ReadBackVertices.SetNum(InitialVertices.Num());
	
	FS2L7_ReadbackDeformMeshCS::PollReadBackandDownloadtoGameThread(
		ReadBackVertices,
		InitialVertices.Num(),
		GPUBufferReadbackQueue
		);
	
	
	if (ReadBackVertices.IsEmpty()){return;}
	
	DynMesh->EditMesh([&](UE::Geometry::FDynamicMesh3& EditMesh)
	{
		
		for (int32 vid : EditMesh.VertexIndicesItr())
		{
			FVector3d Pos = (FVector3d)ReadBackVertices[vid];
			EditMesh.SetVertex(vid, Pos);
			
		}
		
		// Positions moved -> normals are now wrong. Recompute them.
		if (EditMesh.HasAttributes())
		{
			UE::Geometry::FMeshNormals::QuickRecomputeOverlayNormals(EditMesh);
		}
		
		
	}, EDynamicMeshChangeType::GeneralEdit);

	DynamicMeshComp->NotifyMeshUpdated();
	
}

