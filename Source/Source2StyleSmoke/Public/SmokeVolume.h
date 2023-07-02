// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SmokeVolume.generated.h"

UCLASS()
class SOURCE2STYLESMOKE_API ASmokeVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASmokeVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the actor is constructed
	virtual void OnConstruction();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool FloodFillOnConstruct = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool SmoothGridPositions = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool ShowParticles = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int GridResolution = 6;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TargetVolumeDiameter = 300.0;

	UPROPERTY(EditAnywhere)
	bool SnapToGrid = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DebugDrawTime = 0.0;

	UPROPERTY(BlueprintReadOnly)
	TArray<FVector> VoxelPositions;

	UFUNCTION(BlueprintCallable)
	void FloodFill();

	TArray<FVector> SmoothPointsToSphere(TArray<FVector> GridPoints, int CollisionCount);

};
