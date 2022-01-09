


#include "GMKGameModeBase.h"

#include "Kismet/KismetSystemLibrary.h"

AGMKGameModeBase::AGMKGameModeBase()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	//UKismetSystemLibrary::LoadAssetClass	// Async Load Class Asset BP node

	// UGMKWorldGeometrySubsystem* subsystem = NewObject<UGMKWorldGeometrySubsystem>();
	// UWorld::GetSubsystemArray<TSubsystemClass>(TSubsystemClass::StaticClass());
	//
	// UWorld::SubsystemCollection.AddAndInitializeSubsystem()
	// CreateDefaultSubobject<UGMKWorldGeometrySubsystem>(TEXT("UGMKWorldGeometrySubsystem"));

}

void AGMKGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (WorldSystem)
	{
		//WorldSystem->Tick(DeltaTime);
	}
}

void AGMKGameModeBase::StartPlay()
{
	Super::StartPlay();
	InitializeWorldSystem();
}


void AGMKGameModeBase::InitializeWorldSystem()
{
	UWorld* World = GetWorld();
	UGameInstance* GameInstance = GetGameInstance();
	check(World && GameInstance);

	// create Scene subsystem
	//WorldSystem = UGameInstance::GetSubsystem<UGMKWorldGeometrySubsystem>(GameInstance);
	WorldSystem = UWorld::GetSubsystem<UGMKWorldGeometrySubsystem>(World);

	// Do we really need singleton? see RuntimeToolsFrameworkSubsystem.cpp
	// UGMKWorldGeometrySubsystem::InitializeSingleton(SceneSystem);

	check(WorldSystem);

	// initialize Tools and Scene systems
	WorldSystem->InitializeWorldContext(World);

	//RegisterTools();
}