#include "Components/ShipHealthComponent.h"

#include "Camera/PlayerCameraManager.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

UShipHealthComponent::UShipHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	MaxHealth = 5;
	CurrentHealth = MaxHealth;
	bInitialTransformCached = false;
	bUseCameraFlashOnDamage = true;
	DamageFlashPeakAlpha = 0.65f;
	DamageFlashFadeInTime = 0.06f;
	DamageFlashFadeOutTime = 0.25f;
}

void UShipHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* Owner = GetOwner())
    {
        TArray<UStaticMeshComponent*> MeshComponents;
        Owner->GetComponents<UStaticMeshComponent>(MeshComponents);
        
        for (UStaticMeshComponent* Mesh : MeshComponents)
        {
            if (!ShipMeshComponent && Mesh->IsSimulatingPhysics())
            {
                ShipMeshComponent = Mesh;
            }
        }

        if (!ShipMeshComponent && MeshComponents.Num() > 0)
        {
            ShipMeshComponent = MeshComponents[0];
        }
        
        if (ShipMeshComponent)
        {
            InitialMeshTransform = ShipMeshComponent->GetComponentTransform();
            bInitialTransformCached = true;
        }
    }

    CurrentHealth = MaxHealth;
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
    ConfigureShipOverlapCollision();
}


void UShipHealthComponent::ConfigureShipOverlapCollision()
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	TArray<UPrimitiveComponent*> Primitives;
	Owner->GetComponents<UPrimitiveComponent>(Primitives);

	for (UPrimitiveComponent* Prim : Primitives)
	{
		if (!IsValid(Prim))
		{
			continue;
		}

		Prim->SetGenerateOverlapEvents(true);
		Prim->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
		Prim->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	}
}

void UShipHealthComponent::ResetToSpawnPosition()
{
    AActor* Owner = GetOwner();
    if (!IsValid(Owner) || !bInitialTransformCached || !ShipMeshComponent)
    {
        return;
    }
	
    bool bWasSimulating = ShipMeshComponent->IsSimulatingPhysics();
    ShipMeshComponent->SetSimulatePhysics(false);
	
    ShipMeshComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
    ShipMeshComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	
    ShipMeshComponent->SetWorldTransform(InitialMeshTransform, false, nullptr, ETeleportType::TeleportPhysics);

    if (bWasSimulating)
    {
        ShipMeshComponent->SetSimulatePhysics(true);
        ShipMeshComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
        ShipMeshComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
    }
}

void UShipHealthComponent::ApplyAsteroidHit(const int32 DamageAmount)
{
	if (DamageAmount <= 0 || CurrentHealth <= 0)
	{
		return;
	}

	CurrentHealth = FMath::Max(0, CurrentHealth - DamageAmount);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnDamageFlash.Broadcast();
	PlayDamageFlash();

	if (CurrentHealth <= 0)
	{
		HandleDeath();
	}
}

void UShipHealthComponent::ResetHealth()
{
	CurrentHealth = MaxHealth;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UShipHealthComponent::HandleDeath()
{
	ResetToSpawnPosition();
	ResetHealth();
	OnShipDestroyed.Broadcast();
}

void UShipHealthComponent::PlayDamageFlash()
{
	if (!bUseCameraFlashOnDamage)
	{
		return;
	}

	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!IsValid(OwnerPawn))
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!IsValid(PC) || !PC->PlayerCameraManager)
	{
		return;
	}

	PC->PlayerCameraManager->StartCameraFade(
		0.0f,
		DamageFlashPeakAlpha,
		DamageFlashFadeInTime,
		FLinearColor::Red,
		false,
		true);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DamageFlashTimerHandle);
		World->GetTimerManager().SetTimer(
			DamageFlashTimerHandle,
			this,
			&UShipHealthComponent::EndDamageFlash,
			DamageFlashFadeInTime + 0.05f,
			false);
	}
}

void UShipHealthComponent::EndDamageFlash()
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!IsValid(OwnerPawn))
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!IsValid(PC) || !PC->PlayerCameraManager)
	{
		return;
	}

	PC->PlayerCameraManager->StartCameraFade(
		DamageFlashPeakAlpha,
		0.0f,
		DamageFlashFadeOutTime,
		FLinearColor::Red,
		false,
		false);
}
