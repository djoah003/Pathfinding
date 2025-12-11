// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaypointActor.generated.h"

UCLASS()
class PATHFINDING_API AWaypointActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWaypointActor();

	UStaticMeshComponent* GetMesh() { return Mesh; }
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;
};
