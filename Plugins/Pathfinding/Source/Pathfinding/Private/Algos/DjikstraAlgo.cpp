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

	FActorSpawnParameters SpawnParams;
	for (int X = 0; X < GridSize.X; ++X)
	{
		for (int Y = 0; Y < GridSize.Y; ++Y)
		{
			FWaypoint NewPoint;
			NewPoint.SetCoordinates(FVector2D(X, Y));
			NewPoint.SetCost(INT_MAX);
			Grid.Emplace(NewPoint);			
			int Index = GridActors.Emplace(GetWorld()->SpawnActor<AWaypointActor>(WaypointBP, NewPoint.GetFCoordinates() * SpawnOffset, FRotator::ZeroRotator, SpawnParams));
			if (X == StartPosition.X && Y == StartPosition.Y)
			{
				Grid[Index].SetCost(0);
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
	Unvisited = Grid;
	while (Unvisited.Num() > 0)
	{
		int MinCost = GetMinCost(Unvisited);
		FWaypoint Point;
		GetWaypoint(Point, Unvisited, MinCost);
		Unvisited.RemoveSwap(Point);

		for (int X = Point.GetFCoordinates().X - 1; X < Point.GetFCoordinates().X + 1; ++X)
		{
			for (int Y = Point.GetFCoordinates().Y - 1; Y < Point.GetFCoordinates().Y + 1; ++Y)
			{
				if (X < 0 || X > GridSize.X - 1 || Y < 0 || Y > GridSize.Y - 1) continue;

				FIntVector2 NeighborPointCoords = FIntVector2(X, Y);
				if (NeighborPointCoords == Point.GetICoordinates()) continue; // Skip checking current tile.

				// Get Neighbor
				int NeighborIndex; // <--- TODO: CHANGE TO INDEX
				GetNeighbor(NeighborIndex, Unvisited, NeighborPointCoords);
				if (NeighborIndex < 0) return;
				
				FIntVector2 Delta = NeighborPointCoords - Point.GetICoordinates();
				Delta.X = FMath::Abs(Delta.X);
				Delta.Y = FMath::Abs(Delta.Y);	
				int MoveCost = (Delta.X + Delta.Y == 1) ? 10 : 14;
				int CostToNeighbor = Point.GetCost() + MoveCost;
				if (CostToNeighbor < Grid[NeighborIndex].GetCost())
				{
					Grid[NeighborIndex].SetCost(CostToNeighbor);
					OnCostSet.Broadcast(CostToNeighbor); // <--- TODO: CHANGE TO INTERFACE
					Grid[NeighborIndex].SetOrigin(Point.GetICoordinates());
				}
			}
		}
	}
}

void ADjikstraAlgo::GetWaypoint(FWaypoint& OutWaypoint, TArray<FWaypoint>& Waypoints, const int Cost)
{
	for (auto& Point : Waypoints)
	{
		if (Point.GetCost() == Cost)
		{
			OutWaypoint = Point;
			return;
		}
	}
}

void ADjikstraAlgo::GetNeighbor(FWaypoint& OutWaypoint, TArray<FWaypoint>& Waypoints, FIntVector2 Coordinates)
{
	for (auto& Waypoint : Waypoints)
	{
		if (Waypoint.GetICoordinates() == Coordinates)
		{
			OutWaypoint = Waypoint;
			return;
		}
	}
}

int ADjikstraAlgo::GetMinCost(TArray<FWaypoint>& Waypoints)
{
	int Min = INT_MAX;
	for (auto& Point : Waypoints)
	{
		const int Cost = Point.GetCost();
		if (Cost < Min) Min = Cost;
	}
	return Min;
}
