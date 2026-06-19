
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


//One vertex of the mesh the S3L2 boids are drawn with.
//The source static mesh is de-indexed into a flat array of these at init time, so the draw needs
//no index buffer and no vertex declaration - the vertex shader pulls straight out of a
//StructuredBuffer using SV_VertexID, the same trick S3L1 uses for its quads.
//float3 + float + float3 + float = exactly 32 bytes, matching the HLSL struct stride.
USTRUCT(BlueprintType)
struct alignas(16) FBoidMeshVertex
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector3f Position;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Padding0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector3f Normal;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Padding1;
};


//One agent of the S3L2 boid simulation.
//Laid out as float3 + float + float3 + float = exactly 32 bytes, so the CPU struct and the HLSL
//struct agree on the element stride without the compiler having to insert any tail padding of its
//own. Speed is cached by the compute shader so the game thread can orient the instanced mesh
//without recomputing a length() per boid.
USTRUCT(BlueprintType)
struct alignas(16) FBoid
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector3f Position;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector3f Velocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Padding;
};


USTRUCT(BlueprintType)
struct alignas(16) FParticle
{
	GENERATED_BODY()
	
	//This is a screen space position where z = depth mostly likely a fixed value in this case
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector3f Position;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Life;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector3f Velocity;
};