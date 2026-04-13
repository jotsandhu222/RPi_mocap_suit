#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "CoreMediaPlayerComponent.h"
#include "ImmersioVRPawn.generated.h"

class AVRProjectionActor;

/**
 * The main VR Pawn for ImmersioPlayer.
 * It contains the camera (player view) and coordinates between the CoreMedia component,
 * the VRRendering projection actor, and the UI.
 */
UCLASS()
class IMMERSIOPLAYER_API AImmersioVRPawn : public APawn
{
	GENERATED_BODY()

public:
	AImmersioVRPawn();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** The VR Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	UCameraComponent* VRCamera;

	/** The media player backend */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Media")
	UCoreMediaPlayerComponent* MediaPlayerComponent;

	/** Class of the projection actor to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "VR Media")
	TSubclassOf<AVRProjectionActor> ProjectionActorClass;

	/** The spawned projection actor */
	UPROPERTY(BlueprintReadOnly, Category = "VR Media")
	AVRProjectionActor* ActiveProjectionActor;

	/** Command to load and play a video */
	UFUNCTION(BlueprintCallable, Category = "VR Media")
	void PlayMedia(const FString& Url, const FString& Filename);
};
