// Copyright @ Mihai Bairac

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pressable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FButtonEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UPressable : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPressable();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	UPROPERTY(BlueprintAssignable)
	FButtonEvent OnPress;

	UPROPERTY(BlueprintAssignable)
	FButtonEvent OnRelease;

	void TriggerPressEvent();
	void TriggerReleaseEvent();

	bool IsEventTriggered();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	bool m_IsEventTriggered;
};
