#include "Quest/QuestMarkerTarget.h"

#include "Camera/PlayerCameraManager.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/PlayerController.h"
#include "QuestSystem/Gameplay/QuestSystem/Quest.h"
#include "UObject/ConstructorHelpers.h"

AQuestMarkerTarget::AQuestMarkerTarget()
{
	PrimaryActorTick.bCanEverTick = true;

	MarkerScale = 1.5f;
	bShowGuideLine = true;
	bMarkerVisible = false;

	MarkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MarkerMesh"));
	SetRootComponent(MarkerMesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (PlaneMesh.Succeeded())
	{
		MarkerMesh->SetStaticMesh(PlaneMesh.Object);
	}

	MarkerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MarkerMesh->SetCastShadow(false);
	MarkerMesh->SetHiddenInGame(true);
}

void AQuestMarkerTarget::BeginPlay()
{
	Super::BeginPlay();

	if (MarkerMesh)
	{
		MarkerMesh->SetRelativeScale3D(FVector(MarkerScale));
	}
}

void AQuestMarkerTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bMarkerVisible)
	{
		UpdateBillboardRotation();
	}
}

void AQuestMarkerTarget::SetMarkerVisible(const bool bVisible)
{
	bMarkerVisible = bVisible;

	if (MarkerMesh)
	{
		MarkerMesh->SetHiddenInGame(!bVisible);

		if (bVisible && MarkerMaterial)
		{
			MarkerMesh->SetMaterial(0, MarkerMaterial);
		}
	}
}

void AQuestMarkerTarget::UpdateBillboardRotation()
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const APlayerController* PC = World->GetFirstPlayerController();
	if (!IsValid(PC) || !PC->PlayerCameraManager)
	{
		return;
	}

	const FVector ToCamera = PC->PlayerCameraManager->GetCameraLocation() - GetActorLocation();
	if (!ToCamera.IsNearlyZero())
	{
		MarkerMesh->SetWorldRotation(ToCamera.Rotation());
	}
}