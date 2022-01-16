


#include "GMKWorldGeometrySubsystem.h"
#include "DynamicPMCActor.h"
#include "RuntimeGeometryDemo.h"
#include <fstream>

#include "MeshNormals.h"
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
		ChunkActor = (GetWorld()->SpawnActor<ADynamicPMCActor>(WorldSystemDataAssets->ChunkActor.Get(), FVector(-290.f, 380.f, 180.f), FRotator(0,0,0), SpawnInfo));
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

void UGMKWorldGeometrySubsystem::BoxMeshTest()
{
	SYSLOG("GMK BoxMeshTest");

	// Box Mesh Test
	const TSharedPtr<FDynamicMesh3> InputMesh = CreateTestBoxMesh();

	const FString FileName = TEXT("BoxMeshTest.buf");

	// Serialize
	SerializeMeshTest(InputMesh.Get(), FileName);

	// DeSerialize
	DeSerializeMeshTest(FileName);

	// Todo: Create mesh from DeSerialized Data and Check it's Validity
	//bool meshValid = InputMesh->CheckValidity();
	//const FString LogMessage = (FString::Printf(TEXT("GMK MeshTest Result [%d]"), meshValid));
	//SYSLOG(LogMessage);
}

void UGMKWorldGeometrySubsystem::MeshTest()
{
	SYSLOG("GMK MeshTest");

	const FString FileName = TEXT("MeshTest.buf");

	FDynamicMesh3 InputMesh;
	ChunkActor->GetMeshCopy(InputMesh);

	// Serialize
	SerializeMeshTest(&InputMesh, FileName);

	// DeSerialize
	DeSerializeMeshTest(FileName);

	// Todo: Create mesh from DeSerialized Data and Check it's Validity
	//bool meshValid = InputMesh->CheckValidity();
	//const FString LogMessage = (FString::Printf(TEXT("GMK MeshTest Result [%d]"), meshValid));
	//SYSLOG(LogMessage);

}

// Some Code Borrowed from FDynamicMesh3::CompactCopy, FDynamicMesh3::IsSameMesh, RTGUtils::UpdatePMCFromDynamicMesh_SplitTriangles
void UGMKWorldGeometrySubsystem::SerializeMeshTest(const FDynamicMesh3* Mesh, FString FileName)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	GMKProtoBuf::Mesh ProtoMesh;

	//---------------------------------------------------------------------------------------
	// Vertices
	//---------------------------------------------------------------------------------------
	FVertexInfo vinfo;
	for (int vid = 0; vid < Mesh->MaxVertexID(); vid++)
	{
		if (Mesh->GetVertex(vid, vinfo, true, true, true))
		{
			GMKProtoBuf::Vertex* ProtoVertex = ProtoMesh.add_vertices();

			// Position
			GMKProtoBuf::Vertex_Position* ProtoPosition = ProtoVertex->mutable_position();
			ProtoPosition->set_x(vinfo.Position.X);
			ProtoPosition->set_y(vinfo.Position.Y);
			ProtoPosition->set_z(vinfo.Position.Z);

			// Normals
			if (vinfo.bHaveN)
			{
				GMKProtoBuf::Vertex_Normal* ProtoNormal = ProtoVertex->mutable_normals();
				ProtoNormal->set_x(vinfo.Normal.X);
				ProtoNormal->set_y(vinfo.Normal.Y);
				ProtoNormal->set_z(vinfo.Normal.Z);
			}
			// Colors
			if (vinfo.bHaveC)
			{
				GMKProtoBuf::Vertex_Color* ProtoColor = ProtoVertex->mutable_colors();
				ProtoColor->set_r(vinfo.Color.X);
				ProtoColor->set_b(vinfo.Color.Y);
				ProtoColor->set_g(vinfo.Color.Z);
			}
			// UVs
			if (vinfo.bHaveUV)
			{
				GMKProtoBuf::Vertex_UV* ProtoUV = ProtoVertex->mutable_uvs();
				ProtoUV->set_u(vinfo.UV.X);
				ProtoUV->set_v(vinfo.UV.Y);
			}

		}

	}

	//---------------------------------------------------------------------------------------
	// Triangles
	//---------------------------------------------------------------------------------------
	for (int32 tid : Mesh->TriangleIndicesItr())
	{
		GMKProtoBuf::Triangle* ProtoTriangle = ProtoMesh.add_triangles();

		// Vertex Indexes
		FIndex3i TriVerts = Mesh->GetTriangle(tid);
		GMKProtoBuf::Triangle_VertexIndexes* ProtoVertexIndex = ProtoTriangle->mutable_vertexindexes();
		ProtoVertexIndex->set_vert0(TriVerts.A);
		ProtoVertexIndex->set_vert1(TriVerts.B);
		ProtoVertexIndex->set_vert2(TriVerts.C);

		// Edge Indexes
		FIndex3i TriEdges = Mesh->GetTriEdges(tid);
		GMKProtoBuf::Triangle_EdgeIndexes* ProtoEdgeIndex = ProtoTriangle->mutable_edgeindexes();
		ProtoEdgeIndex->set_edge0(TriEdges.A);
		ProtoEdgeIndex->set_edge1(TriEdges.B);
		ProtoEdgeIndex->set_edge2(TriEdges.C);
	}

	//---------------------------------------------------------------------------------------
	// Edges
	//---------------------------------------------------------------------------------------
	for (int eid : Mesh->EdgeIndicesItr())
	{
		GMKProtoBuf::Edge* ProtoEdge = ProtoMesh.add_edges();
		FDynamicMesh3::FEdge edge = Mesh->GetEdge(eid);
		GMKProtoBuf::Edge_Elements* ProtoEdgeElements = ProtoEdge->mutable_elements();
		ProtoEdgeElements->set_tri0(edge.Tri[0]);
		ProtoEdgeElements->set_tri1(edge.Tri[1]);
		ProtoEdgeElements->set_verta(edge.Vert[0]);
		ProtoEdgeElements->set_vertb(edge.Vert[1]);
	}

	const FString LogMessage = (FString::Printf(TEXT("writing [%d] vertices"), ProtoMesh.vertices_size()));
	SYSLOG(LogMessage);

	const FString OutputFileName = FPaths::Combine(FPaths::ProjectDir(), TEXT("buf"), FileName);
	std::fstream output(ToCStr(OutputFileName), std::ios::out | std::ios::trunc | std::ios::binary);

	if (!ProtoMesh.SerializeToOstream(&output)) {
		SYSLOG("Failed to write ProtoMesh.");
	} else
	{
		output.close();
	}

}

void UGMKWorldGeometrySubsystem::DeSerializeMeshTest(FString FileName)
{

	GOOGLE_PROTOBUF_VERIFY_VERSION;
	GMKProtoBuf::Mesh ProtoMesh;

	const FString InputFileName = FPaths::Combine(FPaths::ProjectDir(), TEXT("buf"), FileName);
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
