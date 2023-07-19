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

#include "SmokeVolume.h"
#include "Math/TransformNonVectorized.h"
#include "Math/Vector.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASmokeVolume::ASmokeVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASmokeVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASmokeVolume::OnConstruction()
{
	Super::OnConstruction(this->GetActorTransform());
}

// Called every frame
void ASmokeVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASmokeVolume::FloodFill()
{
	// Declare local variables
	TArray<FVector> PositionQueue;
	TArray<FVector> CheckedPositions;
	TArray<FVector> AcceptedPositions;
	FVector CurrentQueryPosition;
	FVector CurrentNeighborPosition;
	int TotalEnergy = GridResolution * GridResolution * GridResolution;
	int TotalCollisions = 0;
	float CellSize = TargetVolumeDiameter / GridResolution;
	FHitResult TraceHit;

	// Declare per-index axis values for use in each position nested loop
	TArray<FVector> CheckAxes;
	CheckAxes.Add(FVector(1.0, 0.0, 0.0));
	CheckAxes.Add(FVector(-1.0, 0.0, 0.0));
	CheckAxes.Add(FVector(0.0, 1.0, 0.0));
	CheckAxes.Add(FVector(0.0, -1.0, 0.0));
	CheckAxes.Add(FVector(0.0, 0.0, 1.0));
	CheckAxes.Add(FVector(0.0, 0.0, -1.0));

	// Add first position to queue - this actor's position
	PositionQueue.Add(this->GetActorLocation());
	// Deduct one point of energy for the actor-centric starting grid position auto-assuming it can be filled
	TotalEnergy--;

	// Keep looping through positions until the position queue has been processed and emptied
	// or until the volume's energy has been depleted
	while (PositionQueue.Num() > 0 && TotalEnergy > 0)
	{
		CurrentQueryPosition = PositionQueue[0];
		if (SnapToGrid)
		{
			CurrentQueryPosition = CurrentQueryPosition.GridSnap(CellSize);
		}

		for (int i = 0; i < 6; i++)
		{
			CurrentNeighborPosition = CurrentQueryPosition + (CheckAxes[i] * FVector(CellSize));
			if (!CheckedPositions.Contains(CurrentNeighborPosition))
			{
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Line trace from current query position to current neighbor position to see if path is obstructed				//
				// if not obstructed, the current neighbor position gets added into AcceptedPositions, CheckedPositions,		//
				// and the PositionQueue for subsequent processing. If it is obstructed, it's only added to CheckedPositions	//
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				
				static const TArray<AActor*> NullArray;
				UKismetSystemLibrary::LineTraceSingle(this, CurrentQueryPosition, CurrentNeighborPosition, TraceTypeQuery1, false, NullArray, EDrawDebugTrace::None, TraceHit, true);

				if (!TraceHit.bBlockingHit)
				{
					PositionQueue.Add(CurrentNeighborPosition);
					AcceptedPositions.Add(CurrentNeighborPosition);
					CheckedPositions.Add(CurrentNeighborPosition);
					TotalEnergy--;
				}
				else
				{
					TotalCollisions++;
				}
				
			}
		}
		// Remove index 0 from Position Queue, shifting it up so that the next grab of index 0 is the next value in the array,
		// and so that the array shrinks as we go along
		PositionQueue.RemoveAt(0, 1, true);
	}

	if (SmoothGridPositions)
	{
		VoxelPositions = SmoothPointsToSphere(AcceptedPositions, TotalCollisions);
	}
	else
	{
		VoxelPositions = AcceptedPositions;
	}
	
	if (DebugDrawTime > 0.0)
	{	static const UWorld* World = GetWorld();
		static const FColor Color = FColor(0.0, 0.66 * 255, 0.66 * 255);
		FlushPersistentDebugLines(World);
		for (FVector VoxelPosition : VoxelPositions)
		{
			//DrawDebugBox(World, VoxelPosition, FVector(CellSize * 0.5), FColor(0.0, 0.66 * 255, 0.66 * 255),false, DebugDrawTime, 0, 2.5);
			DrawDebugPoint(World, VoxelPosition, (CellSize * 0.33), Color, false, DebugDrawTime, 0);
		}
	}
	
}

TArray<FVector> ASmokeVolume::SmoothPointsToSphere(TArray<FVector> GridPoints, int CollisionCount)
{
	// Declare local variables
	TArray<float> PointLengths;
	float LengthAverage = 0.0f;
	TArray<FVector> AveragedPoints;

	// Get point lengths
	for(FVector GridPoint:GridPoints)
	{
		FVector TransformedPoint = this->GetTransform().InverseTransformPosition(GridPoint);
		float TempPoint = TransformedPoint.Length();
		PointLengths.Add(TempPoint);
	}

	// Get Length Average
	for(float Length:PointLengths)
	{
		LengthAverage = Length + LengthAverage;
	}

	// Modulate Length Average by collision count; this helps the volume "grow" under spatial constraints
	float PointCount = float(PointLengths.Num());
	float CollisionCountMultiplier = float(FMath::Clamp((CollisionCount - PointLengths.Num()), 0, (CollisionCount - PointLengths.Num())));
	LengthAverage = (LengthAverage / PointCount) + (LengthAverage * CollisionCountMultiplier);

	// If point falls within averaged locations length, add it to the array, otherwise don't
	// This prevents the "outer shell" of the grid sphere from stacking up with unnecessary near-neighbor positions
	for (FVector GridPoint:GridPoints)
	{
		FVector TransformedPoint = this->GetTransform().InverseTransformPosition(GridPoint);
		FVector Direction = TransformedPoint.GetUnsafeNormal();
		float DistanceThreshold = (LengthAverage / TransformedPoint.Length() * TransformedPoint.Length());
		if (TransformedPoint.Length() <= DistanceThreshold)
		{
			FVector AveragedPoint = this->GetTransform().TransformPosition(Direction * TransformedPoint.Length());
			AveragedPoints.Add(AveragedPoint);
		}
	}
	return AveragedPoints;
}

