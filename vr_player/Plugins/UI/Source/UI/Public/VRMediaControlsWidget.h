#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MediaBackend.h"
#include "VRMediaControlsWidget.generated.h"

/**
 * Base C++ class for the VR Media Controls.
 * Designed to be subclassed in Blueprints (UMG) where the visual buttons and sliders are implemented.
 * This class provides the logic connection between the UI buttons and the CoreMedia backend.
 */
UCLASS(Abstract)
class UI_API UVRMediaControlsWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Assign the backend so this UI can control playback */
	UFUNCTION(BlueprintCallable, Category = "VR Media UI")
	void SetMediaBackend(TScriptInterface<IMediaBackend> InMediaBackend);

protected:

	/** The backend we are controlling */
	UPROPERTY(BlueprintReadOnly, Category = "VR Media UI")
	TScriptInterface<IMediaBackend> MediaBackend;

	/** Called when user presses Play */
	UFUNCTION(BlueprintCallable, Category = "VR Media UI")
	void OnPlayClicked();

	/** Called when user presses Pause */
	UFUNCTION(BlueprintCallable, Category = "VR Media UI")
	void OnPauseClicked();

	/** Called when user interacts with the seek slider */
	UFUNCTION(BlueprintCallable, Category = "VR Media UI")
	void OnSeekScrubbed(float NormalizedTime);

	/** Helper to update UI state based on current playback (e.g. update slider position) */
	UFUNCTION(BlueprintCallable, Category = "VR Media UI")
	float GetNormalizedPlaybackTime() const;
};
