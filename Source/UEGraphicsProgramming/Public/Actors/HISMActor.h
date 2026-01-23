// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RHIGPUReadback.h"
#include "HISMActor.generated.h"

UCLASS()
class UEGRAPHICSPROGRAMMING_API AHISMActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHISMActor();
	
	UPROPERTY(EditInstanceOnly, meta = (category = "ActorDefaults"))
	UStaticMesh * HISMStaticMesh;
	
	UPROPERTY(EditInstanceOnly, meta = (category = "ActorDefaults"))
	int SectionIndex = 0;
	
	UPROPERTY(EditInstanceOnly, meta = (category = "ActorDefaults"))
	int LessonIndex = 0;
	
	UPROPERTY(EditInstanceOnly, meta = (category = "ActorDefaults"))
	int InstanceCount = 1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool InitHISM(int instanceCount);
	
	void UpdateHISMTransforms(TArray<FTransform> inTransformsArray);

	virtual void OnConstruction(const FTransform& Transform) override;

	
	UPROPERTY(VisibleAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	TArray<FVector3f> TransformsArray;
	
private:

	TUniquePtr<FRHIGPUBufferReadback> StarTransformReadback;
	
};
