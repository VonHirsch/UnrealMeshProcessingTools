


#include "GMKWorldGeometrySubsystem.h"


#include "DynamicPMCActor.h"
#include "RuntimeGeometryDemo.h"

UGMKWorldGeometrySubsystem::UGMKWorldGeometrySubsystem()
{
	// Default chunk actor
	static ConstructorHelpers::FObjectFinder<UWorldSystemDataAsset> DefaultAsteroidObj(TEXT("/Game/GameMaker/DataAssets/BP_WorldSystemDataAsset.BP_WorldSystemDataAsset"));
	WorldSystemDataAssets = DefaultAsteroidObj.Object;
}

void UGMKWorldGeometrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SYSLOG("GMK Initialize");
}

void UGMKWorldGeometrySubsystem::Deinitialize()
{
	SYSLOG("GMK Deinitialize");
}

void UGMKWorldGeometrySubsystem::InitializeWorldContext(UWorld* World)
{
	SYSLOG("GMK InitializeWorldContext");
	TargetWorld = World;

	if (WorldSystemDataAssets->ChunkActor)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.bNoFail = true;
		SpawnInfo.ObjectFlags = RF_Transient;
		FTransform MyTransform = FTransform(FVector(-290.f, 380.f, 180.f));
		ADynamicPMCActor* ChunkActor = (GetWorld()->SpawnActor<ADynamicPMCActor>(WorldSystemDataAssets->ChunkActor.Get(), FVector(-290.f, 380.f, 180.f), FRotator(0,0,0), SpawnInfo));
		ChunkActor->SetActorLabel("GMK ChunkActor");
		ChunkActor->CollisionMode = EDynamicMeshActorCollisionMode::ComplexAsSimpleAsync;

		ChunkActor->MeshComponent->bUseAsyncCooking = (ChunkActor->CollisionMode == EDynamicMeshActorCollisionMode::ComplexAsSimpleAsync);	// enables collision
		ChunkActor->MeshComponent->bUseComplexAsSimpleCollision = true;

	} else
	{
		SYSLOG("GMK ChunkActor Null");
	}

	// This works for spawning a simple ADynamicPMCActor

	//FActorSpawnParameters SpawnInfo;
	//PDIRenderActor = TargetWorld->SpawnActor<ADynamicPMCActor>(FVector(-290.f, 380.f, 180.f), FRotator(0,0,0), SpawnInfo);
	//PDIRenderActor = GetWorld()->SpawnActor<ADynamicPMCActor>(FVector(-290.f, 380.f, 180.f), FRotator(0,0,0), SpawnInfo);
	//PDIRenderActor->SetActorLabel("GMK DynamicPMCActor");
	//PDIRenderActor->CollisionMode = EDynamicMeshActorCollisionMode::ComplexAsSimpleAsync;

	// Add component to actor

	// PDIRenderComponent = NewObject<UToolsContextRenderComponent>(PDIRenderActor);
	// PDIRenderActor->SetRootComponent(PDIRenderComponent);
	// PDIRenderComponent->RegisterComponent();

	// Other Misc SpawnActor Code

	// FActorSpawnParameters SpawnInfo;
	// SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	// SpawnInfo.bNoFail = true;
	// SpawnInfo.ObjectFlags = RF_Transient;
	// FTransform MyTransform = FTransform(FVector(-290.f, 380.f, 180.f));
	// Does GetWorld work here?
	// GetWorld()->SpawnActor->SpawnActor(ProjectileBPType.Get(), &MyTransform, FRotator(0,0,0), SpawnInfo);
	// FActorSpawnParameters SpawnParams;
	// AActor* NewProjectile = GetWorld()->SpawnActor(ProjectileBPType.Get(), &MyTransform, SpawnParams);
	// create PDI rendering bridge Component
	// FActorSpawnParameters SpawnInfo;
	// PDIRenderActor = TargetWorld->SpawnActor<AActor>(FVector::ZeroVector, FRotator(0,0,0), SpawnInfo);
	// Attach Component to generic actor ... (should I do this instead?)

}
