


#include "GMKWorldGeometrySubsystem.h"
#include "DynamicPMCActor.h"
#include "RuntimeGeometryDemo.h"
#include <fstream>

#include "MeshComponentRuntimeUtils.h"
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

		// ChunkActorSerialize
		ChunkActorSerialize = (GetWorld()->SpawnActor<ADynamicPMCActor>(WorldSystemDataAssets->ChunkActor.Get(), FVector(-290.f, 380.f, 180.f), FRotator(0,0,0), SpawnInfo));
		ChunkActorSerialize->SetActorLabel("GMKChunkActorSerialize");
		ChunkActorSerialize->CollisionMode = EDynamicMeshActorCollisionMode::ComplexAsSimpleAsync;

		ChunkActorSerialize->MeshComponent->bUseAsyncCooking = (ChunkActorSerialize->CollisionMode == EDynamicMeshActorCollisionMode::ComplexAsSimpleAsync);
		ChunkActorSerialize->MeshComponent->bUseComplexAsSimpleCollision = true;

		// Do a null edit just to RegenerateSourceMesh()  Otherwise collisions won't work
		ChunkActorSerialize->EditMesh([this](FDynamicMesh3& MeshToUpdate) {});

		// ChunkActorDeSerialize
		ChunkActorDeSerialize = (GetWorld()->SpawnActor<ADynamicPMCActor>(WorldSystemDataAssets->ChunkActor.Get(), FVector(-290.f, 80.f, 180.f), FRotator(0,0,0), SpawnInfo));
		ChunkActorDeSerialize->SetActorLabel("GMKChunkActorDeSerialize");
		ChunkActorDeSerialize->CollisionMode = EDynamicMeshActorCollisionMode::ComplexAsSimpleAsync;

		ChunkActorDeSerialize->MeshComponent->bUseAsyncCooking = (ChunkActorSerialize->CollisionMode == EDynamicMeshActorCollisionMode::ComplexAsSimpleAsync);
		ChunkActorDeSerialize->MeshComponent->bUseComplexAsSimpleCollision = true;

		// Do a null edit just to RegenerateSourceMesh()  Otherwise collisions won't work
		ChunkActorDeSerialize->EditMesh([this](FDynamicMesh3& MeshToUpdate) {});

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
	ChunkActorSerialize->GetMeshCopy(InputMesh);

	// Serialize
	SerializeMeshTest(&InputMesh, FileName);

	// DeSerialize
	const FDynamicMesh3 OutputMesh = DeSerializeMeshTest(FileName);

	const bool meshValid = OutputMesh.CheckValidity();

	if (meshValid)
	{
		// Rehydrate Mesh
		UpdatePMCMesh(ChunkActorDeSerialize, OutputMesh);
	} else {
		SYSLOG(TEXT("GMK Mesh was invalid"));
	}

	// TODO, Check IsSameMesh to see where the two differ
	bool IsSame = IsSameMesh(InputMesh, OutputMesh, true, true, true, true, true, true, 1.0f);

	if (IsSame)
	{
		SYSLOG(TEXT("GMK Meshes are similar"));
	} else {
		SYSLOG(TEXT("GMK Meshes are dissimilar"));
	}

}

