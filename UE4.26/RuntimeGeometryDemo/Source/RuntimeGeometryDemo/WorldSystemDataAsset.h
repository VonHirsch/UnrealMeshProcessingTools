

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WorldSystemDataAsset.generated.h"

/**
 *
 */
UCLASS()
class RUNTIMEGEOMETRYDEMO_API UWorldSystemDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	/*----------------------------------------------------
		Public data
	----------------------------------------------------*/

	UPROPERTY(EditAnywhere, Category = Content)
	TSubclassOf<class ADynamicPMCActor> ChunkActor;

};
