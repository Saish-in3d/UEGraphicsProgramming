// Fill out your copyright notice in the Description page of Project Settings.
#include "BasePlane.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Engine/TextureRenderTarget2D.h"


// Sets default values
ABasePlane::ABasePlane()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Root
	//USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//RootComponent = Root;
	
	// Plane mesh component
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh"));
	//PlaneMesh->SetupAttachment(RootComponent);
	
	// Find Engine plane mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneAsset(
		TEXT("/Engine/BasicShapes/Plane.Plane"));
	
	if (PlaneAsset.Succeeded())
	{
		PlaneMesh->SetStaticMesh(PlaneAsset.Object);
	}
	
	
	
	

}

// Called when the game starts or when spawned
void ABasePlane::BeginPlay()
{
	Super::BeginPlay();
	
	//Loading material from plugin content folder
	UMaterialInterface* TextureImporterMaterial = LoadObject<UMaterialInterface>(nullptr,
		TEXT("/UEGraphicsProgramming/Common/Materials/M_RenderTargetVisualiser.M_RenderTargetVisualiser"));
	
	
	
	//Creating material dynamic instance
	if (!TextureImporterMaterial){return;}
	DynamicMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(),
		TextureImporterMaterial);
	if (!DynamicMaterialInstance){return;}
	if (!PlaneMesh){return;}
	PlaneMesh->SetMaterial(0, DynamicMaterialInstance);
	
	
	InitRenderTarget();
	
	if (!TempOutputRenderTarget){return;}
	
	AssignRenderTargetToMaterial(TempOutputRenderTarget);
	
}

// Called every frame
void ABasePlane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Compute shader logic that outputs to render target
	
	
	
	
}

void ABasePlane::AssignRenderTargetToMaterial(UTextureRenderTarget2D* inRenderTarget)
{
	if (!inRenderTarget && !DynamicMaterialInstance){return;}
	//Assigning the texture to the dynamic material parameter
	DynamicMaterialInstance->SetTextureParameterValue(FName("RenderTarget"), inRenderTarget);
}

void ABasePlane::InitRenderTarget()
{
	TempOutputRenderTarget = NewObject<UTextureRenderTarget2D>(this);
	
	TempOutputRenderTarget->InitCustomFormat(RenderTargetResolution.X, RenderTargetResolution.Y,
		RenderTargetPixelFormat,true);
	
	TempOutputRenderTarget->ClearColor = FLinearColor::Blue;
	TempOutputRenderTarget->bGPUSharedFlag = true;
	TempOutputRenderTarget->bAutoGenerateMips = false;
	TempOutputRenderTarget->bCanCreateUAV = true;
	
	
	//TempOutputRenderTarget->AddToRoot();
	
	TempOutputRenderTarget->UpdateResourceImmediate();
	TempOutputRenderTarget->ClearColor = FLinearColor::MakeRandomColor();
	//TempOutputRenderTarget->AddToRoot();
	TempOutputRenderTarget->UpdateResource(); // Need to do this extra step otherwise material won't update.
	
	
	
}

