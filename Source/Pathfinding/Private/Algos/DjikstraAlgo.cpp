// Fill out your copyright notice in the Description page of Project Settings.


#include "Algos/DjikstraAlgo.h"

#include "Core/WaypointActor.h"


// Sets default values
ADjikstraAlgo::ADjikstraAlgo()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ADjikstraAlgo::BeginPlay()
{
	Super::BeginPlay();

	StartPosition = FIntVector2(FMath::RandRange(0, GridSize.X - 1), FMath::RandRange(0, GridSize.Y - 1));
	TargetPosition = FIntVector2(FMath::RandRange(0, GridSize.X - 1), FMath::RandRange(0, GridSize.Y - 1));
	
	FActorSpawnParameters SpawnParams;
	for (int X = 0; X < GridSize.X; ++X)
	{
		for (int Y = 0; Y < GridSize.Y; ++Y)
		{
			FWaypoint NewPoint;
			NewPoint.SetCoordinates(FVector2D(X, Y));
			NewPoint.SetCost(INT_MAX);
			Grid.Emplace(NewPoint);			
			int Index = GridActors.Emplace(GetWorld()->SpawnActor<AWaypointActor>(WaypointBP, FVector(X, Y, 0) * SpawnOffset, FRotator::ZeroRotator, SpawnParams));
			if (X == StartPosition.X && Y == StartPosition.Y)
			{
				Grid[Index].SetCost(0);
				GridActors[Index]->UpdateCost(0);
				GridActors[Index]->GetMesh()->SetMaterial(0, StartMat);
			}
			if (X == TargetPosition.X && Y == TargetPosition.Y) GridActors[Index]->GetMesh()->SetMaterial(0, TargetMat);
			Grid[Index].SetID(Index);
		}
	}

	DrawDebugBox(GetWorld(), FVector(StartPosition.X, StartPosition.Y, 0) * SpawnOffset, FVector(50, 50, 1), FColor::Green, true, -1, 0, 2);
	DrawDebugBox(GetWorld(), FVector(TargetPosition.X, TargetPosition.Y, 0) * SpawnOffset, FVector(50, 50, 1), FColor::Magenta, true, -1, 0, 2);

	CalculateGridCost();
}

void ADjikstraAlgo::CalculateGridCost()
{
	for (int i = 0; i < Grid.Num(); ++i) Unvisited.Add(i);
	while (Unvisited.Num() > 0)
	{
		int CurrentIndex = GetMinIndex(Unvisited);
		FWaypoint& Current = Grid[CurrentIndex];
		Current.SetVisited(true);
		Unvisited.Remove(CurrentIndex);
		for (int X = Current.GetPosition().X - 1; X <= Current.GetPosition().X + 1; ++X)
		{
			for (int Y = Current.GetPosition().Y - 1; Y <= Current.GetPosition().Y + 1; ++Y)
			{
				if (X < 0 || X > GridSize.X - 1 || Y < 0 || Y > GridSize.Y - 1) continue;

				FIntVector2 NeighborPosition = FIntVector2(X, Y);
				if (NeighborPosition == Current.GetPosition()) continue; // Skip checking current tile.

				// Get Neighbor
				int NeighborIndex = -1;
				for (auto Index : Unvisited)
					if (Grid[Index].GetPosition() == NeighborPosition)
					{
						NeighborIndex = Index;
						break;
					}
				if (NeighborIndex == -1) continue;
				FWaypoint& Neighbor = Grid[NeighborIndex];
				if (Neighbor.Visited()) continue;
				
				FIntVector2 Delta = NeighborPosition - Current.GetPosition();
				Delta.X = FMath::Abs(Delta.X);
				Delta.Y = FMath::Abs(Delta.Y);	
				int MoveCost = (Delta.X + Delta.Y == 1) ? 10 : 14;
				int CostToNeighbor = Current.GetCost() + MoveCost;
				if (CostToNeighbor < Neighbor.GetCost())
				{
					Neighbor.SetCost(CostToNeighbor);
					GridActors[Neighbor.GetID()]->UpdateCost(CostToNeighbor);
					Neighbor.SetOrigin(Current.GetPosition());
				}
			}
		}
		Visited.Add(CurrentIndex);
	}
	TraceRoute();
}

void ADjikstraAlgo::TraceRoute()
{
	// Find the tile that matches TargetPosition in Visited
	FWaypoint* CurrentTile = &Grid[Visited[0]]; // default
	if (int* FoundIndex = Visited.FindByPredicate([&](int Index) {
			return Grid[Index].GetPosition() == TargetPosition;
		}))
	{
		CurrentTile = &Grid[*FoundIndex];
	}

	// Trace back to StartPosition
	while (CurrentTile->GetPosition() != StartPosition)
	{
		// Find the origin tile in Visited
		int* Previous = Visited.FindByPredicate([&](int Index) {
			return Grid[Index].GetPosition() == CurrentTile->GetOrigin();
		});

		if (!Previous) break; 

		CurrentTile = &Grid[*Previous];
		if (CurrentTile->GetPosition() != StartPosition) GridActors[*Previous]->GetMesh()->SetMaterial(0, PathMat);
	}
}

int ADjikstraAlgo::GetMinIndex(TArray<int32>& Array)
{
	int MinIndex = Array[0];
	for (auto Index : Array)
		if (Grid[MinIndex].GetCost() > Grid[Index].GetCost()) MinIndex = Index;
	return MinIndex;
}
