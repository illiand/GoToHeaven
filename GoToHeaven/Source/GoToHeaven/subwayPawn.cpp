// Fill out your copyright notice in the Description page of Project Settings.


#include "subwayPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h"
#include "Components/ActorComponent.h"
#include "Engine/SkyLight.h"
#include "Components/SkyLightComponent.h"
#include "Engine/PointLight.h"
#include "Components/PointLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "Interfaces/Interface_PostProcessVolume.h"
#include "Engine/PostProcessVolume.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MySaveGame.h"
#include "Sound/AmbientSound.h"
#include "Components/AudioComponent.h"

// Sets default values
AsubwayPawn::AsubwayPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
}

// Called when the game starts or when spawned
void AsubwayPawn::BeginPlay()
{
	Super::BeginPlay();

	currentTime = 0;
	Cast<AAmbientSound>(getObjectName("BoomSound"))->Stop();
	Cast<AAmbientSound>(getObjectName("SubwaySound"))->Stop();

	if (Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot("SaveData0", 0)) == NULL)
	{
		UMySaveGame* saveData = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
		saveData->gameStatus = 0;
		UGameplayStatics::SaveGameToSlot(saveData, "SaveData0", 0);
	}
	else
	{
		gameStatus = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot("SaveData0", 0))->gameStatus;
	}


	if (gameStatus == 1) 
	{
		Cast<AStaticMeshActor>(getObjectName("blackWall_1"))->GetStaticMeshComponent()->SetScalarParameterValueOnMaterials(TEXT("transparency"), 0);
		Cast<AStaticMeshActor>(getObjectName("blackWall_2"))->GetStaticMeshComponent()->SetScalarParameterValueOnMaterials(TEXT("transparency"), 0);
		Cast<AStaticMeshActor>(getObjectName("blackWall_3"))->GetStaticMeshComponent()->SetScalarParameterValueOnMaterials(TEXT("transparency"), 0);
		Cast<AStaticMeshActor>(getObjectName("blackWall_4"))->GetStaticMeshComponent()->SetScalarParameterValueOnMaterials(TEXT("transparency"), 0);
		Cast<AStaticMeshActor>(getObjectName("blackWall_5"))->GetStaticMeshComponent()->SetScalarParameterValueOnMaterials(TEXT("transparency"), 0);
		Cast<AStaticMeshActor>(getObjectName("blackWall_6"))->GetStaticMeshComponent()->SetScalarParameterValueOnMaterials(TEXT("transparency"), 0);

		Cast<AAmbientSound>(getObjectName("BoomSound"))->GetAudioComponent()->FadeIn(1);
	}
	else 
	{
		Cast<AAmbientSound>(getObjectName("SubwaySound"))->GetAudioComponent()->FadeIn(1);
	}
}

// Called every frame
void AsubwayPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	lightChange(DeltaTime);
}

// Called to bind functionality to input
void AsubwayPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AsubwayPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AsubwayPawn::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AsubwayPawn::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AsubwayPawn::LookUpAtRate);

}

void AsubwayPawn::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AsubwayPawn::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AsubwayPawn::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AsubwayPawn::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

AActor* AsubwayPawn::getObjectName(FString name)
{
	//get objects
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);

	for (AActor* Actor : Actors)
	{
		//UE_LOG(LogTemp, Warning, TEXT("NAME = %s"), *Actor->GetName());
		if (Actor->GetName() == name)
		{
			return Actor;
		}
	}

	return NULL;
}

