#include "CoreMediaPlayerComponent.h"

UCoreMediaPlayerComponent::UCoreMediaPlayerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	MediaPlayer = nullptr;
	MediaTexture = nullptr;
}

void UCoreMediaPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeDefaultMediaComponents();
}

void UCoreMediaPlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCoreMediaPlayerComponent::InitializeDefaultMediaComponents()
{
	if (!MediaPlayer)
	{
		MediaPlayer = NewObject<UMediaPlayer>(this, TEXT("InternalMediaPlayer"));
		MediaPlayer->PlayOnOpen = false;
	}

	if (!MediaTexture)
	{
		MediaTexture = NewObject<UMediaTexture>(this, TEXT("InternalMediaTexture"));
		if (MediaPlayer)
		{
			MediaTexture->SetMediaPlayer(MediaPlayer);
		}
	}
}

bool UCoreMediaPlayerComponent::OpenUrl(const FString& Url)
{
	if (MediaPlayer)
	{
		return MediaPlayer->OpenUrl(Url);
	}
	return false;
}

void UCoreMediaPlayerComponent::Play()
{
	if (MediaPlayer)
	{
		MediaPlayer->Play();
	}
}

void UCoreMediaPlayerComponent::Pause()
{
	if (MediaPlayer)
	{
		MediaPlayer->Pause();
	}
}

void UCoreMediaPlayerComponent::Stop()
{
	if (MediaPlayer)
	{
		MediaPlayer->Close();
	}
}

void UCoreMediaPlayerComponent::Seek(FTimespan Time)
{
	if (MediaPlayer)
	{
		MediaPlayer->Seek(Time);
	}
}

bool UCoreMediaPlayerComponent::IsPlaying() const
{
	if (MediaPlayer)
	{
		return MediaPlayer->IsPlaying();
	}
	return false;
}

bool UCoreMediaPlayerComponent::IsPaused() const
{
	if (MediaPlayer)
	{
		return MediaPlayer->IsPaused();
	}
	return false;
}

FTimespan UCoreMediaPlayerComponent::GetTime() const
{
	if (MediaPlayer)
	{
		return MediaPlayer->GetTime();
	}
	return FTimespan::Zero();
}

FTimespan UCoreMediaPlayerComponent::GetDuration() const
{
	if (MediaPlayer)
	{
		return MediaPlayer->GetDuration();
	}
	return FTimespan::Zero();
}

void UCoreMediaPlayerComponent::SetVolume(float Volume)
{
	// Note: Unreal MediaPlayer doesn't have a direct SetVolume function.
	// You usually control volume via a MediaSoundComponent attached to the actor.
	// For this abstraction, we will rely on external MediaSoundComponent setup,
	// or we can add it here if needed in the future.
}