bool UGMKWorldGeometrySubsystem::IsSameMesh(const FDynamicMesh3& InputMesh, const FDynamicMesh3& OutputMesh, bool bCheckConnectivity, bool bCheckEdgeIDs,
	bool bCheckNormals, bool bCheckColors, bool bCheckUVs, bool bCheckGroups,
	float Epsilon)
{
	if (InputMesh.VertexCount() != OutputMesh.VertexCount())
	{
		return false;
	}
	if (InputMesh.TriangleCount() != OutputMesh.TriangleCount())
	{
		return false;
	}
	for (int vid : InputMesh.VertexIndicesItr())
	{
		if (OutputMesh.IsVertex(vid) == false || VectorUtil::EpsilonEqual(InputMesh.GetVertex(vid), OutputMesh.GetVertex(vid), (double)Epsilon) == false)
		{
			return false;
		}
	}
	for (int tid : InputMesh.TriangleIndicesItr())
	{
		if (OutputMesh.IsTriangle(tid) == false || (InputMesh.GetTriangle(tid) != OutputMesh.GetTriangle(tid)))
		{
			return false;
		}
	}
	if (bCheckConnectivity)
	{
		for (int eid : InputMesh.EdgeIndicesItr())
		{
			FDynamicMesh3::FEdge e = InputMesh.GetEdge(eid);
			int other_eid = OutputMesh.FindEdge(e.Vert[0], e.Vert[1]);
			if (other_eid == FDynamicMesh3::InvalidID)
			{
				return false;
			}
			FDynamicMesh3::FEdge oe = OutputMesh.GetEdge(other_eid);
			if (FMath::Min(e.Tri[0], e.Tri[1]) != FMath::Min(oe.Tri[0], oe.Tri[1]) ||
			    FMath::Max(e.Tri[0], e.Tri[1]) != FMath::Max(oe.Tri[0], oe.Tri[1]))
			{
				return false;
			}
		}
	}
	if (bCheckEdgeIDs)
	{
		if (InputMesh.EdgeCount() != OutputMesh.EdgeCount())
		{
			return false;
		}
		for (int eid : InputMesh.EdgeIndicesItr())
		{
			if (OutputMesh.IsEdge(eid) == false || InputMesh.GetEdge(eid) != OutputMesh.GetEdge(eid))
			{
				return false;
			}
		}
	}
	if (bCheckNormals)
	{
		if (InputMesh.HasVertexNormals() != OutputMesh.HasVertexNormals())
		{
			return false;
		}
		if (InputMesh.HasVertexNormals())
		{
			for (int vid : InputMesh.VertexIndicesItr())
			{
				if (VectorUtil::EpsilonEqual(InputMesh.GetVertexNormal(vid), OutputMesh.GetVertexNormal(vid), Epsilon) == false)
				{
					return false;
				}
			}
		}
	}
	if (bCheckColors)
	{
		if (InputMesh.HasVertexColors() != OutputMesh.HasVertexColors())
		{
			return false;
		}
		if (InputMesh.HasVertexColors())
		{
			for (int vid : InputMesh.VertexIndicesItr())
			{
				if (VectorUtil::EpsilonEqual(InputMesh.GetVertexColor(vid), OutputMesh.GetVertexColor(vid), Epsilon) == false)
				{
					return false;
				}
			}
		}
	}
	if (bCheckUVs)
	{
		if (InputMesh.HasVertexUVs() != OutputMesh.HasVertexUVs())
		{
			return false;
		}
		if (InputMesh.HasVertexUVs())
		{
			for (int vid : InputMesh.VertexIndicesItr())
			{
				if (VectorUtil::EpsilonEqual(InputMesh.GetVertexUV(vid), OutputMesh.GetVertexUV(vid), Epsilon) == false)
				{
					return false;
				}
			}
		}
	}
	if (bCheckGroups)
	{
		if (InputMesh.HasTriangleGroups() != OutputMesh.HasTriangleGroups())
		{
			return false;
		}
		if (InputMesh.HasTriangleGroups())
		{
			for (int tid : InputMesh.TriangleIndicesItr())
			{
				int M1TriangleGroupID = InputMesh.GetTriangleGroup(tid);	// 1
				int M2TriangleGroupID = OutputMesh.GetTriangleGroup(tid);   // 0 == TriangleRefCounts.IsValid(tID) false

				if (M1TriangleGroupID != M2TriangleGroupID)
				{
					return false;	// Returns Here
				}
			}
		}
	}
	return true;
}

