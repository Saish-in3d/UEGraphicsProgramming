// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HISMActor.generated.h"

UCLASS()
class UEGRAPHICSPROGRAMMING_API AHISMActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHISMActor();
	
	UPROPERTY(EditInstanceOnly)
	UStaticMesh * StaticMesh;
	
	UPROPERTY(EditInstanceOnly)
	int SectionIndex = 0;
	
	UPROPERTY(EditInstanceOnly)
	int LessonIndex = 0;
	
	UPROPERTY(EditInstanceOnly)
	int InstanceCount = 0;

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

};
