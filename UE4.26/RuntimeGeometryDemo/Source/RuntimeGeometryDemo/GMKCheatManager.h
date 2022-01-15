

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "GMKCheatManager.generated.h"

/**
 *
 */
UCLASS( Within = GMKPlayerController )
class RUNTIMEGEOMETRYDEMO_API UGMKCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:

	UFUNCTION(exec)
	void MeshTest() const;

	UFUNCTION(exec)
	static void TestInts(int32 Num, int32 DefaultNum = 1);

	UFUNCTION(exec)
	static void TestFString(FString NewName);

};
