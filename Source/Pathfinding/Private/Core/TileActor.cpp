// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/TileActor.h"

#include "EngineUtils.h"
#include "Algos/AStarAlgo.h"


// Sets default values
ATileActor::ATileActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
}

AAStarAlgo* ATileActor::GetAStarManager() const
{
	for (AAStarAlgo* Actor : TActorRange<AAStarAlgo>(GetWorld()))
		if (IsValid(Actor)) return Actor;
	return nullptr;
}

void ATileActor::ComputeHCost(const FIntVector2 Target)
{
	int Dx = FMath::Abs(Target.X - Position.X);
	int Dy = FMath::Abs(Target.Y - Position.Y);
	HeuristicCost = 10 * (Dx + Dy) + (14 - 2 * 10) * FMath::Min(Dx, Dy);
}

void ATileActor::UpdateCost_Implementation(int InCost){ }
