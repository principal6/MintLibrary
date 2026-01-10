#pragma once


#ifndef _MINT_APP_MESH_COMPONENT_H_
#define _MINT_APP_MESH_COMPONENT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRendering/Include/RenderingBaseCommon.h>
#include <MintRendering/Include/MeshData.h>

#include <MintRendering/Include/Utilities/MeshGenerator.h>

#include <MintApp/Include/SceneObject.h>


namespace mint
{
	struct MeshComponent
	{
		Rendering::MeshData _meshData;
		Rendering::GraphicsObjectID _materialID;
		bool _shouldDrawNormals = false;
		bool _shouldDrawEdges = false;
	};

	struct Mesh2DComponent
	{
		Rendering::Shape _shape;
	};
}


#endif // !_MINT_APP_MESH_COMPONENT_H_
