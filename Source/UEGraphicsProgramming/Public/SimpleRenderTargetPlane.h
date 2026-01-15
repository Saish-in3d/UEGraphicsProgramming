// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlane.h"
#include "CommonStructs.h"
#include "SimpleRenderTargetPlane.generated.h"

/**
 * 
 */

UCLASS(Blueprintable)
class UEGRAPHICSPROGRAMMING_API ASimpleRenderTargetPlane : public ABasePlane
{
	GENERATED_BODY()
	
	
public:	
	// Sets default values for this actor's properties
	ASimpleRenderTargetPlane();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

	
	

public:	//Variables

	UPROPERTY(EditInstanceOnly)
	int SectionIndex = 0;
	
	UPROPERTY(EditInstanceOnly)
	int LessonIndex = 0;
	
	
	
private: // Variables

	bool S2_L2_BufferCreated = false;
	TArray<FCircleData> CircleDataBuffer;
	
	
private: //Functions
	

	
	
};
