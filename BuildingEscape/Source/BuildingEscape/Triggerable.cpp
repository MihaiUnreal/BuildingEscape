// Copyright @ Mihai Bairac

#include "Triggerable.h"
#include "Pressable.h"

// Sets default values for this component's properties
UTriggerable::UTriggerable()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTriggerable::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerButton == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Triggerable of %s - missing TriggerButton component !"), *(GetOwner()->GetName()));
	}
	
}


// Called every frame
void UTriggerable::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TriggerButton)
	{
		UPressable* PressableComponent = TriggerButton->FindComponentByClass<UPressable>();

		static bool once = true; // we want to trigger our event only once !

		if (PressableComponent)
		{
			if (PressableComponent->IsEventTriggered())
			{
				if (once)
				{
					OnTriggerEvent.Broadcast();

					UE_LOG(LogTemp, Warning, TEXT("Triggerable of %s - OnTriggerEvent broadcast !"), *(GetOwner()->GetName()));

					once = false;
				}
			}
			else
			{
				once = true;
			}
		}
	}
}

