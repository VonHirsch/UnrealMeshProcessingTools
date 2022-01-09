

#pragma once

#include "CoreMinimal.h"

#include "GMKWorldGeometrySubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "GMKGameModeBase.generated.h"

/**
 *
 */
UCLASS()
class RUNTIMEGEOMETRYDEMO_API AGMKGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGMKGameModeBase();

	virtual void Tick(float DeltaTime) override;
	virtual void StartPlay() override;

	virtual void InitializeWorldSystem();

	//virtual void RegisterTools();

	UPROPERTY()
	UGMKWorldGeometrySubsystem* WorldSystem;

};
