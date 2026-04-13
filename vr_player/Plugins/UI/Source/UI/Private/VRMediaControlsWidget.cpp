#include "VRMediaControlsWidget.h"

void UVRMediaControlsWidget::SetMediaBackend(TScriptInterface<IMediaBackend> InMediaBackend)
{
	MediaBackend = InMediaBackend;
}

void UVRMediaControlsWidget::OnPlayClicked()
{
	if (MediaBackend)
	{
		MediaBackend->Play();
	}
}

void UVRMediaControlsWidget::OnPauseClicked()
{
	if (MediaBackend)
	{
		MediaBackend->Pause();
	}
}

void UVRMediaControlsWidget::OnSeekScrubbed(float NormalizedTime)
{
	if (MediaBackend)
	{
		FTimespan Duration = MediaBackend->GetDuration();
		if (Duration > FTimespan::Zero())
		{
			// Calculate target time based on normalized (0.0 to 1.0) value
			int64 Ticks = Duration.GetTicks() * FMath::Clamp(NormalizedTime, 0.0f, 1.0f);
			MediaBackend->Seek(FTimespan(Ticks));
		}
	}
}

float UVRMediaControlsWidget::GetNormalizedPlaybackTime() const
{
	if (MediaBackend)
	{
		FTimespan Duration = MediaBackend->GetDuration();
		FTimespan CurrentTime = MediaBackend->GetTime();

		if (Duration > FTimespan::Zero())
		{
			return (float)CurrentTime.GetTicks() / (float)Duration.GetTicks();
		}
	}
	return 0.0f;
}
