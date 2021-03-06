// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/Interface_PostProcessVolume.h"
#include "MyUserWidget.h"
#include "Engine/World.h"
#include "GoToHeavenCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class AGoToHeavenCharacter : public ACharacter
{
	GENERATED_BODY()
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

public:
	AGoToHeavenCharacter();

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	AActor* getObjectName(FString name);

	void Episodes(float deltaTime);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface


public:
	UPROPERTY(BlueprintReadWrite)
	UMyUserWidget* ui;

	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	void affectVision();

public:
	float value = 0.0f;
	float timelift = 300;
	float currentTime = 0.0f;

	TArray<FTimerHandle*> timers;
	TArray<float> timersElipsed;

	TArray<bool> triggeredEvent;

	TArray<float> lightIntensityRecords;
	TArray<AActor*> lights;
	TArray<ULightComponent*> lightComponents;
	TArray<UStaticMeshComponent*> meshes;

	void getLightSource();
	void setValueInLightSource(float a);
};

