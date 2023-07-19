/*
MIT License

Copyright (c) [2023] [Christian Sparks]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

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