void UGMKWorldGeometrySubsystem::UpdatePMCMesh(ADynamicPMCActor* target, FDynamicMesh3 SourceMesh)
{
	if (target->MeshComponent)
	{
		// Setting bUseFaceNormals true fixed the bad texture problem.  Looks much better now
		bool bUseFaceNormals = true; // (target->NormalsMode == EDynamicMeshActorNormalsMode::FaceNormals);
		bool bUseUV0 = true;
		bool bUseVertexColors = false;

		bool bGenerateSectionCollision = false;
		if (target->CollisionMode == EDynamicMeshActorCollisionMode::ComplexAsSimple
			|| target->CollisionMode == EDynamicMeshActorCollisionMode::ComplexAsSimpleAsync)
		{
			bGenerateSectionCollision = true;
			target->MeshComponent->bUseAsyncCooking = (target->CollisionMode == EDynamicMeshActorCollisionMode::ComplexAsSimpleAsync);
			target->MeshComponent->bUseComplexAsSimpleCollision = true;
		}

		RTGUtils::UpdatePMCFromDynamicMesh_SplitTriangles(target->MeshComponent, &SourceMesh, bUseFaceNormals, bUseUV0, bUseVertexColors, bGenerateSectionCollision);

		// update material on new section
		UMaterialInterface* UseMaterial = (target->Material != nullptr) ? target->Material : UMaterial::GetDefaultMaterial(MD_Surface);
		target->MeshComponent->SetMaterial(0, UseMaterial);
	}
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
	int32 SerializedVertexID = 0;
	TArray<int32> SerializedVertexIndex; // lookup of InternalVertexID -> SerializedVertexID
	SerializedVertexIndex.AddUninitialized (Mesh->MaxVertexID());

	for (int vid = 0; vid < Mesh->MaxVertexID(); vid++)
	{

		// TODO: Store lookup table of InternalVertexID -> SerializedVertexID, Use that lookup when writing triangle vertex indexes (use SerializedVertexID, not InternalVertexID)
		if (Mesh->GetVertex(vid, vinfo, true, true, true))
		{
			GMKProtoBuf::Vertex* ProtoVertex = ProtoMesh.add_vertices();
			ProtoVertex->set_index(vid);

			SerializedVertexIndex[vid] = SerializedVertexID;
			SerializedVertexID++;

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
		ProtoVertexIndex->set_vert0(SerializedVertexIndex[TriVerts.A]);
		ProtoVertexIndex->set_vert1(SerializedVertexIndex[TriVerts.B]);
		ProtoVertexIndex->set_vert2(SerializedVertexIndex[TriVerts.C]);

		// Edge Indexes - assume edges are computed
		// FIndex3i TriEdges = Mesh->GetTriEdges(tid);
		// GMKProtoBuf::Triangle_EdgeIndexes* ProtoEdgeIndex = ProtoTriangle->mutable_edgeindexes();
		// ProtoEdgeIndex->set_edge0(TriEdges.A);
		// ProtoEdgeIndex->set_edge1(TriEdges.B);
		// ProtoEdgeIndex->set_edge2(TriEdges.C);
	}

	//---------------------------------------------------------------------------------------
	// Edges - assume edges are computed
	//---------------------------------------------------------------------------------------
	// for (int eid : Mesh->EdgeIndicesItr())
	// {
	// 	GMKProtoBuf::Edge* ProtoEdge = ProtoMesh.add_edges();
	// 	FDynamicMesh3::FEdge edge = Mesh->GetEdge(eid);
	// 	GMKProtoBuf::Edge_Elements* ProtoEdgeElements = ProtoEdge->mutable_elements();
	// 	ProtoEdgeElements->set_tri0(edge.Tri[0]);
	// 	ProtoEdgeElements->set_tri1(edge.Tri[1]);
	// 	ProtoEdgeElements->set_verta(edge.Vert[0]);
	// 	ProtoEdgeElements->set_vertb(edge.Vert[1]);
	// }

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

// Some code borrowed from RTGUtils::ReadOBJMesh (ToolsFrameworkDemo Project)
FDynamicMesh3 UGMKWorldGeometrySubsystem::DeSerializeMeshTest(FString FileName)
{

	GOOGLE_PROTOBUF_VERIFY_VERSION;
	GMKProtoBuf::Mesh ProtoMesh;
	FDynamicMesh3 MeshOut;

	const FString InputFileName = FPaths::Combine(FPaths::ProjectDir(), TEXT("buf"), FileName);
	std::fstream input(ToCStr(InputFileName), std::ios::in | std::ios::binary);

	if (!ProtoMesh.ParseFromIstream(&input)) {
		SYSLOG("Failed to read ProtoMesh.");
		return nullptr;
	}
	input.close();

	const FString LogMessage = (FString::Printf(TEXT("read [%d] vertices"), ProtoMesh.vertices_size()));
	SYSLOG(LogMessage);

	FDynamicMeshNormalOverlay* Normals = nullptr;
	FDynamicMeshUVOverlay* UVs = nullptr;

	// append vertices
	for (int i = 0; i < ProtoMesh.vertices_size(); i++)
	{
		// Position
		const GMKProtoBuf::Vertex& ProtoVertex = ProtoMesh.vertices(i);
		const GMKProtoBuf::Vertex::Position& ProtoPosition = ProtoVertex.position();
		MeshOut.AppendVertex(FVector3d(ProtoPosition.x(), ProtoPosition.y(), ProtoPosition.z()));
		// const FString LogMessage2 = (FString::Printf(TEXT("Vertex Position: [%f],[%f],[%f]"), ProtoPosition.x(), ProtoPosition.y(), ProtoPosition.z()));
		// SYSLOG(LogMessage2);

		// Colors
		if (ProtoVertex.has_colors())
		{
			const GMKProtoBuf::Vertex::Color& ProtoColor = ProtoVertex.colors();
			MeshOut.SetVertexColor(i, FVector3f((float)ProtoColor.r(), (float)ProtoColor.g(), (float)ProtoColor.b()));
		}

		if (ProtoVertex.has_normals() || ProtoVertex.has_uvs())
		{
			MeshOut.EnableAttributes();
		}

		// Normals
		if (ProtoVertex.has_normals())
		{
			Normals = MeshOut.Attributes()->PrimaryNormals();
			const GMKProtoBuf::Vertex::Normal& ProtoNormal = ProtoVertex.normals();
			Normals->AppendElement(FVector3f((float)ProtoNormal.x(), (float)ProtoNormal.y(), (float)ProtoNormal.z()));
		}

		// UVs
		if (ProtoVertex.has_uvs())
		{
			UVs = MeshOut.Attributes()->PrimaryUV();
			const GMKProtoBuf::Vertex::UV& ProtoUV = ProtoVertex.uvs();
			UVs->AppendElement(FVector2f((float)ProtoUV.u(), (float)ProtoUV.v()));
		}
	}

	// append triangles
	for (int i = 0; i < ProtoMesh.triangles_size(); i++) {

		const GMKProtoBuf::Triangle& ProtoTriangle = ProtoMesh.triangles(i);
		const GMKProtoBuf::Triangle_VertexIndexes& Triangle_VertexIndexes = ProtoTriangle.vertexindexes();

		int32 idx0 = Triangle_VertexIndexes.vert0();
		int32 idx1 = Triangle_VertexIndexes.vert1();
		int32 idx2 = Triangle_VertexIndexes.vert2();

		int32 tid = MeshOut.AppendTriangle(idx0, idx1, idx2);

		// Assume vertex, normal and uv indexes are the same
		if (Normals) Normals->SetTriangle(tid, FIndex3i(idx0, idx1, idx2));
		if (UVs) UVs->SetTriangle(tid, FIndex3i(idx0, idx1, idx2));

	}

	return MeshOut;

}
