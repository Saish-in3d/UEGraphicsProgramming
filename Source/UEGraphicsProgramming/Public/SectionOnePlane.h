// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlane.h"
#include "SectionOnePlane.generated.h"

/**
 * 
 */
UCLASS()
class UEGRAPHICSPROGRAMMING_API ASectionOnePlane : public ABasePlane
{
	GENERATED_BODY()
	
	
public:	
	// Sets default values for this actor's properties
	ASectionOnePlane();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

	
	

public:	//Variables


	
	
	
private: // Variables

	
	
	
private: //Functions
	

	
	
};
