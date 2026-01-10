// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlane.h"
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
	int LessonIndex = 0;
	
	
	
private: // Variables

	
	
	
private: //Functions
	

	
	
};
