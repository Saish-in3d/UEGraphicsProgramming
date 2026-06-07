// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/DynamicMeshComponent.h"
#include "RHIGPUReadback.h"
#include "Containers/Queue.h"
#include "DynamicMeshActor_GP.generated.h"

UCLASS()
class UEGRAPHICSPROGRAMMING_API ADynamicMeshActor_GP : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ADynamicMeshActor_GP();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	
	UPROPERTY(EditAnywhere)
	UDynamicMeshComponent* DynamicMeshComp;
	
	UPROPERTY(EditAnywhere)
	UStaticMesh* StaticMeshComponent_temp;
	
	
	private:
	
	//UDynamicMesh* Mesh;
	
	UDynamicMesh* DynMesh;
	
	TArray<FVector3f> InitialVertices;
	
	TQueue<TUniquePtr<FRHIGPUBufferReadback>> GPUBufferReadbackQueue;
	
	TUniquePtr<FRHIGPUBufferReadback> StarTransformReadback;
	
	TArray<FVector3f> ReadBackVertices;
	
	void ConvertStaticMeshToDynamicMesh(UStaticMesh* StaticMesh,UDynamicMeshComponent* DynamicMeshComp,float Time);
	
	void DeformMesh();
	

};
