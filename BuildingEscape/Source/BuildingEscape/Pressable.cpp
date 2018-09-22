// Copyright @ Mihai Bairac

#include "Pressable.h"

// Sets default values for this component's properties
UPressable::UPressable()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	m_IsEventTriggered = false;
}


// Called when the game starts
void UPressable::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UPressable::TriggerPressEvent()
{
	OnPress.Broadcast();

	if (!m_IsEventTriggered)
	{
		m_IsEventTriggered = true;
	}

	UE_LOG(LogTemp, Warning, TEXT("Pressable of %s - OnPress event broadcast !"), *(GetOwner()->GetName()));
}

void UPressable::TriggerReleaseEvent()
{
	OnRelease.Broadcast();

	if (m_IsEventTriggered)
	{
		m_IsEventTriggered = false;
	}

	UE_LOG(LogTemp, Warning, TEXT("Pressable of %s - OnRelease event broadcast !"), *(GetOwner()->GetName()));
}

bool UPressable::IsEventTriggered()
{
	return m_IsEventTriggered;
}

// Called every frame
void UPressable::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

