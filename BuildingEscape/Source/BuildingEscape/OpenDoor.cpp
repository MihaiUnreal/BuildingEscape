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

	assert(Owner != nullptr);

	FString OwnerName = Owner->GetName();

	// Determine the type of door we are dealing with:
	// Each door must have at least one Trigger Volume !!!
	if (FirstPressurePlate == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("OpenDoor of %s - missing PressurePlate component !"), *OwnerName);
	}

	if (FirstPressurePlate && SecondPressurePlate && ThirdPressurePlate)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenDoor of %s - is triggered by 3 PressurePlate components !"), *OwnerName);
	}
	else if (FirstPressurePlate && SecondPressurePlate)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenDoor of %s - is triggered by 2 PressurePlate components !"), *OwnerName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenDoor of %s - is triggered by 1 PressurePlate component !"), *OwnerName);
	}

	assert(FirstTriggerMass > 0.0f);
	assert(SecondTriggerMass > 0.0f);
	assert(ThirdTriggerMass > 0.0f);
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bool Condition = false;
	
	// Poll the TriggerVolume(s)
	// 3 trigger volumes
	if (FirstPressurePlate && SecondPressurePlate && ThirdPressurePlate)
	{
		Condition = GetTotalMassOfActorsOnPlate(FirstPressurePlate) > FirstTriggerMass &&
					GetTotalMassOfActorsOnPlate(SecondPressurePlate) > SecondTriggerMass &&
					GetTotalMassOfActorsOnPlate(ThirdPressurePlate) > ThirdTriggerMass;
	}
	// 2 trigger volumes
	else if (FirstPressurePlate && SecondPressurePlate)
	{
		Condition = GetTotalMassOfActorsOnPlate(FirstPressurePlate) > FirstTriggerMass &&
					GetTotalMassOfActorsOnPlate(SecondPressurePlate) > SecondTriggerMass;
	}
	// 1 trigger volume
	else
	{
		Condition = GetTotalMassOfActorsOnPlate(FirstPressurePlate) > FirstTriggerMass;
	}

	TriggerDoor(Condition);
}

// if the conditon is true it triggr the signal to open the door
// otherwise triggers the signal to close the door
void UOpenDoor::TriggerDoor(bool Condition)
{
	if (Condition)
	{
		OnOpen.Broadcast();
	}
	else
	{
		OnClose.Broadcast();
	}
}

// Return total mass in Kg
float UOpenDoor::GetTotalMassOfActorsOnPlate(const ATriggerVolume* PressurePlate)
{
	float TotalMass = 0.0f;

	// Find all the overlapping actors
	if (PressurePlate)
	{
		TArray<AActor*> OverlappingActorArray;
		PressurePlate->GetOverlappingActors(OUT OverlappingActorArray);

		for (const AActor* OverlappingActor : OverlappingActorArray)
		{
			if (OverlappingActor)
			{
				UPrimitiveComponent* PrimitiveComponent = OverlappingActor->FindComponentByClass<UPrimitiveComponent>();
				// take into account only the mass of the movable objects (that can be grabbed and moved)
				if (PrimitiveComponent && OverlappingActor->IsRootComponentMovable())
				{
					TotalMass += PrimitiveComponent->GetMass();

					//UE_LOG(LogTemp, Warning, TEXT("OpenDoor - %s on pressure plate, total mass: %f"), *(OverlappingActor->GetName()), TotalMass);
				}
			}
		}

		if (TotalMass > 0)
		{
			assert(Owner != nullptr);
			UE_LOG(LogTemp, Warning, TEXT("OpenDoor of %s -  total mass: %f"), *(Owner->GetName()), TotalMass);
		}
	}

	return TotalMass;
}