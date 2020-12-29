// Copyright @ Mihai Bairac

#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/ActorComponent.h"
#include "Components/InputComponent.h"
#include "Components/AudioComponent.h"
#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPhysicsHandleComponent* m_PhysicsHandleComponent = nullptr;
	UInputComponent* m_InputComponent = nullptr;
	UAudioComponent* m_GrabAudioComponent = nullptr;

	struct PlayerData
	{
		FVector ViewPointLocation;
		FRotator ViewPointRotation;
		FVector LineTraceEnd;
		// how far does the player reach for grabbable objects
		float Reach = 200.0f;
	} m_PlayerData;

	// Find (assumed) attached physics handle component
	void FindPhysicsHandleComponent();

	// Setup (assumed) attached input component
	void SetupInputComponent();

	// Ray-cast and grab what's in reach
	void Grab();

	// Release what has been grabbed
	void Release();

	// Return hit for frist physics body in reach
	const FHitResult FindFirstPhysicsObjectInReach();

	FHitResult LineTraceObjectInReach();

	void ComputePlayerData();

	void SetupGrabSound();
};