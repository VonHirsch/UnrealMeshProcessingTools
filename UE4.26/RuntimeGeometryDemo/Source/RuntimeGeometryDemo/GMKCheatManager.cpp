


#include "GMKCheatManager.h"

#include "GMKWorldGeometrySubsystem.h"
#include "RuntimeGeometryDemo.h"

class UGMKWorldGeometrySubsystem;

void UGMKCheatManager::MeshTest() const
{
	UGMKWorldGeometrySubsystem* MySubsystem = GetWorld()->GetSubsystem<UGMKWorldGeometrySubsystem>();
	MySubsystem->MeshTest();
}

void UGMKCheatManager::TestInts(int32 Num, int32 DefaultNum)
{
	const FString LogMessage = (FString::Printf(TEXT("Nums [%d] [%d]"), Num, DefaultNum));
	SYSLOG(LogMessage);
}

void UGMKCheatManager::TestFString(FString NewName)
{
	const FString LogMessage = (FString::Printf(TEXT("SetName [%s]"), *NewName));
	SYSLOG(LogMessage);
}
