#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "CityGenerator.generated.h"

UCLASS()
class PROCEDURALCITYBUILDER_API ACityGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACityGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    // Generates the city (Callable from the Editor Details Panel)
    UFUNCTION(CallInEditor, Category = "City Generation")
    void GenerateCity();

    UFUNCTION(CallInEditor, Category = "City Generation")
    void ClearCity();

    // --- Components ---

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "City Generation|Components")
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "City Generation|Components")
    UHierarchicalInstancedStaticMeshComponent* HISM_Roads;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "City Generation|Components")
    UHierarchicalInstancedStaticMeshComponent* HISM_Sidewalks;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "City Generation|Components")
    UHierarchicalInstancedStaticMeshComponent* HISM_BaseFloors;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "City Generation|Components")
    UHierarchicalInstancedStaticMeshComponent* HISM_MidFloors;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "City Generation|Components")
    UHierarchicalInstancedStaticMeshComponent* HISM_Roofs;

    // --- Parameters ---

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City Generation|Layout")
    int32 GridSizeX = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City Generation|Layout")
    int32 GridSizeY = 10;

    // How far apart roads are (in grid cells)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City Generation|Layout")
    int32 RoadSpacing = 4;

    // The physical size of one grid unit (in Unreal Units / cm)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City Generation|Layout")
    float CellSize = 1000.0f;

    // Probability (0.0 to 1.0) that a lot is empty instead of having a building
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City Generation|Density")
    float EmptyLotChance = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City Generation|Buildings")
    int32 MinFloors = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City Generation|Buildings")
    int32 MaxFloors = 10;

    // Height of each floor (in Unreal Units) to stack them correctly
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City Generation|Buildings")
    float FloorHeight = 400.0f;

    // Seed for reproducible generation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City Generation|System")
    int32 RandomSeed = 12345;
};
