

#pragma once

#include "CoreMinimal.h"

#include "WorldSystemDataAsset.h"
#include "Subsystems/WorldSubsystem.h"
#include "GMKWorldGeometrySubsystem.generated.h"

class ADynamicPMCActor;
/**
 *
 * TODO: Make this a BP - https://forums.unrealengine.com/t/configuring-subsystems-via-editor/134784/10
 * TODO: Use a UWorldSubsystem instead?
 *
 * see UWaterSubsystem for example Blueprintable UCLASS(BlueprintType, Transient)
 */
//UCLASS(Abstract, Blueprintable)
UCLASS()
class RUNTIMEGEOMETRYDEMO_API UGMKWorldGeometrySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UGMKWorldGeometrySubsystem();

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//
	// Functions to setup/shutdown/operate the WorldFramework
	//

	void InitializeWorldContext(UWorld* World);

	UPROPERTY()
	UWorld* TargetWorld;

	UPROPERTY()
	ADynamicPMCActor* PDIRenderActor;

	// UPROPERTY(EditAnywhere, Category = PlayerConfig, meta = (AllowPrivateAccess = "true"))
	// TSubclassOf<class AActor> ProjectileBPType;

private:

	/** Default asteroid */
	UPROPERTY()
	UWorldSystemDataAsset*                               WorldSystemDataAssets;

};
