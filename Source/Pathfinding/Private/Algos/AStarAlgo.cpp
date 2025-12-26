// Fill out your copyright notice in the Description page of Project Settings.


#include "Algos/AStarAlgo.h"

#include "Core/TileActor.h"

void AAStarAlgo::ResetGrid()
{
	if (!IsValid(TileBP)) return;

	for (TObjectPtr<ATileActor>& TileActor : Grid) TileActor->Destroy();
	Grid.Reset();
	
	FActorSpawnParameters SpawnParams;
	for (int x = 0; x < GridSize.X; x++)
	{
		for (int y = 0; y < GridSize.Y; y++)
		{
			int Index = Grid.Emplace(GetWorld()->SpawnActor<ATileActor>(TileBP, FVector(x, y, 0) *
				GridSpacing, FRotator::ZeroRotator, SpawnParams));
			
			TObjectPtr<ATileActor>& Tile = Grid[Index];
			Tile->SetPosition(FIntVector2(x, y));
			Tile->SetRealCost(ALGO_MAX);
			Tile->SetID(Index);
		}
	}
}

void AAStarAlgo::SetRandomTargets()
{
	int S = -1, E = -1;
	S = FMath::RandRange(0, Grid.Num() - 1);
	do E = FMath::RandRange(0, Grid.Num() - 1);
	while (S == E);
	Grid[S]->SetAsStart();
	Grid[E]->SetAsTarget();
}


void AAStarAlgo::RunAlgo()
{
	if (!Grid.IsValidIndex(End) || !Grid.IsValidIndex(Start)) return;
	BEFORE_AGLO_STEP();
	while (!bTargetFound) AlgoStep();
}

void AAStarAlgo::RunAlgoTimestep()
{
	if (!Grid.IsValidIndex(End) || !Grid.IsValidIndex(Start)) return;
	BEFORE_AGLO_STEP();
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&AAStarAlgo::AlgoStep,
		.25f,
		true
		);
}

void AAStarAlgo::BEFORE_AGLO_STEP()
{
	for (TObjectPtr<ATileActor>& TileActor : Grid)
	{
		TileActor->ComputeHCost(Grid[End]->GetPosition());
		TileActor->SetRealCost(ALGO_MAX);
	}
	Grid[Start]->SetRealCost(0);
	Unvisited = Grid;
	bTargetFound = false;
}

void AAStarAlgo::AlgoStep()
{
	if (!Grid.IsValidIndex(End) || !Grid.IsValidIndex(Start)) return;
	
	int MinCost = ALGO_MAX, Index = 0;
	for (int i = 0; i < Unvisited.Num(); ++i) // GET MIN COST
	{
		if (Unvisited[i]->GetEstimatedCost() >=  MinCost) continue;
		
		MinCost = Unvisited[i]->GetEstimatedCost();
		Index = i;
	}
	if (!Unvisited.IsValidIndex(Index)) return;
	auto Tile = Unvisited[Index];
	Unvisited.RemoveAt(Index);
	
	for (int x = Tile->GetPosition().X - 1; x <= Tile->GetPosition().X + 1; ++x)
	{
		for (int y = Tile->GetPosition().Y - 1; y <= Tile->GetPosition().Y + 1; ++y)
		{
			if (x < 0 || x > GridSize.X - 1 || y < 0 || y > GridSize.Y - 1) continue;
			auto NeighborPos = FIntVector2(x, y);
			if (NeighborPos == Tile->GetPosition()) continue;

			int NeighborIndex = -1;
			for (int i = 0; i < Unvisited.Num(); ++i)
				if (Unvisited[i]->GetPosition() == NeighborPos)
				{
					NeighborIndex = i;
					break;
				}
			
			if (!Unvisited.IsValidIndex(NeighborIndex)) continue;
			if (!Unvisited[NeighborIndex]->CanBeVisited()) continue;
			auto Neighbor = Unvisited[NeighborIndex];
			if (Neighbor->GetStatus() != END && Neighbor->GetStatus() != WALL)
				Neighbor->GetMesh()->SetMaterial(0, NeighborMat);

			FIntVector2 Delta = Neighbor->GetPosition() - Tile->GetPosition();
			Delta.X = FMath::Abs(Delta.X);
			Delta.Y = FMath::Abs(Delta.Y);
			int MoveCost = (Delta.X + Delta.Y == 1) ? 10 : 14;
			int CostToNeighbor = Tile->GetRealCost() + MoveCost;
			if (CostToNeighbor < Neighbor->GetRealCost())
			{
				Neighbor->SetRealCost(CostToNeighbor);
				Neighbor->CameFrom(Tile->GetID());
			}
		}
	}

	if (Tile->GetPosition() == Grid[End]->GetPosition())
	{
		bTargetFound = true;
		TracePath();
		return;
	}
	if (Tile->GetPosition() != Grid[Start]->GetPosition() && Tile->GetPosition() != Grid[End]->GetPosition())
	{
		Tile->SetVisited();
	}
		Visited.Emplace(Tile);
}

void AAStarAlgo::SetTileStatus(const int ID, const ETileStatus NewStatus)
{
	if (!Grid.IsValidIndex(ID)) return; 
	auto Tile = Grid[ID];
	UMaterial* Material = DefaultMat;
	switch (NewStatus) {
	case UNVISITED:
		break;
	case VISITED:
		Material = VisitedMat;
		break;
	case WALL:
		Material = WallMat;
		Grid[ID]->SetRealCost(ALGO_MAX);
		break;
	case START:
		Material = StartMat;
		if (Grid.IsValidIndex(Start))
		{
			Grid[Start]->GetMesh()->SetMaterial(0, DefaultMat);
			Grid[Start]->SetRealCost(ALGO_MAX);
		}
		Start = ID;
		Grid[Start]->SetRealCost(0);
		break;
	case END:
		Material = EndMat;
		if (Grid.IsValidIndex(End))
		{
			Grid[End]->GetMesh()->SetMaterial(0, DefaultMat);
			Grid[End]->SetRealCost(ALGO_MAX);
		}
		End = ID;
		break;
	case PATH:
		Material = PathMat;
		break;
	}
	Tile->GetMesh()->SetMaterial(0, Material);
}

void AAStarAlgo::TracePath()
{
	auto Tile = Grid[End];
	if (!IsValid(Tile)) return;
	while (Tile != Grid[Start])
	{
		Tile->SetAsPath();
		Tile = Grid[Tile->GetOrigin()];
	}
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}
