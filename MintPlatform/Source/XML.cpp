#include <MintPlatform/Include/XML.h>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/StringReference.hpp>
#include <MintContainer/Include/String.hpp>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintPlatform/Include/TextFile.h>


namespace mint
{
#pragma region XMLParser
	struct XMLParaser
	{
		XMLParaser(XML& xml, const char* const text, uint32 length)
			: _xml{ xml }
			, _text{ text }
			, _length{ length }
			, _atPrev{ 0 }
			, _at{ 0 }
			, _line{ 1 }
		{
			_xml._namePool.Reserve(256);
			_xml._valuePool.Reserve(256);
			_xml._textPool.Reserve(256);
		}

		bool Parse()
		{
			while (_at < _length)
			{
				if (ParseNode(kUint32Max) == false)
				{
					return false;
				}
			}
			return true;
		}

		bool ParseNode(uint32 parentNodeID)
		{
			while (_at < _length)
			{
				if (_text[_at] == '<')
				{
					break;
				}
				++_at;
			}

			if (_at >= _length)
			{
				return true;
			}

			XML::Node node;
			node._xml = &_xml;
			if (_text[_at + 1] == '/')
			{
				// Closing node
				++_atPrev;

				const uint32 nodeID = parentNodeID;
				XML::Node& node = _xml._nodes[nodeID];
				ParseString(_xml._textPool, node._textAt, node._textLength);

				// Sklp '</{NAME}>'
				_at += node._nameLength + 3;
				_atPrev = _at;
				return ParseNode(node._parentID);
			}
			else
			{
				// Opening node
				node._nodeType = XML::NodeType::Expaned;

				// Sklp '<'
				++_at;
				_atPrev = _at;
			}

			node._ID = _xml._nodes.Size();
			node._parentID = parentNodeID;
			while (_at < _length)
			{
				if (IsWhiteSpace(_text[_at]))
				{
					ParseString(_xml._namePool, node._nameAt, node._nameLength);
					break;
				}
				++_at;
			}
			if (node.HasName() == false)
			{
				ReportError("Node name is not found!");
				return false;
			}

			XML::Attribute attribute;
			attribute._xml = &_xml;
			attribute._nodeID = node._ID;
			while (_at < _length)
			{
				if (_text[_at] == '>')
				{
					// End of node
					if (_text[_at - 1] == '?')
					{
						node._nodeType = XML::NodeType::Meta;
					}
					else if (_text[_at - 1] == '/')
					{
						node._nodeType = XML::NodeType::OpenClose;
					}

					_xml._nodes.PushBack(std::move(node));
					if (node._parentID != kUint32Max)
					{
						_xml._nodes[node._parentID]._childNodeIDs.PushBack(node._ID);
					}

					++_at;
					if (node._nodeType == XML::NodeType::OpenClose)
					{
						return ParseNode(node._parentID);
					}
					else
					{
						// Parse child node or text
						return ParseNode(node._ID);
					}
				}
				else if (_text[_at] == '=')
				{
					// Parse attribute
					MINT_ASSERT(attribute.HasName() == false, "!!!");

					ParseString(_xml._namePool, attribute._nameAt, attribute._nameLength);
					if (ParseAttributeValue(attribute) == false)
					{
						return false;
					}

					node._attributeIDs.PushBack(attribute._ID);
					_xml._attributes.PushBack(std::move(attribute));

					attribute._ID = _xml._attributes.Size();
					attribute._nameLength = 0;
					attribute._valueLength = 0;
				}
				++_at;
			}

			ReportError("End of node was not found!");
			return false;
		}

		void ParseString(Vector<char>& pool, uint32& outNameAt, uint32& outNameLength)
		{
			MINT_ASSERT(IsWhiteSpace(_text[_at]) == true || _text[_at] == '=' || _text[_at] == '\"' || _text[_at] == '<', "!!!");

			while (IsWhiteSpace(_text[_atPrev]))
			{
				++_atPrev;
			}

			outNameLength = _at - _atPrev;
			if (outNameLength == 0)
			{
				outNameAt = 0;
				return;
			}

			outNameAt = pool.Size();
			while (outNameAt + outNameLength > pool.Capacity())
			{
				pool.Reserve(pool.Capacity() * 2);
			}

			pool.Resize(outNameAt + outNameLength + 1);
			StringUtil::Copy(&pool[outNameAt], &_text[_atPrev], outNameLength);

			_atPrev = _at + 1;
		}

		bool ParseAttributeValue(XML::Attribute& attribute)
		{
			MINT_ASSERT(_text[_at] == '=', "!!!");

			bool isOpened = false;
			bool isClosed = false;
			while (_at < _length)
			{
				if (_text[_at] == '\"')
				{
					if (isOpened == false)
					{
						isOpened = true;
						_atPrev = _at + 1;
					}
					else
					{
						isClosed = true;
						ParseString(_xml._valuePool, attribute._valueAt, attribute._valueLength);
						break;
					}
				}
				++_at;
			}

			if (isOpened == false)
			{
				ReportError("No opening \" was found!");
			}
			else if (isClosed == false)
			{
				ReportError("No closing \" was found!");
			}

			return true;
		}

		bool IsWhiteSpace(char at) const
		{
			return at == ' ' || at == '\t' || at == '\r' || at == '\n';
		}

		void ReportError(const StringReferenceA& errorMessage)
		{
			_errorMessage = "ERROR At[";
			_errorMessage += StringUtil::ToStringA(_at);
			_errorMessage += "] Line[";
			_errorMessage += StringUtil::ToStringA(_line);
			_errorMessage += "] ";
			_errorMessage += errorMessage.CString();
		}