void AsubwayPawn::lightChange(float deltaTime)
{
	currentTime += deltaTime;

	if (gameStatus == 0) 
	{
		if (20 <= currentTime && currentTime <= 25)
		{
			AActor* object = getObjectName("SkyLight_1");
			// dark to bright
			Cast<ASkyLight>(object)->GetLightComponent()->SetIntensity(UKismetMathLibrary::Ease(0, 800, (currentTime - 20.0f) / 5.0f, EEasingFunc::CircularIn));
		}

		if (25 <= currentTime && currentTime <= 30)
		{
			AActor* object = getObjectName("SkyLight_1");
			// bright to normal
			Cast<ASkyLight>(object)->GetLightComponent()->SetIntensity(UKismetMathLibrary::Ease(800, 0, (currentTime - 25.0f) / 5.0f, EEasingFunc::CircularOut));
			if (26.0f <= currentTime && currentTime <= 30)
			{

				Cast<AStaticMeshActor>(getObjectName("blackWall_1"))->GetStaticMeshComponent()->SetScalarParameterValueOnMaterials(TEXT("transparency"), FMath::Lerp(1.0f, 0.025f, (currentTime - 26.0f) / 4.0f));

				Cast<AStaticMeshActor>(getObjectName("blackWall_2"))->GetStaticMeshComponent()->SetScalarParameterValueOnMaterials(TEXT("transparency"), FMath::Lerp(1.0f, 0.025f, (currentTime - 26.0f) / 4.0f));

				Cast<AStaticMeshActor>(getObjectName("blackWall_3"))->GetStaticMeshComponent()->SetScalarParameterValueOnMaterials(TEXT("transparency"), FMath::Lerp(1.0f, 0.025f, (currentTime - 26.0f) / 4.0f));

				Cast<AStaticMeshActor>(getObjectName("blackWall_4"))->GetStaticMeshComponent()->SetScalarParameterValueOnMaterials(TEXT("transparency"), FMath::Lerp(1.0f, 0.025f, (currentTime - 26.0f) / 4.0f));

				Cast<AStaticMeshActor>(getObjectName("blackWall_5"))->GetStaticMeshComponent()->SetScalarParameterValueOnMaterials(TEXT("transparency"), FMath::Lerp(1.0f, 0.025f, (currentTime - 26.0f) / 4.0f));

				Cast<AStaticMeshActor>(getObjectName("blackWall_6"))->GetStaticMeshComponent()->SetScalarParameterValueOnMaterials(TEXT("transparency"), FMath::Lerp(1.0f, 0.025f, (currentTime - 26.0f) / 4.0f));
			}
		}

		if (38 <= currentTime && currentTime <= 43)
		{
			((FPostProcessSettings*)Cast<APostProcessVolume>(getObjectName("PPV"))->GetProperties().Settings)->DepthOfFieldFocalDistance = UKismetMathLibrary::Ease(0.0f, 0.5f, (currentTime - 38.0f) / 2.0f, EEasingFunc::CircularIn);
			//UE_LOG(LogTemp, Warning, TEXT("%d"), getObjectName("PPV"));
			if (currentTime > 41)
			{
				ui->changeWBackgroundAlpha(FMath::Lerp(0.0f, 1.0f, (currentTime - 41.0f) / 2.0f));

				//UE_LOG(LogTemp, Warning, TEXT("%d"), getObjectName("PPV"));
			}
		}

		if (currentTime >= 45)
		{
			// transit to club
			UGameplayStatics::OpenLevel(GetWorld(), "MainMap");
		}
	}
	else 
	{
		if (5 <= currentTime && currentTime <= 10)
		{
			ui->changeWBackgroundAlpha(FMath::Lerp(0.0f, 1.0f, (currentTime - 5.0f) / 5.0f));
		}
		if (11 <= currentTime && currentTime <= 15)
		{
			ui->changeBBackgroundAlpha(FMath::Lerp(0.0f, 1.0f, (currentTime - 11.0f) / 4.0f));
		}
		if (15 <= currentTime && currentTime <= 17)
		{
			ui->changeTextAlpha(FMath::Lerp(0.0f, 1.0f, (currentTime - 15.0f) / 2.0f));
		}
		if (13 <= currentTime)
		{
			Cast<AAmbientSound>(getObjectName("BoomSound"))->GetAudioComponent()->FadeOut(5, 0);
		}
	}

}	