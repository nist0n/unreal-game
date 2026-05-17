#include "PickUp/AsteroidPickUp.h"

#include "Components/ShipHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

AAsteroidPickUp::AAsteroidPickUp()
{
	PrimaryActorTick.bCanEverTick = false;

	bSimulatePhysicsOnSpawn = false;
	bDestroyOnShipHit = true;
	ShipDamageAmount = 1;
	HitCooldownSeconds = 0.25f;
	LastHitTimeSeconds = -1000.0f;
}

void AAsteroidPickUp::BeginPlay()
{
	Super::BeginPlay();

	if (PickUpMeshComponent)
	{
		ConfigureMeteorOverlapCollision();
		PickUpMeshComponent->SetSimulatePhysics(bSimulatePhysicsOnSpawn);
		PickUpMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AAsteroidPickUp::OnAsteroidOverlap);
	}
}

void AAsteroidPickUp::ConfigureMeteorOverlapCollision()
{
	if (!PickUpMeshComponent)
	{
		return;
	}

	PickUpMeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
	PickUpMeshComponent->SetGenerateOverlapEvents(true);
	// Overlap with ship (Pawn) — no blocking, so the ship is not spun by physics.
	PickUpMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PickUpMeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
}

void AAsteroidPickUp::LaunchInDirection(FVector Direction, float Speed)
{
	if (!PickUpMeshComponent)
	{
		return;
	}

	ConfigureMeteorOverlapCollision();
	PickUpMeshComponent->SetSimulatePhysics(true);
	PickUpMeshComponent->SetEnableGravity(false);
	PickUpMeshComponent->SetPhysicsLinearVelocity(Direction.GetSafeNormal() * Speed);
}

void AAsteroidPickUp::SetLifetimeSeconds(const float Seconds)
{
	if (Seconds > 0.0f)
	{
		SetLifeSpan(Seconds);
	}
}

void AAsteroidPickUp::OnAsteroidOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor))
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const float Now = World->GetTimeSeconds();
	if (Now - LastHitTimeSeconds < HitCooldownSeconds)
	{
		return;
	}

	UShipHealthComponent* ShipHealth = OtherActor->FindComponentByClass<UShipHealthComponent>();
	if (!IsValid(ShipHealth) && IsValid(OtherActor->GetParentActor()))
	{
		ShipHealth = OtherActor->GetParentActor()->FindComponentByClass<UShipHealthComponent>();
	}

	if (!IsValid(ShipHealth))
	{
		return;
	}

	LastHitTimeSeconds = Now;
	ShipHealth->ApplyAsteroidHit(ShipDamageAmount);

	if (bDestroyOnShipHit)
	{
		Destroy();
	}
}
