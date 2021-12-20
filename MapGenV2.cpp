// Fill out your copyright notice in the Description page of Project Settings.


#include "MapGenV2.h"

// Sets default values
AMapGenV2::AMapGenV2()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = mesh;
	// New in UE 4.17, multi-threaded PhysX cooking.
	mesh->bUseAsyncCooking = true;
}

// Called when the game starts or when spawned
void AMapGenV2::BeginPlay()
{
	Super::BeginPlay();

	GenerateMap();
	
}

// Called every frame
void AMapGenV2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!chunkQueue.IsEmpty())
	{
		FChunkData dat;
		chunkQueue.Dequeue(dat);
		SpawnMesh(dat);
	}
}

void AMapGenV2::GenerateChunk(int LOD, int xOffset, int yOffset, int index)
{
	
	TArray<FVector> chunkVerticeArray;
	for (size_t y = 0; y <= chunkSize; y = y + pow(2, LOD))
	{
		for (size_t x = 0; x <= chunkSize; x = x + pow(2, LOD))
		{
			chunkVerticeArray.Add(FVector(x*tileSize + (xOffset * (tileSize * chunkSize)), y*tileSize + (yOffset * (tileSize * chunkSize)), 0));
		}
	}
	GenerateTriangle(LOD, chunkVerticeArray, index);
}

void AMapGenV2::GenerateTriangle(int LOD, TArray<FVector> Vertices, int index)
{
	TArray<int> chunkTriangleArray;
	for (size_t y = 0; y <= chunkSize/ pow(2, LOD); y++)
	{
		for (size_t x = 0; x <= chunkSize / pow(2,LOD) -1; x++)
		{
			chunkTriangleArray.Add(x + (y * chunkSize/ pow(2,LOD)));
			chunkTriangleArray.Add(x + (y * chunkSize / pow(2,LOD)) + chunkSize/ pow(2,LOD));
			chunkTriangleArray.Add(x + (y * chunkSize / pow(2,LOD)) + chunkSize/ pow(2,LOD) + 1);

			chunkTriangleArray.Add(x + (y * chunkSize / pow(2,LOD)));
			chunkTriangleArray.Add(x + (y * chunkSize / pow(2,LOD)) + chunkSize/ pow(2,LOD) + 1);
			chunkTriangleArray.Add(x + (y * chunkSize / pow(2,LOD)) + 1);
			
		}
	}
	FChunkData data;
	data.chunkVertice = Vertices;
	data.chunkTriangles = chunkTriangleArray;
	data.index = index;
	chunkQueue.Enqueue(data);
}

void AMapGenV2::CalculateLOD(FVector playerLocation)
{
	for (size_t y = 0; y < numberOfChunks; y++)
	{
		for (size_t x = 0; x < numberOfChunks; x++)
		{
			FVector chunkCenter = { float((x + 0.5) * (chunkSize * tileSize)),float((y + 0.5) * (chunkSize * tileSize)),0 };
			UE_LOG(LogTemp, Warning, TEXT("Center: %s"), *chunkCenter.ToString());
			double lodDistance = sqrt((pow(playerLocation.X - chunkCenter.X,2) + pow(playerLocation.Y - chunkCenter.Y, 2)));
			UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), lodDistance);

			float lod = round(pow((lodDistance / (chunkSize * tileSize * numberOfChunks) * 10),1.2));
			UE_LOG(LogTemp, Warning, TEXT("LOD: %f"), lod);

			if (lod > sqrt(chunkSize))
			{
				lod = sqrt(chunkSize);
			}
			if (lastLOD.IsValidIndex((x + (y * numberOfChunks))))
			{
				if (lastLOD[(x + (y * numberOfChunks))] != lod)
				{
					lastLOD[(x + (y * numberOfChunks))] = lod;
					GenerateChunk(lod, x, y, (x + (y * numberOfChunks))); /////LOG 0 = INF = BAD!!!!!! fix it!
				}
			}
			else
			{
				lastLOD.Add(lod);
				GenerateChunk(lod, x, y, (x + (y * numberOfChunks))); /////LOG 0 = INF = BAD!!!!!! fix it!
			}
			

			
		}
	}
}

void AMapGenV2::SpawnMesh(FChunkData chunkdata)
{
	
	TArray<FVector> normals;
	

	TArray<FVector2D> UV0;
	


	TArray<FProcMeshTangent> tangents;
	

	TArray<FLinearColor> vertexColors;
	
	if (lastLOD[chunkdata.index] >= 2)
	{
		mesh->CreateMeshSection_LinearColor(chunkdata.index, chunkdata.chunkVertice, chunkdata.chunkTriangles, normals, UV0, vertexColors, tangents, false);
	}
	else
	{
		mesh->CreateMeshSection_LinearColor(chunkdata.index, chunkdata.chunkVertice, chunkdata.chunkTriangles, normals, UV0, vertexColors, tangents, true);
	}
	
	chunk++;
}

void AMapGenV2::GenerateMap()
{
	/*for (size_t y = 0; y < numberOfChunks; y++)
	{
		for (size_t x = 0;  x < numberOfChunks; x++)
		{
			GenerateChunk(LevelOfDetail, x, y, (x+(y*numberOfChunks)));
			chunklod++;
		}
	}*/
}