	private:
		XML& _xml;
		const char* const _text;
		const uint32 _length;

		uint32 _at;
		uint32 _atPrev;
		uint32 _line;
		StringA _errorMessage;
	};
#pragma endregion

#pragma region XML::Attribute
	XML::Attribute::Attribute()
		: _xml{ nullptr }
		, _ID{ 0 }
		, _nodeID{ 0 }
		, _nameAt{ 0 }
		, _nameLength{ 0 }
		, _valueAt{ 0 }
		, _valueLength{ 0 }
	{
	}

	XML::Attribute::~Attribute()
	{
	}

	XML::Attribute* XML::Attribute::GetNextSiblingAttribute() const
	{
		if (_xml == nullptr)
		{
			return nullptr;
		}

		const XML::Node* const node = _xml->GetNode(_nodeID);
		MINT_ASSERT(node != nullptr, "!!!");

		const uint32 attributeCount = node->_attributeIDs.Size();
		uint32 nextAttributeIndex = kUint32Max;
		for (uint32 i = 0; i < attributeCount; i++)
		{
			if (node->_attributeIDs[i] == _ID)
			{
				nextAttributeIndex = i + 1;
				break;
			}
		}
		if (nextAttributeIndex > attributeCount)
		{
			return nullptr;
		}

		return _xml->GetAttribute(node->_attributeIDs[nextAttributeIndex]);
	}
#pragma endregion

#pragma region XML::Node
	XML::Node::Node()
		: _xml{ nullptr }
		, _ID{ 0 }
		, _nodeType{ NodeType::Expaned }
		, _parentID{ 0 }
		, _nameAt{ 0 }
		, _nameLength{ 0 }
		, _textAt{ 0 }
		, _textLength{ 0 }
	{
	}

	XML::Node::~Node()
	{
	}

	XML::Attribute* XML::Node::FindAttribute(const StringReferenceA& attributeName) const
	{
		for (XML::Attribute* attribute = GetFirstAttribute(); attribute != nullptr; attribute = attribute->GetNextSiblingAttribute())
		{
			if (attributeName == attribute->GetName())
			{
				return attribute;
			}
		}
		return nullptr;
	}

	XML::Node* XML::Node::GetNextSiblingNode() const
	{
		if (_xml == nullptr)
		{
			return nullptr;
		}

		if (_parentID == kUint32Max)
		{
			const uint32 nodeIndex = _xml->GetNodeIndex(_ID);
			MINT_ASSERT(nodeIndex != kUint32Max, "!!!");
			return &_xml->_nodes[nodeIndex + 1];
		}
		else
		{
			const XML::Node* const parentNode = _xml->GetNode(_parentID);
			const uint32 childNodeCount = parentNode->_childNodeIDs.Size();
			uint32 childNodeIndex = kUint32Max;
			for (uint32 i = 0; i < childNodeCount; ++i)
			{
				if (parentNode->_childNodeIDs[i] == _ID)
				{
					childNodeIndex = i;
					break;
				}
			}
			MINT_ASSERT(childNodeIndex != kUint32Max, "!!!");

			if (childNodeIndex + 1 >= childNodeCount)
			{
				return nullptr;
			}
			return _xml->GetNode(parentNode->_childNodeIDs[childNodeIndex + 1]);
		}

	}
#pragma endregion

#pragma region XML
	XML::XML()
		: _namePoolAt{ 0 }
	{
	}

	XML::~XML()
	{
	}

	bool XML::Parse(const StringReferenceA& fileName)
	{
		TextFileReader textFileReader;
		if (textFileReader.Open(fileName.CString()) == false)
		{
			return false;
		}

		XMLParaser parser{ *this, textFileReader.Get(), textFileReader.GetFileSize() };
		return parser.Parse();
	}

	const XML::Node* XML::GetRootNode() const
	{
		if (_nodes.IsEmpty())
		{
			return nullptr;
		}

		if (_nodes[0].GetNodeType() == NodeType::Meta)
		{
			return _nodes[0].GetNextSiblingNode();
		}
		return &_nodes[0];
	}

	uint32 XML::GetNodeIndex(uint32 ID) const
	{
		const uint32 nodeCount = _nodes.Size();
		for (uint32 i = 0; i < nodeCount; ++i)
		{
			if (_nodes[i].GetID() == ID)
			{
				return i;
			}
		}
		return kUint32Max;
	}

	const XML::Node* XML::GetNode(uint32 ID) const
	{
		uint32 nodeIndex = GetNodeIndex(ID);
		if (nodeIndex == kUint32Max)
		{
			return nullptr;
		}
		return &_nodes[nodeIndex];
	}

	const XML::Attribute* XML::GetAttribute(uint32 ID) const
	{
		const uint32 attributeCount = _attributes.Size();
		for (uint32 i = 0; i < attributeCount; ++i)
		{
			if (_attributes[i].GetID() == ID)
			{
				return &_attributes[i];
			}
		}
		return nullptr;
	}

	XML::Node* XML::GetNode(uint32 ID)
	{
		uint32 nodeIndex = GetNodeIndex(ID);
		if (nodeIndex == kUint32Max)
		{
			return nullptr;
		}
		return &_nodes[nodeIndex];
	}

	XML::Attribute* XML::GetAttribute(uint32 ID)
	{
		return const_cast<XML::Attribute*>(static_cast<const XML*>(this)->GetAttribute(ID));
	}
#pragma endregion
}
