#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MediaBackend.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UMediaBackend : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for abstracting the underlying media player implementation (e.g. Unreal Media Framework, libVLC, FFmpeg).
 */
class COREMEDIA_API IMediaBackend
{
	GENERATED_BODY()

public:

	/** Opens a media URL (file:// or smb:// etc) */
	UFUNCTION(BlueprintCallable, Category="Media")
	virtual bool OpenUrl(const FString& Url) = 0;

	/** Plays the currently loaded media */
	UFUNCTION(BlueprintCallable, Category="Media")
	virtual void Play() = 0;

	/** Pauses the currently playing media */
	UFUNCTION(BlueprintCallable, Category="Media")
	virtual void Pause() = 0;

	/** Stops playback and clears media */
	UFUNCTION(BlueprintCallable, Category="Media")
	virtual void Stop() = 0;

	/** Seeks to a specific time */
	UFUNCTION(BlueprintCallable, Category="Media")
	virtual void Seek(FTimespan Time) = 0;

	/** Returns true if media is currently playing */
	UFUNCTION(BlueprintCallable, Category="Media")
	virtual bool IsPlaying() const = 0;

	/** Returns true if media is currently paused */
	UFUNCTION(BlueprintCallable, Category="Media")
	virtual bool IsPaused() const = 0;

	/** Gets the current playback time */
	UFUNCTION(BlueprintCallable, Category="Media")
	virtual FTimespan GetTime() const = 0;

	/** Gets the duration of the current media */
	UFUNCTION(BlueprintCallable, Category="Media")
	virtual FTimespan GetDuration() const = 0;

	/** Sets volume (0.0 to 1.0) */
	UFUNCTION(BlueprintCallable, Category="Media")
	virtual void SetVolume(float Volume) = 0;
};
