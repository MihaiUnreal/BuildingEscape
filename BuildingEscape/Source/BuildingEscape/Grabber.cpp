// Copyright @ Mihai Bairac

#include "Grabber.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Grabber reporting for duty!"));
	
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

	const float Reach = 60.0f;
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	/// Draw a red trace in the world, to visualize
	DrawDebugLine(GetWorld(),
				  PlayerViewPointLocation,
		          LineTraceEnd,
		          FColor::Red,
		          false,
		         -1.0f,
		          0,
				  3.0f);

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

}

