

#pragma once

#include "CoreMinimal.h"
#include "DynamicMesh3.h"
#include "WorldSystemDataAsset.h"
#include "ProtoBuf/DynamicMesh3_ProtoBuf.pb.h"
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
	TSharedPtr<FDynamicMesh3> CreateTestBoxMesh();
	void MeshTest();
	void AddPosition(GMKProtoBuf::Mesh& ProtoMesh, const FVector3d& Position);
	void AddVertex(GMKProtoBuf::Vertex* Vertex, FVector3d* Vector3s);
	void SerializeMeshTest(const FDynamicMesh3* Mesh);
	void DeSerializeMeshTest();
	void AddMesh(const FDynamicMesh3* Mesh);
	void AddPerson();

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
