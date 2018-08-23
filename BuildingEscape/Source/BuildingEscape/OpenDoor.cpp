// Copyright @ Mihai Bairac

#include "OpenDoor.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include <assert.h>

#define OUT

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

	Owner = GetOwner();

	if (PressurePlate == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("OpenDoor of %s - missing PressurePlate component !"), *(GetOwner()->GetName()));
	}
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	// Poll the TriggerVolume
	if (GetTotalMassOfActorsOnPlate() > TriggerMass)
	{
		OnOpen.Broadcast();
	}
	else
	{
		OnClose.Broadcast();
	}

}

// Return total mass in Kg
float UOpenDoor::GetTotalMassOfActorsOnPlate()
{
	float TotalMass = 0.0f;

	// Find all the overlapping actors
	if (PressurePlate)
	{
		TArray<AActor*> OverlappingActorArray;
		PressurePlate->GetOverlappingActors(OUT OverlappingActorArray);

		for (const AActor* OverlappingActor : OverlappingActorArray)
		{
			UPrimitiveComponent* PrimitiveComponent = OverlappingActor->FindComponentByClass<UPrimitiveComponent>();

			if (PrimitiveComponent)
			{
				TotalMass += PrimitiveComponent->GetMass();
				UE_LOG(LogTemp, Warning, TEXT("%s on pressure plate, total mass: %f"), *(OverlappingActor->GetName()), TotalMass);
			}
		}
	}

	return TotalMass;
}