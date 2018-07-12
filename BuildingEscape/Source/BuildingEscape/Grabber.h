// Copyright @ Mihai Bairac

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/InputComponent.h"
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
	// how far does the player reach for grabbable objects
	float Reach = 200.0f;

	UPhysicsHandleComponent* PhysicsHandleComponent = nullptr;

	UInputComponent* InputComponent = nullptr;

	FVector LineTraceEnd;

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
};