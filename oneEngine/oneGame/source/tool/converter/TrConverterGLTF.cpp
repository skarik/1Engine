#include "TrConverterGLTF.h"

#include "core-ext/system/io/assets/ModelIO.h"
#include "core/debug/console.h"

#include "core/types/ModelData.h"
#include "core/math/vect2d_template.h"
#include "core/math/vect3d_template.h"
#include "core/math/vect4d_template.h"

#include "fx/gltf.h"

#include <type_traits>

static bool LoadGLTF(fx::gltf::Document& document, const char* filename)
{
	bool bSuccessfulRead = false;
	try
	{
		document = fx::gltf::LoadFromText(filename);
		bSuccessfulRead = true;
	}
	catch (...)
	{
		bSuccessfulRead = false;
	}

	if (!bSuccessfulRead)
	{
		try
		{
			document = fx::gltf::LoadFromBinary(filename);
			bSuccessfulRead = true;
		}
		catch (...)
		{
			bSuccessfulRead = false;
		}
	}

	return bSuccessfulRead;
}

struct trMeshInfo
{
	arstring256 name;
	arModelData data;
};

template <typename TypeTo, typename TypeFrom>
void CopyCastBuffer ( TypeTo* output_buffer, const uint8* buffer_base, uint32 buffer_stride, uint32 accessor_count )
{
	buffer_stride = buffer_stride ? buffer_stride : sizeof(TypeFrom);
	for (uint i = 0; i < accessor_count; ++i)
	{
		output_buffer[i] = (TypeTo) *static_cast<const TypeFrom*>(static_cast<const void*>(buffer_base + buffer_stride * i));
	}
}

template <typename Type>
void CopyFromGLTFData( const fx::gltf::Document& document, const fx::gltf::Accessor& accessor, Type*& output_data )
{
	const fx::gltf::BufferView& bufferView = document.bufferViews[accessor.bufferView];
	const fx::gltf::Buffer& buffer = document.buffers[bufferView.buffer];

	const uint8* buffer_base = &buffer.data[bufferView.byteOffset + accessor.byteOffset];

	if constexpr (std::is_same<Type, int32>::value
		|| std::is_same<Type, uint32>::value
		|| std::is_same<Type, int16>::value
		|| std::is_same<Type, uint16>::value)
	{
		output_data = new Type[accessor.count];

		ARCORE_ASSERT(accessor.type == fx::gltf::Accessor::Type::Scalar);
		switch (accessor.componentType)
		{
		case fx::gltf::Accessor::ComponentType::Byte:
			CopyCastBuffer<Type, int8>(output_data, buffer_base, bufferView.byteStride, accessor.count);
			break;
		case fx::gltf::Accessor::ComponentType::Short:
			CopyCastBuffer<Type, int16>(output_data, buffer_base, bufferView.byteStride, accessor.count);
			break;
		case fx::gltf::Accessor::ComponentType::UnsignedByte:
			CopyCastBuffer<Type, uint8>(output_data, buffer_base, bufferView.byteStride, accessor.count);
			break;
		case fx::gltf::Accessor::ComponentType::UnsignedShort:
			CopyCastBuffer<Type, uint16>(output_data, buffer_base, bufferView.byteStride, accessor.count);
			break;
		case fx::gltf::Accessor::ComponentType::UnsignedInt:
			CopyCastBuffer<Type, uint32>(output_data, buffer_base, bufferView.byteStride, accessor.count);
			break;
		default:
			ARCORE_ERROR("Invalid type cast");
		}
	}
	else if constexpr ( std::is_same<Type, Vector2f>::value
		|| std::is_same<Type, Vector3f>::value
		|| std::is_same<Type, Vector4f>::value)
	{
		output_data = new Type[accessor.count];

		switch (accessor.componentType)
		{
		case fx::gltf::Accessor::ComponentType::Float:
			switch (accessor.type)
			{
			case fx::gltf::Accessor::Type::Vec2:
				CopyCastBuffer<Type, Vector2f>(output_data, buffer_base, bufferView.byteStride, accessor.count);
				break;
			case fx::gltf::Accessor::Type::Vec3:
				CopyCastBuffer<Type, Vector3f>(output_data, buffer_base, bufferView.byteStride, accessor.count);
				break;
			case fx::gltf::Accessor::Type::Vec4:
				CopyCastBuffer<Type, Vector4f>(output_data, buffer_base, bufferView.byteStride, accessor.count);
				break;
			default:
				ARCORE_ERROR("Invalid type cast");
			}
			break;
		default:
			ARCORE_ERROR("Invalid type cast");
		}
	}
	else if constexpr (std::is_same<Type, Vector4u16>::value)
	{
		output_data = new Type[accessor.count];

		switch (accessor.componentType)
		{
		case fx::gltf::Accessor::ComponentType::UnsignedShort:
			switch (accessor.type)
			{
			case fx::gltf::Accessor::Type::Vec2:
				CopyCastBuffer<Type, core::vect2d_template<uint16>>(output_data, buffer_base, bufferView.byteStride, accessor.count);
				break;
			case fx::gltf::Accessor::Type::Vec3:
				CopyCastBuffer<Type, core::vect3d_template<uint16>>(output_data, buffer_base, bufferView.byteStride, accessor.count);
				break;
			case fx::gltf::Accessor::Type::Vec4:
				CopyCastBuffer<Type, core::vect4d_template<uint16>>(output_data, buffer_base, bufferView.byteStride, accessor.count);
				break;
			default:
				ARCORE_ERROR("Invalid type cast");
			}
			break;
		default:
			ARCORE_ERROR("Invalid type cast");
		}
	}
}

