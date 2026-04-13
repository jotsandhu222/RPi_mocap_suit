#include "VRProjectionActor.h"
#include "Materials/MaterialInstanceDynamic.h"

AVRProjectionActor::AVRProjectionActor()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectionMesh"));
	RootComponent = ProjectionMesh;

	// Default configuration
	CurrentMode = EVRProjectionMode::FlatScreen;
}

void AVRProjectionActor::BeginPlay()
{
	Super::BeginPlay();

	// Initialize dynamic material if BaseMaterial is provided
	if (BaseMaterial && !DynamicMaterial)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		ProjectionMesh->SetMaterial(0, DynamicMaterial);
	}

	UpdateMeshForMode();
}

void AVRProjectionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVRProjectionActor::SetProjectionMode(EVRProjectionMode NewMode)
{
	if (CurrentMode != NewMode)
	{
		CurrentMode = NewMode;
		UpdateMeshForMode();
	}
}

void AVRProjectionActor::UpdateMeshForMode()
{
	UStaticMesh* TargetMesh = nullptr;

	switch (CurrentMode)
	{
	case EVRProjectionMode::FlatScreen:
		TargetMesh = FlatMesh;
		break;
	case EVRProjectionMode::CurvedScreen:
		TargetMesh = CurvedMesh;
		break;
	case EVRProjectionMode::Immersive180:
		TargetMesh = HemisphereMesh;
		break;
	case EVRProjectionMode::Immersive360:
		TargetMesh = SphereMesh;
		break;
	}

	if (TargetMesh && ProjectionMesh->GetStaticMesh() != TargetMesh)
	{
		ProjectionMesh->SetStaticMesh(TargetMesh);

		// Re-apply the dynamic material to the new mesh
		if (DynamicMaterial)
		{
			ProjectionMesh->SetMaterial(0, DynamicMaterial);
		}
	}
}

void AVRProjectionActor::SetMediaTexture(UMediaTexture* InMediaTexture)
{
	if (DynamicMaterial && InMediaTexture)
	{
		// Assuming the parameter name in the BaseMaterial is 'MediaTexture'
		DynamicMaterial->SetTextureParameterValue(FName("MediaTexture"), InMediaTexture);
	}
}

void AVRProjectionActor::AutoDetectModeFromFilename(const FString& Filename)
{
	FString LowerFilename = Filename.ToLower();

	if (LowerFilename.Contains(TEXT("_360")) || LowerFilename.Contains(TEXT("360vr")))
	{
		SetProjectionMode(EVRProjectionMode::Immersive360);
	}
	else if (LowerFilename.Contains(TEXT("_180")) || LowerFilename.Contains(TEXT("180vr")))
	{
		SetProjectionMode(EVRProjectionMode::Immersive180);
	}
	else if (LowerFilename.Contains(TEXT("_curve")) || LowerFilename.Contains(TEXT("_curved")))
	{
		SetProjectionMode(EVRProjectionMode::CurvedScreen);
	}
	else
	{
		// Default to flat
		SetProjectionMode(EVRProjectionMode::FlatScreen);
	}

	// Note: SBS (Side-by-side) or TB (Top-Bottom) handling for 3D stereoscopy
	// should be passed to the material shader to handle UV splitting for left/right eyes.
	// You might want to detect that here and set a scalar parameter on DynamicMaterial.
	if (LowerFilename.Contains(TEXT("_sbs")))
	{
		if (DynamicMaterial) DynamicMaterial->SetScalarParameterValue(FName("StereoMode"), 1.0f); // 1 = SBS
	}
	else if (LowerFilename.Contains(TEXT("_tb")) || LowerFilename.Contains(TEXT("_ou")))
	{
		if (DynamicMaterial) DynamicMaterial->SetScalarParameterValue(FName("StereoMode"), 2.0f); // 2 = TB/OU
	}
	else
	{
		if (DynamicMaterial) DynamicMaterial->SetScalarParameterValue(FName("StereoMode"), 0.0f); // 0 = Monoscopic
	}
}
