// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Algos/AStarAlgo.h"
#include "GameFramework/Actor.h"
#include "TileActor.generated.h"

class AAStarAlgo;

UENUM()
enum ETileStatus : uint8
{
	UNVISITED = 0,
	START = 1,
	END = 2,
	VISITED = 3,
	WALL = 4,
	PATH = 5
};

UCLASS()
class PATHFINDING_API ATileActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATileActor();

	UStaticMeshComponent* GetMesh() { return Mesh; }

	UFUNCTION(BlueprintNativeEvent, BlueprintNativeEvent)
	void UpdateCost(int InCost);

	UFUNCTION(CallInEditor, Category="TILE")
	void SetAsWall() 
	{
		GetAStarManager()->SetTileStatus(ID, Status = WALL);
	};
	UFUNCTION(CallInEditor, Category="TILE")
	void SetAsStart()
	{
		GetAStarManager()->SetTileStatus(ID, Status = START);
	};
	UFUNCTION(CallInEditor, Category="TILE")
	void SetAsTarget()
	{
		GetAStarManager()->SetTileStatus(ID, Status = END);
	};
	void SetAsPath()
	{
		if (Status == START || Status == END || Status == WALL) return;
		GetAStarManager()->SetTileStatus(ID, Status = PATH);
	}
	void SetVisited()
	{
		if (Status == START || Status == END || Status == WALL) return;
		GetAStarManager()->SetTileStatus(ID, Status = VISITED);
	}
	bool CanBeVisited() const { return Status < VISITED && Status != START;}

	AAStarAlgo* GetAStarManager() const;

	void ComputeHCost(FIntVector2 Target);
	void SetRealCost(const int NewValue)
	{
		RealCost = NewValue;
		EstimatedCost = RealCost == ALGO_MAX ? ALGO_MAX : RealCost + HeuristicCost;		
		UpdateCost(RealCost);
	}
	int GetEstimatedCost() const { return EstimatedCost; };
	int GetRealCost() const { return RealCost; };
	void SetID(const int NewID) { ID = NewID; }
	int GetID() const { return ID; }
	
	void SetPosition(const FIntVector2 NewPos) { Position = NewPos; };
	FIntVector2 GetPosition() const { return Position; }

	void CameFrom(const int NewID) { OriginID = NewID; }
	int GetOrigin() const { return OriginID; }
	ETileStatus GetStatus() const {return Status;}
private:
	ETileStatus Status = ETileStatus::UNVISITED;
	UPROPERTY(VisibleAnywhere)
	FIntVector2 Position = FIntVector2(0, 0);

	int EstimatedCost = -1;
	int HeuristicCost = -1;
	int RealCost = -1;
	int OriginID = -1;
	int ID = -1;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;
};
