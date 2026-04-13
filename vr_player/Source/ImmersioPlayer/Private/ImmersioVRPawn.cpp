#include "ImmersioVRPawn.h"
#include "VRProjectionActor.h"
#include "Engine/World.h"

AImmersioVRPawn::AImmersioVRPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Setup Camera
	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	RootComponent = VRCamera;

	// Setup Media Backend
	MediaPlayerComponent = CreateDefaultSubobject<UCoreMediaPlayerComponent>(TEXT("MediaPlayerBackend"));
}

void AImmersioVRPawn::BeginPlay()
{
	Super::BeginPlay();

	// Spawn the Projection Actor if a class is provided
	if (ProjectionActorClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		ActiveProjectionActor = GetWorld()->SpawnActor<AVRProjectionActor>(ProjectionActorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (ActiveProjectionActor)
		{
			// Attach it to the Pawn (or root) so it moves with the player if needed,
			// though usually the projection environment is static and the player sits inside it.
			// ActiveProjectionActor->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

			// Link the media texture
			ActiveProjectionActor->SetMediaTexture(MediaPlayerComponent->GetMediaTexture());
		}
	}
}

void AImmersioVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AImmersioVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AImmersioVRPawn::PlayMedia(const FString& Url, const FString& Filename)
{
	if (ActiveProjectionActor)
	{
		ActiveProjectionActor->AutoDetectModeFromFilename(Filename);
	}

	if (MediaPlayerComponent)
	{
		MediaPlayerComponent->OpenUrl(Url);
		// Note: PlayOnOpen is false in our component, so we must explicitly play once opened,
		// or rely on a media event delegate to call play when ready.
		// For simplicity in this base version:
		MediaPlayerComponent->Play();
	}
}
