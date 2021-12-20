// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Containers/Queue.h"
#include "MapGenV2.generated.h"

USTRUCT()
struct FChunkData {
	GENERATED_BODY()
	TArray<FVector> chunkVertice;
	TArray<int> chunkTriangles;
	int index;
};

UCLASS()
class MAPGENERATIONV2_API AMapGenV2 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapGenV2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere) 
	int numberOfChunks = 1;
	UPROPERTY(EditAnywhere)
	int chunkSize = 32;
	UPROPERTY(EditAnywhere)
	float tileSize = 100;
	UPROPERTY(EditAnywhere)
	int LevelOfDetail = 4;

	TArray<int> lastLOD;
	int chunklod = 0;
	UProceduralMeshComponent* mesh;

	

	

	TArray<int> LODFactors = { 1, 2, 4, 8, 16, 32, 64 };

	TQueue<FChunkData> chunkQueue;

	int chunk;

	void GenerateChunk(int LOD, int xOffset, int yOffset, int index);

	void GenerateTriangle(int LOD, TArray<FVector> Vertices, int index);

	UFUNCTION(BlueprintCallable)
	void CalculateLOD(FVector playerLocation);

	void SpawnMesh(FChunkData chunkdata);

	void GenerateMap();




};
