// Copyright @ Mihai Bairac

#include "Grabber.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include <assert.h>

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();	

	FindPhysicsHandleComponent();

	SetupInputComponent();
}

/// Look for attached PhysicshandleComponent
void UGrabber::FindPhysicsHandleComponent()
{
	assert(GetOwner() != nullptr);

	PhysicsHandleComponent = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if (PhysicsHandleComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Grabber of %s - missing physics handle component !"), *(GetOwner()->GetName()));
	}
}

/// Look for attached InputComponent (only appears at runtime)
void UGrabber::SetupInputComponent()
{
	assert(GetOwner() != nullptr);

	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	if (InputComponent)
	{
		// Bind input actions
		InputComponent->BindAction("Grab", EInputEvent::IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", EInputEvent::IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Grabber of %s - missing input component !"), *(GetOwner()->GetName()));
	}
}

void UGrabber::Grab()
{
	// Try to reach any actors with physics body collision channel set
	FHitResult HitResult = FindFirstPhysicsObjectInReach();

	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent(); //gets the mesh in our case
	AActor* ActorHit =  HitResult.GetActor();

	// If we hit something then attach a physics handle
	if (ActorHit && PhysicsHandleComponent && ComponentToGrab)
	{
		PhysicsHandleComponent->GrabComponent(ComponentToGrab,
											  NAME_None,
											  ComponentToGrab->GetOwner()->GetActorLocation(),
											  true); //allow rotation
	}
}

void UGrabber::Release()
{
	if (PhysicsHandleComponent)
	{
		PhysicsHandleComponent->ReleaseComponent();
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ComputePlayerData();

	// if yhe physics handle is attached
	if (PhysicsHandleComponent && PhysicsHandleComponent->GetGrabbedComponent())
	{
		// move the object that we're holding
		PhysicsHandleComponent->SetTargetLocation(PlayerData.LineTraceEnd);
	}

}

const FHitResult UGrabber::FindFirstPhysicsObjectInReach()
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

FHitResult UGrabber::LineTraceObjectInReach()
{
	assert(GetWorld() != nullptr);
	assert(GetOwner() != nullptr);

	/// Setup query params
	FCollisionQueryParams QueryParams(FName(TEXT("")), false, GetOwner());

	/// Line-trace (AKA Ray-cast) out to reach distance
	FHitResult LineTraceHit;
	GetWorld()->LineTraceSingleByObjectType(OUT LineTraceHit,
		PlayerData.ViewPointLocation,
		PlayerData.LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		QueryParams
	);

	/// Log what we hit
	AActor* HitActor = LineTraceHit.GetActor();
	if (HitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor Name: %s!"), *(HitActor->GetName()));
	}

	return LineTraceHit;
}

void UGrabber::ComputePlayerData()
{
	assert(GetWorld() != nullptr);
	assert(GetWorld()->GetFirstPlayerController() != nullptr);

	/// Get player view point this tick
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerData.ViewPointLocation,
		OUT PlayerData.ViewPointRotation
	);

	PlayerData.LineTraceEnd = PlayerData.ViewPointLocation + PlayerData.ViewPointRotation.Vector() * PlayerData.Reach;
}