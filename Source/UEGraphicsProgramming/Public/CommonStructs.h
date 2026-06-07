
#pragma once
#include "CoreMinimal.h"
#include "CommonStructs.generated.h"


struct FCircleData
{
	FVector2f Origin;
	FVector2f Velocity;
	float Radius;
};



//StructuredBuffer alignment rule
//Each element must be aligned to 16 bytes AND padded to a 16-byte multiple
//Unreal enforces this strictly.
//Internally UE rounds the element stride to 16-byte alignment, and MemoryImage validates this.

USTRUCT(BlueprintType)
struct alignas(16) FProceduralWoodData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor PaleColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor DarkColour;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Frequency;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NoiseScale;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RingScale;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Contrast;
};


/*FVector2f Origin;
FVector2f Velocity;
float Radius;*/


USTRUCT(BlueprintType)
struct alignas(16) FParticle
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector3f Position;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Life;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector3f Velocity;
};