


#include "GMKWorldGeometrySubsystem.h"
#include "DynamicPMCActor.h"
#include "RuntimeGeometryDemo.h"
#include <fstream>

#include "Generators/GridBoxMeshGenerator.h"
#include "ProtoBuf/addressbook.pb.h"
#include "ProtoBuf/DynamicMesh3_ProtoBuf.pb.h"

/*
 *	Vertex
 *		Position[double 3]		Vertices{}
 *		Reference Count			VertexRefCounts{}		Reference counts of vertex indices
 *		Normal[float 3]			VertexNormals{}
 *		Color[float 3]			VertexColors{}
 *		UV[float 2]				VertexUVs{}
 *		VertexEdgeLists[]		VertexEdgeLists
 *			each VertexEdgeList is an unordered list of connected edge indexes
 *
 *	Triangle[vertex indexes / int 3]
 *		[v1,v2,v3]				Triangles
 *		Reference Count			TriangleRefCounts{}		Reference counts of triangle indices
 *
 *	TriangleEdge[edge indexes / int 3]
 *		e1=edge(v1,v2), e2=edge(v2,v3), e3=edge(v3,v1) (where the vertices are the vertex indexes of the triangle)*
 *		[e1,e2,e3]				TriangleEdges
 *		TriangleGroups (optional)
 *
 *	Edge[element indexes int 4]
 *		[VertA, VertB, Tri0, Tri1]	Edges
 *
 */

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
		ChunkActor->SetActorLabel("GMKChunkActor");
		ChunkActor->CollisionMode = EDynamicMeshActorCollisionMode::ComplexAsSimpleAsync;

		ChunkActor->MeshComponent->bUseAsyncCooking = (ChunkActor->CollisionMode == EDynamicMeshActorCollisionMode::ComplexAsSimpleAsync);
		ChunkActor->MeshComponent->bUseComplexAsSimpleCollision = true;

		// Do a null edit just to RegenerateSourceMesh()  Otherwise collisions won't work
		ChunkActor->EditMesh([this](FDynamicMesh3& MeshToUpdate) {});

	} else
	{
		SYSLOG("GMK ChunkActor Null");
	}

	/*

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

	*/

}

TSharedPtr<FDynamicMesh3> UGMKWorldGeometrySubsystem::CreateTestBoxMesh()
{
	FGridBoxMeshGenerator BoxGenerator;
	BoxGenerator.Box = FOrientedBox3d(FVector3d::Zero(), FVector3d(1.0, 1.0, 1.0));
	int EdgeNum = 5;
	BoxGenerator.EdgeVertices = FIndex3i(EdgeNum, EdgeNum, EdgeNum);
	BoxGenerator.Generate();

	TSharedPtr<FDynamicMesh3> Mesh = MakeShareable<FDynamicMesh3>(new FDynamicMesh3(&BoxGenerator));
	return Mesh;
}

void UGMKWorldGeometrySubsystem::MeshTest()
{
	SYSLOG("GMK MeshTest");

	// ---- Setup
	// Create a box mesh, random noise, and a directional filter pointing in the +Y direction
	TSharedPtr<FDynamicMesh3> InputMesh = CreateTestBoxMesh();
	bool meshValid = InputMesh->CheckValidity();

	SerializeMeshTest(InputMesh.Get());
	DeSerializeMeshTest();

	const FString LogMessage = (FString::Printf(TEXT("GMK MeshTest Result [%d]"), meshValid));
	SYSLOG(LogMessage);

}

void UGMKWorldGeometrySubsystem::SerializeMeshTest(const FDynamicMesh3* Mesh)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	GMKProtoBuf::Mesh ProtoMesh;

	// for each triangle index
	for (int32 tid : Mesh->TriangleIndicesItr())
	{
		FVector3d Position[3];
		// get vertices
		Mesh->GetTriVertices(tid, Position[0], Position[1], Position[2]);

		for (int j = 0; j < 3; ++j)
		{
			AddVertex(ProtoMesh.add_vertices(), &Position[j]);
		}
	}

	const FString LogMessage = (FString::Printf(TEXT("writing [%d] vertices"), ProtoMesh.vertices_size()));
	SYSLOG(LogMessage);

	const FString OutputFileName = FPaths::Combine(FPaths::ProjectDir(), TEXT("buf"), TEXT("ProtoMesh.buf"));
	std::fstream output(ToCStr(OutputFileName), std::ios::out | std::ios::trunc | std::ios::binary);

	if (!ProtoMesh.SerializeToOstream(&output)) {
		SYSLOG("Failed to write ProtoMesh.");
	} else
	{
		output.close();
	}

}

void UGMKWorldGeometrySubsystem::DeSerializeMeshTest()
{

	GOOGLE_PROTOBUF_VERIFY_VERSION;
	GMKProtoBuf::Mesh ProtoMesh;

	const FString InputFileName = FPaths::Combine(FPaths::ProjectDir(), TEXT("buf"), TEXT("ProtoMesh.buf"));
	std::fstream input(ToCStr(InputFileName), std::ios::in | std::ios::binary);

	if (!ProtoMesh.ParseFromIstream(&input)) {
		SYSLOG("Failed to read ProtoMesh.");
	} else
	{
		input.close();
	}

	const FString LogMessage = (FString::Printf(TEXT("read [%d] vertices"), ProtoMesh.vertices_size()));
	SYSLOG(LogMessage);

	for (int i = 0; i < ProtoMesh.vertices_size(); i++)
	{
		const GMKProtoBuf::Vertex& ProtoVertex = ProtoMesh.vertices(i);
		const GMKProtoBuf::Vertex::Position& ProtoPosition = ProtoVertex.position();

		// const FString LogMessage2 = (FString::Printf(TEXT("Vertex Position: [%f],[%f],[%f]"), ProtoPosition.x(), ProtoPosition.y(), ProtoPosition.z()));
		// SYSLOG(LogMessage2);

	}

}

void UGMKWorldGeometrySubsystem::AddVertex(GMKProtoBuf::Vertex* ProtoVertex, FVector3d* Position)
{
	GMKProtoBuf::Vertex_Position* ProtoPosition = ProtoVertex->mutable_position();
	ProtoPosition->set_x(Position->X);
	ProtoPosition->set_y(Position->Y);
	ProtoPosition->set_z(Position->Z);
}
