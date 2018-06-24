// Copyright @ Mihai Bairac

#include "OpenDoor.h"
#include "Engine/World.h"
#include <assert.h>

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();

	if (ActorThatOpens)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActorThatOpens name: %s"), *(ActorThatOpens->GetName()));
	}
}

void UOpenDoor::OpenDoor()
{
	AActor* Owner = GetOwner();
	assert(Owner != nullptr);

	// get owner
	FString ObjectName = Owner->GetName();

	// create rotator
	FRotator NewRotation(0.0f, -60.0f, 0.0f); // yaw 60 degrees

											  // set rotation
	Owner->SetActorRotation(NewRotation);

	// log stuff
	FString ObjectRotation = Owner->GetActorRotation().ToString();
	Owner->GetActorRotation().Yaw;
	UE_LOG(LogTemp, Warning, TEXT("%s is rotated %s degrees!"), *ObjectName, *ObjectRotation);
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	// Poll the TriggerVolume
	// if the ActorThatOpens is in the volume
	if (ActorThatOpens && PressurePlate && PressurePlate->IsOverlappingActor(ActorThatOpens))
	{
		OpenDoor();
	}
}

