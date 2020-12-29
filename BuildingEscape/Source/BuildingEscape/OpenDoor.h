// Copyright @ Mihai Bairac

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h"
#include "GameFramework/Actor.h"
#include "OpenDoor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDoorEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOpenDoor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	UPROPERTY(BlueprintAssignable)
	FDoorEvent OnOpen;

	UPROPERTY(BlueprintAssignable)
	FDoorEvent OnClose;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY(EditAnywhere)
	ATriggerVolume* FirstPressurePlate = nullptr; // first trigger to open the door

	UPROPERTY(EditAnywhere)
	ATriggerVolume* SecondPressurePlate = nullptr; // second trigger to open the door

	UPROPERTY(EditAnywhere)
	ATriggerVolume* ThirdPressurePlate = nullptr; // third trigger to open the door

	UPROPERTY(EditAnywhere)
	float FirstTriggerMass = 50.0f;

	UPROPERTY(EditAnywhere)
	float SecondTriggerMass = 50.0f;

	UPROPERTY(EditAnywhere)
	float ThirdTriggerMass = 50.0f;

	AActor* Owner = nullptr; // the owning door

	void TriggerDoor(bool Condition);
	float GetTotalMassOfActorsOnPlate(const ATriggerVolume* PressurePlate);
};
