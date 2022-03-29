// Fill out your copyright notice in the Description page of Project Settings.


#include "Clock.h"
#include "GoToHeavenCharacter.h"

// Sets default values
AClock::AClock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AClock::BeginPlay()
{
	Super::BeginPlay();
	
	UStaticMeshComponent* clockMesh = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("MainClock")));

	for (int i = 1; i <= 13; i += 1) 
	{
		materials.Add(UMaterialInstanceDynamic::Create(clockMesh->GetMaterial(i), NULL));
		clockMesh->SetMaterial(i, materials[materials.Num() - 1]);
	}

	UStaticMeshComponent* hariMesh1 = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("hariLong")));
	materials.Add(UMaterialInstanceDynamic::Create(hariMesh1->GetMaterial(0), NULL));
	hariMesh1->SetMaterial(0, materials[materials.Num() - 1]);

	UStaticMeshComponent* hariMesh2 = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("hariShort")));
	materials.Add(UMaterialInstanceDynamic::Create(hariMesh2->GetMaterial(0), NULL));
	hariMesh2->SetMaterial(0, materials[materials.Num() - 1]);
}

// Called every frame
void AClock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("haguruma1")))->AddLocalRotation(FRotator(4 * DeltaTime, 0, 0));
	Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("haguruma2")))->AddLocalRotation(FRotator(-12 * DeltaTime, 0, 0));
	Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("haguruma3")))->AddLocalRotation(FRotator(8 * DeltaTime, 0, 0));
	Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("haguruma4")))->AddLocalRotation(FRotator(-8 * DeltaTime, 0, 0));

	float value = Cast<AGoToHeavenCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn())->timelift;
	float realTime = FMath::Lerp(0, 46800, value / 300.0f);
	
	float h = realTime / 3600.0f;
	float min = FMath::Fmod(realTime / 60.0f, 60.0f);
	
	float hDegree = h / 13.0f * 360.0f;
	float minDegree = min / 60.0f * 360.0f;

	Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("hariLong")))->SetRelativeRotation(FRotator(minDegree, 0, 0));
	Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("hariShort")))->SetRelativeRotation(FRotator(hDegree, 0, 0));

	//light
	FVector v;

	if (value < 150)
	{
		v = FMath::Lerp(FVector(1, 0, 0), FVector(1, 1, 0), value / 150.0f);
	}
	else
	{
		v = FMath::Lerp(FVector(1, 1, 0), FVector(0, 1, 0), (value - 150) / 150.0f);
	}

	for (int i = 0; i < materials.Num(); i += 1)
	{
		materials[i]->SetVectorParameterValue(TEXT("Color"), FLinearColor(v));

		if (i < materials.Num() - 2) 
		{
			if (i + 1 < 13 - h) 
			{
				materials[i]->SetScalarParameterValue(TEXT("Alpha"), 0);
			}
		}
	}


}

