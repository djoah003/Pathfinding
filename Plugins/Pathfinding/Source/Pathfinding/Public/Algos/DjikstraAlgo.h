// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DjikstraAlgo.generated.h"

class AWaypointActor;
static FColor START = FColor::Green;
static FColor TARGET = FColor::Red;
static FColor EDGE = FColor::Magenta;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCostChangedSignature, int, NewCost);

USTRUCT()
struct FWaypoint
{
	GENERATED_BODY()
public:
	void SetCost(const int NewCost) { Cost = NewCost; }
	int GetCost() const { return Cost; }

	void SetID(const int NewID) { ID = NewID; }
	int GetID() const { return ID; }
	
	void SetCoordinates(const FVector2D& NewCoords)
	{
		Coordinates = FIntVector2(FMath::FloorToInt(NewCoords.X), FMath::FloorToInt(NewCoords.Y));
	}
	void SetOrigin(const FIntVector2& NewCoords)
	{
		Previous = NewCoords;
	}
	
	FVector GetFCoordinates() const { return FVector(Coordinates.X, Coordinates.Y, 0); }
	FIntVector2 GetICoordinates() const { return Coordinates; }

	
	bool operator==(const FWaypoint& Other) const
	{
		return ID == Other.ID;
	}
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
private:
	static void GetWaypoint(FWaypoint& OutWaypoint, TArray<FWaypoint>& Waypoints, int Cost);
	static void GetNeighbor(FWaypoint& OutWaypoint, TArray<FWaypoint>& Waypoints, FIntVector2 Coordinates);
	static int GetMinCost(TArray<FWaypoint>& Waypoints);

	UPROPERTY(BlueprintAssignable)
	FOnCostChangedSignature OnCostSet;
	
	UPROPERTY(EditAnywhere, Category = "SETTING|MATERIALS")
	UMaterial* StartMat;
	UPROPERTY(EditAnywhere, Category = "SETTING|MATERIALS")
	UMaterial* TargetMat;
	
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
	TArray<FWaypoint> Unvisited;
};
