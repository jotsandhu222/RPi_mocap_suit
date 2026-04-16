#include "CityGenerator.h"
#include "Components/SceneComponent.h"
#include "Math/RandomStream.h"

// Sets default values
ACityGenerator::ACityGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    HISM_Roads = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HISM_Roads"));
    HISM_Roads->SetupAttachment(Root);

    HISM_Sidewalks = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HISM_Sidewalks"));
    HISM_Sidewalks->SetupAttachment(Root);

    HISM_BaseFloors = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HISM_BaseFloors"));
    HISM_BaseFloors->SetupAttachment(Root);

    HISM_MidFloors = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HISM_MidFloors"));
    HISM_MidFloors->SetupAttachment(Root);

    HISM_Roofs = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HISM_Roofs"));
    HISM_Roofs->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ACityGenerator::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACityGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACityGenerator::ClearCity()
{
    HISM_Roads->ClearInstances();
    HISM_Sidewalks->ClearInstances();
    HISM_BaseFloors->ClearInstances();
    HISM_MidFloors->ClearInstances();
    HISM_Roofs->ClearInstances();
}

void ACityGenerator::GenerateCity()
{
    ClearCity();

    FRandomStream RNG(RandomSeed);

    for (int32 X = 0; X < GridSizeX; ++X)
    {
        for (int32 Y = 0; Y < GridSizeY; ++Y)
        {
            // Determine position
            FVector Location(X * CellSize, Y * CellSize, 0.0f);

            // Random rotation in 90 degree increments to add variation
            float RandomYaw = RNG.RandRange(0, 3) * 90.0f;
            FRotator Rotation(0.0f, RandomYaw, 0.0f);

            FTransform InstanceTransform(Rotation, Location, FVector::OneVector);

            // Simple Logic: Is this a road?
            bool bIsRoad = (X % RoadSpacing == 0) || (Y % RoadSpacing == 0);

            if (bIsRoad)
            {
                HISM_Roads->AddInstance(InstanceTransform);
            }
            else
            {
                // Is this lot empty?
                if (RNG.FRand() < EmptyLotChance)
                {
                    // Empty lot - maybe add a sidewalk or park area instead
                    HISM_Sidewalks->AddInstance(InstanceTransform);
                }
                else
                {
                    // Build a building
                    int32 Floors = RNG.RandRange(MinFloors, MaxFloors);

                    for (int32 F = 0; F < Floors; ++F)
                    {
                        FVector FloorLocation = Location + FVector(0.0f, 0.0f, F * FloorHeight);
                        FTransform FloorTransform(Rotation, FloorLocation, FVector::OneVector);

                        if (F == 0)
                        {
                            HISM_BaseFloors->AddInstance(FloorTransform);
                        }
                        else if (F == Floors - 1)
                        {
                            HISM_Roofs->AddInstance(FloorTransform);
                        }
                        else
                        {
                            HISM_MidFloors->AddInstance(FloorTransform);
                        }
                    }
                }
            }
        }
    }
}
