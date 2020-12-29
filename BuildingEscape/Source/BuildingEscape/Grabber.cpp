// Copyright @ Mihai Bairac

#include "Grabber.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "Components/PrimitiveComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include <assert.h>

//#define DEBUG_LINE
#ifdef DEBUG_LINE
#include "DrawDebugHelpers.h"
#endif

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Has to be called in constructor!
	SetupGrabSound();
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

	m_PhysicsHandleComponent = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if (m_PhysicsHandleComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Grabber of %s - missing physics handle component !"), *(GetOwner()->GetName()));
	}
}

/// Look for attached InputComponent (only appears at runtime)
void UGrabber::SetupInputComponent()
{
	assert(GetOwner() != nullptr);

	m_InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	if (m_InputComponent)
	{
		// Bind input actions
		m_InputComponent->BindAction("Grab", EInputEvent::IE_Pressed, this, &UGrabber::Grab);
		m_InputComponent->BindAction("Grab", EInputEvent::IE_Released, this, &UGrabber::Release);
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
	if (ActorHit && m_PhysicsHandleComponent && ComponentToGrab)
	{
		m_PhysicsHandleComponent->GrabComponent(ComponentToGrab,
											  NAME_None,
											  ComponentToGrab->GetOwner()->GetActorLocation(),
											  true); //allow rotation

		// Play the grab sound
		if (m_GrabAudioComponent)
		{
			m_GrabAudioComponent->Play();
		}
	}
}

void UGrabber::Release()
{
	if (m_PhysicsHandleComponent)
	{
		m_PhysicsHandleComponent->ReleaseComponent();
	}
}

void UGrabber::SetupGrabSound()
{
	// Load our Sound Cue for the grab object sound we created in the editor... 
	static ConstructorHelpers::FObjectFinder<USoundCue> GrabCue(
		TEXT("/Game/GrabObject_SoundCue")
	);

	// Store a reference to the Cue asset - we'll need it later.
	USoundCue* GrabAudioCue = GrabCue.Object;

	// Create an audio component, the audio component wraps the Cue, 
	// and allows us to ineract with it, and its parameters from code.
	m_GrabAudioComponent = CreateDefaultSubobject<UAudioComponent>(
		TEXT("PropellerAudioComp")
		);

	if (m_GrabAudioComponent)
	{
		// I don't want the sound playing the moment it's created.
		m_GrabAudioComponent->bAutoActivate = false;

		// Attach our sound cue to the SoundComponent
		if (GrabAudioCue && m_GrabAudioComponent->IsValidLowLevelFast()) {
			m_GrabAudioComponent->SetSound(GrabAudioCue);
		}
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ComputePlayerData();

	// if yhe physics handle is attached
	if (m_PhysicsHandleComponent && m_PhysicsHandleComponent->GetGrabbedComponent())
	{
		// move the object that we're holding
		m_PhysicsHandleComponent->SetTargetLocation(m_PlayerData.LineTraceEnd);
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
		m_PlayerData.ViewPointLocation,
		m_PlayerData.LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		QueryParams
	);

	/// Log what we hit
	AActor* HitActor = LineTraceHit.GetActor();
	if (HitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Grabber of %s - Hit Actor Name: %s!"), *(GetOwner()->GetName()), *(HitActor->GetName()));
	}

	return LineTraceHit;
}

void UGrabber::ComputePlayerData()
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