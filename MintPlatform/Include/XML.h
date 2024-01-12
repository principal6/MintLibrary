#pragma once


#ifndef _MINT_PLATFORM_XML_H_
#define _MINT_PLATFORM_XML_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/StringReference.h>
#include <MintContainer/Include/String.h>


namespace mint
{
	struct XMLParaser;
}

namespace mint
{
	class XML
	{
		friend XMLParaser;

	public:
		class Attribute;
		class Node;

		class Attribute
		{
			friend XMLParaser;
			//friend Node;

		public:
			Attribute();
			~Attribute();

		public:
			uint32 GetID() const { return _ID; }
			bool HasName() const { return _nameLength > 0; }
			const char* GetName() const { (_xml == nullptr || _nameLength == 0 ? nullptr : &_xml->_namePool[_nameAt]); }
			const char* GetValue() const { (_xml == nullptr || _valueLength == 0 ? nullptr : &_xml->_valuePool[_valueAt]); }
			Attribute* GetNextSiblingAttribute() const;

		private:
			XML* _xml;
			uint32 _ID;
			uint32 _nodeID;
			uint32 _nameAt;
			uint32 _nameLength;
			uint32 _valueAt;
			uint32 _valueLength;
		};

		enum class NodeType
		{
			Meta,
			Expaned,
			OpenClose
		};

		class Node
		{
			friend XMLParaser;
			friend Attribute;

		public:
			Node();
			~Node();

		public:
			uint32 GetID() const { return _ID; }
			bool HasName() const { return _nameLength > 0; }
			NodeType GetNodeType() const { return _nodeType; }
			const char* GetName() const { (_xml == nullptr || _nameLength == 0 ? nullptr : &_xml->_namePool[_nameAt]); }
			const char* GetText() const { (_xml == nullptr || _textLength == 0 ? nullptr : &_xml->_textPool[_textAt]); }
			Attribute* GetFirstAttribute() const { (_xml == nullptr || _attributeIDs.IsEmpty() ? nullptr : &_xml->_attributes[_attributeIDs[0]]); }
			Node* GetFirstChildNode() const { (_xml == nullptr || _childNodeIDs.IsEmpty() ? nullptr : &_xml->_nodes[_childNodeIDs[0]]); }
			Node* GetNextSiblingNode() const;

		private:
			XML* _xml;
			uint32 _ID;
			NodeType _nodeType;
			uint32 _parentID;
			uint32 _nameAt;
			uint32 _nameLength;
			uint32 _textAt;
			uint32 _textLength;
			Vector<uint32> _attributeIDs;
			Vector<uint32> _childNodeIDs;
		};

	public:
		XML();
		~XML();

	public:
		bool Parse(const StringReferenceA& fileName);
		const Node* GetRootNode() const;

	private:
		uint32 GetNodeIndex(uint32 ID) const;
		const Node* GetNode(uint32 ID) const;
		const Attribute* GetAttribute(uint32 ID) const;
		Node* GetNode(uint32 ID);
		Attribute* GetAttribute(uint32 ID);

	private:
		Vector<Node> _nodes;
		Vector<Attribute> _attributes;

	private:
		Vector<char> _namePool;
		Vector<char> _valuePool;
		Vector<char> _textPool;
		uint32 _namePoolAt;
	};
}


#endif // !_MINT_PLATFORM_XML_H_