template <core::ModelFmtVertexAttribute Attribute, typename Type>
void AddMPDVertexAttribute ( core::MpdInterface& mpd, const int32 meshIndex, Type* attributeData, const uint32 vertexCount )
{
	if (attributeData != NULL)
	{
		core::modelFmtSegmentInfoHeader header;
		header.type = core::ModelFmtSegmentType::kGeometryVertexData;
		header.subindex = meshIndex;
		header.subtype = (uint8)Attribute;
		header.dataSize = sizeof(Type) * vertexCount;

		mpd.AddSegment(header, attributeData);
	}
}

static void CalculateBBoxMinAndMax ( Vector3f* min, Vector3f* max, const arModelData& model )
{
	ARCORE_ASSERT(model.position && model.vertexNum > 0);

	*min = model.position[0];
	*max = model.position[0];

	for (uint i = 0; i < model.vertexNum; ++i)
	{
		min->x = std::min(min->x, model.position[i].x);
		min->y = std::min(min->y, model.position[i].y);
		min->z = std::min(min->z, model.position[i].z);

		max->x = std::max(max->x, model.position[i].x);
		max->y = std::max(max->y, model.position[i].y);
		max->z = std::max(max->z, model.position[i].z);
	}
}

bool TrConverterGLTF::Convert(const char* inputFilename, const char* outputFilename)
{
	// Store ALL the meshes
	std::vector<trMeshInfo> loadedMeshes;
	{
		// Open the GLTF:
		fx::gltf::Document document;
		const bool bSuccessfulRead = LoadGLTF(document, inputFilename);
		// Couldn't read, we leave
		if (!bSuccessfulRead)
		{
			return false;
		}

		// Get the meshes
		int totalPrimitiveIndex = 0;
		for (int meshIndex = 0; meshIndex < document.meshes.size(); ++meshIndex)
		{
			const fx::gltf::Mesh& mesh = document.meshes[meshIndex];
			for (int primitiveIndex = 0; primitiveIndex < mesh.primitives.size(); ++primitiveIndex)
			{
				const fx::gltf::Primitive& primitive = mesh.primitives[primitiveIndex];

				trMeshInfo loadedMesh;
				loadedMesh.name = mesh.name.c_str();

				CopyFromGLTFData(document, document.accessors[primitive.indices], loadedMesh.data.indices);
				loadedMesh.data.indexNum = document.accessors[primitive.indices].count;

				for (auto const & attrib : primitive.attributes)
				{
					if (attrib.first == "POSITION")
					{
						CopyFromGLTFData(document, document.accessors[attrib.second], loadedMesh.data.position);
						loadedMesh.data.vertexNum = document.accessors[attrib.second].count;
					}
					else if (attrib.first == "NORMAL")
					{
						CopyFromGLTFData(document, document.accessors[attrib.second], loadedMesh.data.normal);
					}
					else if (attrib.first == "TANGENT")
					{
						CopyFromGLTFData(document, document.accessors[attrib.second], loadedMesh.data.tangent);
					}
					else if (attrib.first == "TEXCOORD_0")
					{
						CopyFromGLTFData(document, document.accessors[attrib.second], loadedMesh.data.texcoord0);
					}
					else if (attrib.first == "TEXCOORD_1")
					{
						CopyFromGLTFData(document, document.accessors[attrib.second], loadedMesh.data.texcoord1);
					}
					else if (attrib.first == "TEXCOORD_2")
					{
						CopyFromGLTFData(document, document.accessors[attrib.second], loadedMesh.data.texcoord2);
					}
					else if (attrib.first == "TEXCOORD_3")
					{
						CopyFromGLTFData(document, document.accessors[attrib.second], loadedMesh.data.texcoord3);
					}
					else if (attrib.first == "COLOR")
					{
						CopyFromGLTFData(document, document.accessors[attrib.second], loadedMesh.data.color);
					}
					else if (attrib.first == "JOINTS_0")
					{
						CopyFromGLTFData(document, document.accessors[attrib.second], loadedMesh.data.bone);
					}
					else if (attrib.first == "WEIGHTS_0")
					{
						CopyFromGLTFData(document, document.accessors[attrib.second], loadedMesh.data.weight);
					}
				}

				loadedMeshes.push_back(loadedMesh);
			}
		}

		// We're done.
		// TODO: Error checking on loaded meshes? Binormal generation?
	}

	// Reparse, recalculate, or generation additional mesh information.
	for (int meshIndex = 0; meshIndex < loadedMeshes.size(); ++meshIndex)
	{
		trMeshInfo& mesh = loadedMeshes[meshIndex];

		// Since GLTF standard is Y-up, we need to swap the Y and Z coordinates to get it into the engine's coordinate system.
		// The attributes that are in world-space all need this swap. This includes:
		//	• position
		//	• normal
		//	• tangent
		//	• binormal
		// As for swapping the other items - not 100% sure. We're using the same handedness as Blender, so we just try to match that with everything.
		if (mesh.data.position)
		{
			for (uint i = 0; i < mesh.data.vertexNum; ++i)
			{
				std::swap(mesh.data.position[i].y, mesh.data.position[i].z);
				mesh.data.position[i].y = -mesh.data.position[i].y;
			}
		}
		if (mesh.data.normal)
		{
			for (uint i = 0; i < mesh.data.vertexNum; ++i)
			{
				std::swap(mesh.data.normal[i].y, mesh.data.normal[i].z);
				mesh.data.normal[i].x = -mesh.data.normal[i].x;
				mesh.data.normal[i].z = -mesh.data.normal[i].z;
			}
		}
		if (mesh.data.tangent)
		{
			for (uint i = 0; i < mesh.data.vertexNum; ++i)
			{
				std::swap(mesh.data.tangent[i].y, mesh.data.tangent[i].z);
				mesh.data.tangent[i].x = -mesh.data.tangent[i].x;
				mesh.data.tangent[i].z = -mesh.data.tangent[i].z;
			}
		}
		if (mesh.data.binormal)
		{
			for (uint i = 0; i < mesh.data.vertexNum; ++i)
			{
				std::swap(mesh.data.binormal[i].y, mesh.data.binormal[i].z);
				mesh.data.binormal[i].x = -mesh.data.binormal[i].x;
				mesh.data.binormal[i].z = -mesh.data.binormal[i].z;
			}
		}
	}

	// 
	{
		// Open up the MPD file.
		core::MpdInterface mpd;
		bool mpdOpen = mpd.OpenFile(outputFilename, true);
		ARCORE_ASSERT(mpdOpen);

		// Remove all the geometry info since we have brand new geometry
		while (mpd.GetSegmentCount(core::ModelFmtSegmentType::kGeometryInfo) > 0)
		{
			mpd.RemoveSegment(core::ModelFmtSegmentType::kGeometryInfo, 0);
		}
		while (mpd.GetSegmentCount(core::ModelFmtSegmentType::kGeometryIndices) > 0)
		{
			mpd.RemoveSegment(core::ModelFmtSegmentType::kGeometryIndices, 0);
		}
		while (mpd.GetSegmentCount(core::ModelFmtSegmentType::kGeometryVertexData) > 0)
		{
			mpd.RemoveSegment(core::ModelFmtSegmentType::kGeometryVertexData, 0);
		}

		// Add in the new geometry info
		for (int meshIndex = 0; meshIndex < loadedMeshes.size(); ++meshIndex)
		{
			const trMeshInfo& mesh = loadedMeshes[meshIndex];

			// Create general purpose header
			core::modelFmtSegmentInfoHeader header;
			header.subindex = (uint8)meshIndex;

			// Create information
			{
				core::modelFmtSegmentGeometry_Info geo_info;
				geo_info.index_count = mesh.data.indexNum;
				geo_info.vertex_count = mesh.data.vertexNum;
				geo_info.name = mesh.name;
				CalculateBBoxMinAndMax(&geo_info.bbox_min, &geo_info.bbox_max, mesh.data);

				header.type = core::ModelFmtSegmentType::kGeometryInfo;
				header.dataSize = sizeof(core::modelFmtSegmentGeometry_Info);
				mpd.AddSegment(header, &geo_info);
			}

			// Create index information
			{
				header.type = core::ModelFmtSegmentType::kGeometryIndices;
				header.dataSize = sizeof(mesh.data.indices[0]) * mesh.data.indexNum;
				mpd.AddSegment(header, mesh.data.indices);
			}

			// Create vertex information
			AddMPDVertexAttribute<core::ModelFmtVertexAttribute::kPosition>(mpd, meshIndex, mesh.data.position, mesh.data.vertexNum);
			AddMPDVertexAttribute<core::ModelFmtVertexAttribute::kUV0>(mpd, meshIndex, mesh.data.texcoord0, mesh.data.vertexNum);
			AddMPDVertexAttribute<core::ModelFmtVertexAttribute::kColor>(mpd, meshIndex, mesh.data.color, mesh.data.vertexNum);
			AddMPDVertexAttribute<core::ModelFmtVertexAttribute::kNormal>(mpd, meshIndex, mesh.data.normal, mesh.data.vertexNum);
			AddMPDVertexAttribute<core::ModelFmtVertexAttribute::kTangent>(mpd, meshIndex, mesh.data.tangent, mesh.data.vertexNum);
			AddMPDVertexAttribute<core::ModelFmtVertexAttribute::kUV1>(mpd, meshIndex, mesh.data.texcoord1, mesh.data.vertexNum);
			AddMPDVertexAttribute<core::ModelFmtVertexAttribute::kBoneWeight>(mpd, meshIndex, mesh.data.weight, mesh.data.vertexNum);
			AddMPDVertexAttribute<core::ModelFmtVertexAttribute::kBoneIndices>(mpd, meshIndex, mesh.data.bone, mesh.data.vertexNum);
		}

		// TODO: clear off the temporarily allocated arModelData arrays

		// Save the MPD file.
		return mpd.Save();
	}

	return false;
}