// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#define ALGO_MAX INT_MAX

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AStarAlgo.generated.h"


enum ETileStatus : uint8;
class ATileActor;
/**
 * 
 */
UCLASS()
class PATHFINDING_API AAStarAlgo : public AActor
{
	GENERATED_BODY()
public:
	UFUNCTION(CallInEditor, Category="ASTAR")
	void ResetGrid();
	
	UFUNCTION(CallInEditor, Category="ASTAR")
	void SetRandomTargets();

	UFUNCTION(CallInEditor, Category="ASTAR")
	void RunAlgo();

	UFUNCTION(CallInEditor, Category="ASTAR")
	void RunAlgoTimestep();

	UFUNCTION(CallInEditor, Category="ASTAR")
	void BEFORE_AGLO_STEP();
	
	UFUNCTION(CallInEditor, Category="ASTAR")
	void AlgoStep();
	
	void SetTileStatus(int ID, ETileStatus NewStatus);

	void TracePath();
	
private:
	FTimerHandle TimerHandle;
	UPROPERTY(VisibleAnywhere, Category="ASTAR|DEBUG")
	TArray<TObjectPtr<ATileActor>> Grid;
	TArray<TObjectPtr<ATileActor>> Unvisited;
	TArray<TObjectPtr<ATileActor>> Visited;

	UPROPERTY(EditAnywhere, Category="ASTAR|MATERIALS")
	UMaterial* StartMat;
	UPROPERTY(EditAnywhere, Category="ASTAR|MATERIALS")
	UMaterial* EndMat;
	UPROPERTY(EditAnywhere, Category="ASTAR|MATERIALS")
	UMaterial* WallMat;
	UPROPERTY(EditAnywhere, Category="ASTAR|MATERIALS")
	UMaterial* NeighborMat;
	UPROPERTY(EditAnywhere, Category="ASTAR|MATERIALS")
	UMaterial* VisitedMat;
	UPROPERTY(EditAnywhere, Category="ASTAR|MATERIALS")
	UMaterial* PathMat;
	UPROPERTY(EditAnywhere, Category="ASTAR|MATERIALS")
	UMaterial* DefaultMat;
	
	UPROPERTY(EditAnywhere, Category="ASTAR|PROPERTIES|GRID")
	FIntVector2 GridSize = FIntVector2(3, 3);
	
	UPROPERTY(VisibleAnywhere, Category="ASTAR|DEBUG")
	int Start = -1;
	
	UPROPERTY(VisibleAnywhere, Category="ASTAR|DEBUG")
	int End = -1;

	UPROPERTY(EditAnywhere, Category = "ASTAR|PROPERTIES|SPAWN")
	TSubclassOf<ATileActor> TileBP;
	
	UPROPERTY(EditAnywhere, Category="ASTAR|PROPERTIES|GRID")
	float GridSpacing = 2.0f;

	bool bTargetFound = false;
};
