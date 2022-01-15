
set proto_bin="F:\UE\GameMaker\GradientSpace\UnrealMeshProcessingTools\UE4.26\RuntimeGeometryDemo\Plugins\Protobuf\Source\Protobuf\ThirdParty\protobuf\bin\protoc.exe"
set proto_path=F:\UE\GameMaker\GradientSpace\UnrealMeshProcessingTools\UE4.26\RuntimeGeometryDemo\Source\RuntimeGeometryDemo\ProtoBuf

set proto_file1=%proto_path%\DynamicMesh3_ProtoBuf.proto

%proto_bin% --proto_path="%proto_path%" --cpp_out="%proto_path%" "%proto_file1%"

pause
