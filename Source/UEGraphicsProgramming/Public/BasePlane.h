// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePlane.generated.h"

UCLASS()
class UEGRAPHICSPROGRAMMING_API ABasePlane : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasePlane();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void AssignRenderTargetToMaterial(UTextureRenderTarget2D* inRenderTarget);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	class UStaticMeshComponent* PlaneMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
    	class UMaterialInterface* PlaneMeshMaterial;
	
	

public:	//Variables

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	TEnumAsByte<EPixelFormat> RenderTargetPixelFormat = EPixelFormat::PF_FloatRGBA;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	FIntVector2 RenderTargetResolution = FIntVector2(256, 256);
	
	
	
private: // Variables
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstance;
	
	UPROPERTY()
	UTextureRenderTarget2D* TempOutputRenderTarget;
	
	
	
private: //Functions
	
	void InitRenderTarget();
};
