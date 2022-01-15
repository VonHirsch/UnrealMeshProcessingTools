

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GMKPlayerController.generated.h"

/**
 *
 */
UCLASS()
class RUNTIMEGEOMETRYDEMO_API AGMKPlayerController : public APlayerController
{
	GENERATED_BODY()

	AGMKPlayerController();

	void BeginPlay();
};
