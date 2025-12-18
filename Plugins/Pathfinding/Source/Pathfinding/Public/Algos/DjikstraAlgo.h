// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DjikstraAlgo.generated.h"

class AWaypointActor;
static FColor START = FColor::Green;
static FColor TARGET = FColor::Red;
static FColor EDGE = FColor::Magenta;

USTRUCT()
struct FWaypoint
{
	GENERATED_BODY()
public:
	FWaypoint() = default;
	void SetCost(const int NewCost) { Cost = NewCost; }
	int GetCost() const { return Cost; }
	FIntVector2 GetOrigin() const { return Previous; }
	bool Visited() const { return IsVisited; }

	void SetID(const int NewID) { ID = NewID; }
	int GetID() const { return ID; }
	
	void SetCoordinates(const FVector2D& NewCoords) { Coordinates = FIntVector2(FMath::FloorToInt(NewCoords.X), FMath::FloorToInt(NewCoords.Y)); }
	void SetOrigin(const FIntVector2& NewCoords){ Previous = NewCoords; }
	void SetVisited(const bool Value) { IsVisited = Value; } 
	
	FIntVector2 GetPosition() const { return Coordinates; }
private:
	FIntVector2 Coordinates = {-1, -1};
	FIntVector2 Previous = {-1, -1};
	int Cost = -1;
	int ID = -1;
	bool IsVisited = false;
};

UCLASS()
class PATHFINDING_API ADjikstraAlgo : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADjikstraAlgo();
	virtual void BeginPlay() override;
	void CalculateGridCost();
	void TraceRoute();
private:
	int GetMinIndex(TArray<int32>& Array);

	UPROPERTY(EditAnywhere, Category = "SETTING|MATERIALS")
	UMaterial* StartMat;
	UPROPERTY(EditAnywhere, Category = "SETTING|MATERIALS")
	UMaterial* TargetMat;
	UPROPERTY(EditAnywhere, Category = "SETTING|MATERIALS")
	UMaterial* PathMat;
	
	UPROPERTY(EditAnywhere, Category = "SETTINGS|SPAWN")
	FIntVector2 GridSize;

	UPROPERTY(EditAnywhere, Category = "SETTINGS|SPAWN")
	TSubclassOf<AWaypointActor> WaypointBP;
	
	UPROPERTY(EditAnywhere, Category = "SETTINGS|PATHFINDING")
	FIntVector2 StartPosition;
	
	UPROPERTY(EditAnywhere, Category = "SETTINGS|PATHFINDING")
	FIntVector2 TargetPosition;

	UPROPERTY(EditAnywhere, Category = "SETTINGS|SPAWN")
	int SpawnOffset;
	
	TArray<FWaypoint> Grid;
	TArray<TObjectPtr<AWaypointActor>> GridActors;
	TArray<int32> Unvisited, Visited;
};
