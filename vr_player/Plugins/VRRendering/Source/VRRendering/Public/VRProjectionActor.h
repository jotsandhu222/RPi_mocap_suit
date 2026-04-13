#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "MediaTexture.h"
#include "VRProjectionActor.generated.h"

UENUM(BlueprintType)
enum class EVRProjectionMode : uint8
{
	FlatScreen UMETA(DisplayName="Flat Screen"),
	CurvedScreen UMETA(DisplayName="Curved Screen"),
	Immersive180 UMETA(DisplayName="180 Degree"),
	Immersive360 UMETA(DisplayName="360 Degree")
};

/**
 * Actor responsible for managing the physical projection surface for media playback.
 * It handles swapping meshes (e.g. Plane, Hemisphere, Sphere) based on the current projection mode
 * and assigns the correct dynamic material instance to display the MediaTexture.
 */
UCLASS()
class VRRENDERING_API AVRProjectionActor : public AActor
{
	GENERATED_BODY()

public:
	AVRProjectionActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	/** The mesh component used to display the video */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Projection")
	UStaticMeshComponent* ProjectionMesh;

	/** The current projection mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Projection", meta=(ExposeOnSpawn="true"))
	EVRProjectionMode CurrentMode;

	/** Set the projection mode and update mesh/material accordingly */
	UFUNCTION(BlueprintCallable, Category = "VR Projection")
	void SetProjectionMode(EVRProjectionMode NewMode);

	/** Set the media texture to be displayed on the projection surface */
	UFUNCTION(BlueprintCallable, Category = "VR Projection")
	void SetMediaTexture(UMediaTexture* InMediaTexture);

	/** Auto-detect mode based on a filename (e.g., looks for _180, _360, _SBS) */
	UFUNCTION(BlueprintCallable, Category = "VR Projection")
	void AutoDetectModeFromFilename(const FString& Filename);

protected:

	/** Pointers to the specific meshes (to be set in a Blueprint subclass) */
	UPROPERTY(EditDefaultsOnly, Category = "VR Projection|Meshes")
	UStaticMesh* FlatMesh;

	UPROPERTY(EditDefaultsOnly, Category = "VR Projection|Meshes")
	UStaticMesh* CurvedMesh;

	UPROPERTY(EditDefaultsOnly, Category = "VR Projection|Meshes")
	UStaticMesh* HemisphereMesh; // For 180

	UPROPERTY(EditDefaultsOnly, Category = "VR Projection|Meshes")
	UStaticMesh* SphereMesh; // For 360

	/** Pointer to the master material to use for rendering */
	UPROPERTY(EditDefaultsOnly, Category = "VR Projection|Materials")
	UMaterialInterface* BaseMaterial;

private:

	void UpdateMeshForMode();

	/** The dynamic material instance we create to pass the media texture to */
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* DynamicMaterial;
};
