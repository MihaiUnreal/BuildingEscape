// Copyright @ Mihai Bairac

#include "Presser.h"
#include "Engine/World.h"
#include "Pressable.h"
#include <assert.h>

//#define DEBUG_LINE
#ifdef DEBUG_LINE
#include "DrawDebugHelpers.h"
#endif

#define OUT

// Sets default values for this component's properties
UPresser::UPresser()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPresser::BeginPlay()
{
	Super::BeginPlay();

	SetupInputComponent();
}

/// Look for attached InputComponent (only appears at runtime)
void UPresser::SetupInputComponent()
{
	assert(GetOwner() != nullptr);

	m_InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	if (m_InputComponent)
	{
		// Bind input actions
		m_InputComponent->BindAction<FTriggerEventDelegate>("Press", EInputEvent::IE_Pressed, this, &UPresser::TriggerEvent, EventType::EPress);
		m_InputComponent->BindAction<FTriggerEventDelegate>("Press", EInputEvent::IE_Released, this, &UPresser::TriggerEvent, EventType::ERelease);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Presser of %s - missing input component !"), *(GetOwner()->GetName()));
	}
}

void UPresser::TriggerEvent(EventType EventType)
{
	// Try to reach any actors with physics body collision channel set
	FHitResult HitResult = FindFirstPhysicsObjectInReach();

	AActor* ActorHit = HitResult.GetActor();

	// If we hit something then we broadcast the on press event
	if (ActorHit)
	{
		UPressable* PressableComponent = ActorHit->FindComponentByClass<UPressable>();

		if (PressableComponent)
		{
			switch (EventType)
			{
			case EventType::EPress:
				PressableComponent->TriggerPressEvent();
				break;
			case EventType::ERelease:
				PressableComponent->TriggerReleaseEvent();
				break;
			}
			
		}
	}
}

// Called every frame
void UPresser::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

const FHitResult UPresser::FindFirstPhysicsObjectInReach()
{
	ComputePlayerData();

#ifdef DEBUG_LINE
	/// Draw a red trace in the world, to visualize
	DrawDebugLine(GetWorld(),
		PlayerData.ViewPointLocation,
		PlayerData.LineTraceEnd,
		FColor::Red,
		false,
		-1.0f,
		0,
		3.0f);
#endif

	FHitResult LineTraceHit = LineTraceObjectInReach();

	return LineTraceHit;
}

FHitResult UPresser::LineTraceObjectInReach()
{
	assert(GetWorld() != nullptr);
	assert(GetOwner() != nullptr);

	/// Setup query params
	FCollisionQueryParams QueryParams(FName(TEXT("")), false, GetOwner());

	/// Line-trace (AKA Ray-cast) out to reach distance
	FHitResult LineTraceHit;
	GetWorld()->LineTraceSingleByObjectType(OUT LineTraceHit,
		m_PlayerData.ViewPointLocation,
		m_PlayerData.LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
		QueryParams
	);

	/// Log what we hit
	AActor* HitActor = LineTraceHit.GetActor();
	if (HitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Presser of %s - Hit Actor Name: %s!"), *(GetOwner()->GetName()), *(HitActor->GetName()));
	}

	return LineTraceHit;
}

void UPresser::ComputePlayerData()
{
	assert(GetWorld() != nullptr);
	assert(GetWorld()->GetFirstPlayerController() != nullptr);

	/// Get player view point this tick
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT m_PlayerData.ViewPointLocation,
		OUT m_PlayerData.ViewPointRotation
	);

	m_PlayerData.LineTraceEnd = m_PlayerData.ViewPointLocation + m_PlayerData.ViewPointRotation.Vector() * m_PlayerData.Reach;
}