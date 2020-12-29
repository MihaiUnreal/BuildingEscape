// Copyright @ Mihai Bairac

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/InputComponent.h"
#include "Presser.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UPresser : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPresser();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPhysicsHandleComponent* m_PhysicsHandleComponent = nullptr;

	UInputComponent* m_InputComponent = nullptr;

	enum class EventType: short {
		EPress = 0,
		ERelease,
		ECount
	};

	struct PlayerData
	{
		FVector ViewPointLocation;
		FRotator ViewPointRotation;
		FVector LineTraceEnd;
		// how far does the player reach for grabbable objects
		float Reach = 200.0f;
	} m_PlayerData;

	// Setup (assumed) attached input component
	void SetupInputComponent();

	DECLARE_DELEGATE_OneParam(FTriggerEventDelegate, EventType);

	// Ray-cast and trigger event on what's in reach
	void TriggerEvent(EventType EventType);

	// Return hit for frist physics body in reach
	const FHitResult FindFirstPhysicsObjectInReach();

	FHitResult LineTraceObjectInReach();

	void ComputePlayerData();
	
};
