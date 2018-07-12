// Copyright @ Mihai Bairac

#include "Grabber.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"

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
	PhysicsHandleComponent = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if (PhysicsHandleComponent)
	{
		// PhysicsHandleComponent is found
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Grabber of %s - missing physics handle component !"), *(GetOwner()->GetName()));
	}
}

/// Look for attached InputComponent (only appears at runtime)
void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	if (InputComponent)
	{
		// Input Component is found
		UE_LOG(LogTemp, Warning, TEXT("Grabber of %s - found input component !"), *(GetOwner()->GetName()));

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
	UE_LOG(LogTemp, Warning, TEXT("Grabber - key pressed !"));

	// Try to reach any actors with physics body collision channel set
	FHitResult HitResult = FindFirstPhysicsObjectInReach();

	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
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
	UE_LOG(LogTemp, Warning, TEXT("Grabber - key released !"));

	if (PhysicsHandleComponent)
	{
		PhysicsHandleComponent->ReleaseComponent();
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/// Get player view point this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	/// Log to test
	//	UE_LOG(LogTemp, Warning, TEXT("Grabber viewpoint location: %s!"), *PlayerViewPointLocation.ToString());
	//	UE_LOG(LogTemp, Warning, TEXT("Grabber viewpoint rotation: %s!"), *PlayerViewPointRotation.ToString());

	LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	// if yhe physics handle is attached
	if (PhysicsHandleComponent && PhysicsHandleComponent->GetGrabbedComponent())
	{
		// move the object that we're holding
		PhysicsHandleComponent->SetTargetLocation(LineTraceEnd);
	}

}

const FHitResult UGrabber::FindFirstPhysicsObjectInReach()
{
	/// Get player view point this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	/// Log to test
	//	UE_LOG(LogTemp, Warning, TEXT("Grabber viewpoint location: %s!"), *PlayerViewPointLocation.ToString());
	//	UE_LOG(LogTemp, Warning, TEXT("Grabber viewpoint rotation: %s!"), *PlayerViewPointRotation.ToString());

	LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	/*
	/// Draw a red trace in the world, to visualize
	DrawDebugLine(GetWorld(),
	PlayerViewPointLocation,
	LineTraceEnd,
	FColor::Red,
	false,
	-1.0f,
	0,
	3.0f);
	*/
	/// Setup query params
	FCollisionQueryParams QueryParams(FName(TEXT("")), false, GetOwner());

	/// Line-trace (AKA Ray-cast) out to reach distance
	FHitResult LineTraceHit;
	GetWorld()->LineTraceSingleByObjectType(OUT LineTraceHit,
		PlayerViewPointLocation,
		LineTraceEnd,
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
