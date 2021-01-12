#include <stdafx.h>
#include <FsLibrary/File/XmlFile.h>

#include <fstream>
#include <sstream>
#include <filesystem>


namespace fs
{
	XmlFile::XmlFile()
		: _rootNode{ nullptr }
	{
		__noop;
	}

	XmlFile::XmlFile(const char* const fileName)
		: _rootNode{ nullptr }
	{
		FS_ASSERT("김장원", openFile(fileName) == true, "파일 열기에 실패했습니다.");
	}

	XmlFile::XmlFile(XmlFile&& rhs) noexcept
	{
		std::swap(_xmlDeclaration, rhs._xmlDeclaration);
		std::swap(_rootNode, rhs._rootNode);
	}

	XmlFile::~XmlFile()
	{
		FS_DELETE(_rootNode);
	}

	XmlFile& XmlFile::operator=(XmlFile&& rhs) noexcept
	{
		if (this != &rhs)
		{
			std::swap(_xmlDeclaration, rhs._xmlDeclaration);
			std::swap(_rootNode, rhs._rootNode);
		}
		return *this;
	}

	bool XmlFile::isValid() const noexcept
	{
		const Tag& declarationTag{ _xmlDeclaration.getTag() };
		if (declarationTag._type != Tag::Type::XmlDeclaration)
		{
			return false;
		}

		for (const auto& attribute : declarationTag._attributeArray)
		{
			if (attribute.getName() == "version" && attribute.getValue() != "1.0")
			{
				return false;
			}
		}

		return (_rootNode == nullptr) ? false : true;
	}

	bool XmlFile::openFile(const char* const fileName)
	{
		std::ifstream ifs{ fileName };
		FS_ASSERT("김장원", ifs.is_open() == true, "파일을 열지 못했습니다.");
		if (ifs.is_open() == false)
		{
			return false;
		}

		std::stringstream ssTag{};
		std::stringstream ssContent{};
		std::string stringTag{}; // <...>
		int32 levelCheck{};
		Node* currentParentNode{ nullptr };
		std::string currentTagName{};
		while (ifs.eof() == false)
		{
			FS_ASSERT("김장원", levelCheck >= 0, "태그가 열리고 닫히는 횟수가 부적절합니다.");

			char ch0{ (char)ifs.get() };
			if (ch0 == '<')
			{
				ssTag << ch0;
				while (ch0 != '>')
				{
					FS_ASSERT("김장원", ifs.eof() == false, "< 가 열렸지만 >로 닫히지 않았습니다.");
					ch0 = ifs.get();
					ssTag << ch0;
				}

				stringTag = ssTag.str();
				ssTag.str("");

				// @XML Declaration
				if (stringTag[1] == '?')
				{
					FS_ASSERT("김장원", stringTag[stringTag.size() - 2] == '?', "XML Declaration은 ?>로 끝나야 합니다.");

					XmlElement newXmlElement{};
					XmlTag xmlDeclarationTag{ XmlTag(Tag::Type::XmlDeclaration, std::string("XMLDeclaration")) };
					parseAttributes(stringTag.c_str(), xmlDeclarationTag._attributeArray);
					newXmlElement.setTag(xmlDeclarationTag);
					_xmlDeclaration = newXmlElement;
				}
				// @Annoation
				else if (stringTag[1] == '!' && stringTag[2] == '-' && stringTag[3] == '-')
				{
					FS_ASSERT("김장원", stringTag[stringTag.size() - 3] == '-', "올바른 주석 형식이 아닙니다.");
					FS_ASSERT("김장원", stringTag[stringTag.size() - 2] == '-', "올바른 주석 형식이 아닙니다.");
				}
				else
				{
					if (stringTag[stringTag.size() - 2] == '/') // empty-element tag
					{
						// create new node without changing level
						XmlElement newXmlElement{};
						parseTagName(stringTag.c_str(), currentTagName);
						XmlTag newXmlTag{ XmlTag(Tag::Type::EmptyElementTag, currentTagName) };
						parseAttributes(stringTag.c_str(), newXmlTag._attributeArray);
						newXmlElement.setTag(newXmlTag);

						Node* const childNode{ insertChildNode(currentParentNode) };
						childNode->_element = newXmlElement;
					}
					else
					{
						if (stringTag[1] == '/') // end tag
						{
							FS_ASSERT("김장원", currentParentNode != nullptr, "여기서 currentParentNode가 nullptr이면 문법이 잘못된 파일입니다.");
							if (currentParentNode == nullptr)
							{
								return false;
							}

							// check end tag
							parseTagName(stringTag.c_str(), currentTagName);
							const bool isSameName{ currentParentNode->_element.getTag()._name == currentTagName };
							FS_ASSERT("김장원", isSameName == true, "열리고 닫힌 tag의 이름이 다릅니다!");
							currentParentNode->_element.setContent(std::string(ssContent.str().c_str()));
							ssContent.str("");

							--levelCheck;
							currentParentNode = currentParentNode->_parentNode;
						}
						else // start tag
						{
							if (currentParentNode == nullptr)
							{
								// create root node
								XmlElement newXmlElement{};
								parseTagName(stringTag.c_str(), currentTagName);
								XmlTag newXmlTag{ XmlTag(Tag::Type::StartEndTag, currentTagName) };
								parseAttributes(stringTag.c_str(), newXmlTag._attributeArray);
								newXmlElement.setTag(newXmlTag);

								_rootNode = new Node(nullptr, newXmlElement);
								currentParentNode = _rootNode;

								++levelCheck;
							}
							else
							{
								// create new node
								XmlElement newXmlElement{};
								parseTagName(stringTag.c_str(), currentTagName);
								XmlTag newXmlTag{ XmlTag(Tag::Type::StartEndTag, currentTagName) };
								parseAttributes(stringTag.c_str(), newXmlTag._attributeArray);
								newXmlElement.setTag(newXmlTag);

								Node* const childNode{ insertChildNode(currentParentNode) };
								childNode->_element = newXmlElement;
								currentParentNode = childNode;

								++levelCheck;
							}
						}
					}
				}
			}
			else
			{
				if (ch0 != '\n')
				{
					ssContent << ch0;
				}
			}
		}
		return true;
	}

	bool XmlFile::saveFile(const char* const fileName) const
	{
		if (std::filesystem::exists(fileName) == true)
		{
			FS_ASSERT("김장원", false, "파일이 이미 존재하므로 덮어씌웁니다.");
		}

		std::ofstream ofs{ fileName };
		FS_ASSERT("김장원", ofs.is_open() == true, "파일을 열지 못했습니다.");
		if (ofs.is_open() == false)
		{
			return false;
		}

		// XML Declaration
		std::string string{};
		appendDeclarationTagToString(_xmlDeclaration.getTag(), string);
		ofs.write(string.c_str(), string.size());

		// From root node down to all its child nodes
		string.clear();
		nodeToStringRecursive(_rootNode, string);
		ofs.write(string.c_str(), string.size());

		return true;
	}

	void XmlFile::nodeToStringRecursive(const XmlNode* const node, std::string& string) const noexcept
	{
		if (node == nullptr)
		{
			return;
		}

		const Tag& tag{ node->_element.getTag() };
		Tag::Type tagType{ tag._type };
		if (tagType == Tag::Type::StartEndTag)
		{
			// Start tag
			appendStartTagToString(tag, string);
		}
		else if (tagType == Tag::Type::EmptyElementTag)
		{
			appendEmptyElementTagToString(tag, string);
			return;
		}

		if (node->_childNodeArray.size())
		{
			for (const auto& childNode : node->_childNodeArray)
			{
				nodeToStringRecursive(childNode, string);
			}
		}
		else
		{
			if (tagType == Tag::Type::StartEndTag)
			{
				string += node->_element.getContent();
			}
		}

		if (tagType == Tag::Type::StartEndTag)
		{
			// End tag
			appendEndTagToString(tag, string);
		}
	}

	const XmlFile::Node* XmlFile::getRootNode() const noexcept
	{
		return _rootNode;
	}

	const XmlFile::Element& XmlFile::getXmlDeclaration() const noexcept
	{
		return _xmlDeclaration;
	}

	XmlFile::Node* XmlFile::createRootNode()
	{
		FS_DELETE(_rootNode);
		_rootNode = FS_NEW(Node);
		return _rootNode;
	}

	XmlFile::Node* XmlFile::insertChildNode(Node* const parentNode)
	{
		FS_ASSERT("김장원", parentNode != nullptr, "부모 노드가 nullptr일 수 없습니다.");
		Node* const childNode = FS_NEW(Node);
		childNode->_parentNode = parentNode;
		parentNode->_childNodeArray.emplace_back(childNode);
		return childNode;
	}

	void XmlFile::parseTagName(const char* const tag, std::string& string) noexcept
	{
		FS_ASSERT("김장원", tag != nullptr, "tag는 nullptr이면 안 됩니다.");

		const uint32 tagLength{ static_cast<uint32>(strlen(tag)) };
		uint32 spaceIndex{};
		FS_ASSERT("김장원", tagLength >= 3, "tag의 최소 길이는 3 이상입니다.");
		for (uint32 i = 0; i < tagLength; ++i)
		{
			if (isWhiteSpace(tag[i]) == true || tag[i] == '>')
			{
				spaceIndex = i;
				break;
			}
		}
		FS_ASSERT("김장원", spaceIndex > 1, "< 뒤 공백의 위치가 유효하지 않습니다.");

		if (tag[1] == '/')
		{
			// end tag
			string.assign(&tag[2], spaceIndex - 2);
		}
		else
		{
			if (tag[tagLength - 2] == '/')
			{
				// empty-element tag
				string.assign(&tag[1], (spaceIndex == tagLength - 1) ? spaceIndex - 2 : spaceIndex - 1);
			}
			else
			{
				// start tag
				string.assign(&tag[1], spaceIndex - 1);
			}
		}
	}

	void XmlFile::parseAttributes(const char* const tag, std::vector<XmlAttribute>& attributeArray) noexcept
	{
		FS_ASSERT("김장원", tag != nullptr, "tag는 nullptr이면 안 됩니다.");

		int32 tagLength{ (int32)strlen(tag) }, begin{ -1 }, equal{}, afterQuoteOpen{ -1 }, beforeQuoteClose{};
		std::string name{};
		std::string value{};
		for (int32 i = 0; i < tagLength; ++i)
		{
			if (isEqualSign(tag[i]) == true)
			{
				equal = i;
			}
			else if ((isWhiteSpace(tag[i]) == true) && (begin == -1))
			{
				begin = i + 1;
				while (isWhiteSpace(tag[begin]) == true)
				{
					++begin;
				}
			}
			else if (isQuote(tag[i]) == true)
			{
				if (afterQuoteOpen == -1)
				{
					afterQuoteOpen = i + 1;
					while (isWhiteSpace(tag[afterQuoteOpen]) == true)
					{
						++afterQuoteOpen;
					}
					continue;
				}
				else
				{
					beforeQuoteClose = i - 1;
					while (isWhiteSpace(tag[beforeQuoteClose]) == true)
					{
						--beforeQuoteClose;
					}

					FS_ASSERT("김장원", (equal > begin) && (equal < afterQuoteOpen), "등호(=)는 속성 이름과 값 사이에 있어야 합니다.");

					int32 nameLength{ equal - begin };
					FS_ASSERT("김장원", nameLength > 0, "이름의 길이가 0 이하일 수 없습니다.");
					name.reserve(static_cast<uint64>(nameLength) + 1);
					strncpy_s(&name[0], static_cast<uint64>(nameLength) + 1, &tag[begin], nameLength);

					int32 valueLength{ beforeQuoteClose - afterQuoteOpen + 1 };
					FS_ASSERT("김장원", valueLength > 0, "값의 길이가 0 이하일 수 없습니다.");
					value.reserve(static_cast<uint64>(valueLength) + 1);
					strncpy_s(&value[0], static_cast<uint64>(valueLength) + 1, &tag[afterQuoteOpen], valueLength);

					attributeArray.emplace_back(name.c_str(), value.c_str());

					afterQuoteOpen = -1; // don't forget to reinitialize @bQuoteOpen

					begin = i + 1; // don't forget to set next @begin value
					while (isWhiteSpace(tag[begin]) == true)
					{
						++begin;
					}
				}
			}
		}
	}

	void XmlFile::appendDeclarationTagToString(const XmlFile::Tag& tag, std::string& string)
	{
		if (tag._type != Tag::Type::XmlDeclaration)
		{
			return;
		}

		string += "<?xml"; // 소문자임에 주의!
		for (auto& attribute : tag._attributeArray)
		{
			string += " ";
			string += attribute.getName();
			string += "=\"";
			string += attribute.getValue();
			string += "\"";
		}
		string += "?>\n";
	}

	void XmlFile::appendStartTagToString(const XmlFile::Tag& tag, std::string& string)
	{
		if (tag._type != Tag::Type::StartEndTag)
		{
			return;
		}

		string += "<";
		string += tag._name;

		for (auto& attribute : tag._attributeArray)
		{
			string += " ";
			string += attribute.getName();
			string += "=\"";
			string += attribute.getValue();
			string += "\"";
		}
		string += ">";
	}

	void XmlFile::appendEndTagToString(const XmlFile::Tag& tag, std::string& string)
	{
		if (tag._type != Tag::Type::StartEndTag)
		{
			return;
		}

		string += "</";
		string += tag._name;
		string += ">";
	}

	void XmlFile::appendEmptyElementTagToString(const XmlFile::Tag& tag, std::string& string)
	{
		if (tag._type != Tag::Type::EmptyElementTag)
		{
			return;
		}

		string += "<";
		string += tag._name;
		for (auto& attribute : tag._attributeArray)
		{
			string += " ";
			string += attribute.getName();
			string += "=\"";
			string += attribute.getValue();
			string += "\"";
		}
		string += "/>";
	}

	bool XmlFile::isWhiteSpace(char ch) noexcept
	{
		return (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n');
	}

	bool XmlFile::isQuote(char ch) noexcept
	{
		return (ch == '\"');
	}

	bool XmlFile::isEqualSign(char ch) noexcept
	{
		return (ch == '=');
	}


	XmlFile::Attribute::Attribute()
	{
		__noop;
	}

	XmlFile::Attribute::Attribute(const std::string& name, const std::string& value)
		: _name{ name }
		, _value{ value }
	{
		__noop;
	}

	XmlFile::Attribute::Attribute(const Attribute& rhs) noexcept
		: _name{ rhs._name }
		, _value{ rhs._value }
	{
		__noop;
	}

	XmlFile::Attribute::Attribute(Attribute&& rhs) noexcept
	{
		std::swap(_name, rhs._name);
		std::swap(_value, rhs._value);
	}

	XmlFile::Attribute::~Attribute()
	{
		__noop;
	}

	XmlFile::Attribute& XmlFile::Attribute::operator=(const Attribute& rhs) noexcept
	{
		_name = rhs._name;
		_value = rhs._value;
		return *this;
	}

	XmlFile::Attribute& XmlFile::Attribute::operator=(Attribute&& rhs) noexcept
	{
		if (this != &rhs)
		{
			std::swap(_name, rhs._name);
			std::swap(_value, rhs._value);
		}
		return *this;
	}

	const std::string& XmlFile::Attribute::getName() const noexcept
	{
		return _name;
	}

	const std::string& XmlFile::Attribute::getValue() const noexcept
	{
		return _value;
	}

	bool XmlFile::Attribute::getValueBool() const noexcept
	{
		return (_value == "true") ? true : false;
	}

	int8 XmlFile::Attribute::getValueInt8() const noexcept
	{
		return static_cast<int8>(atoi(_value.c_str()));
	}

	int16 XmlFile::Attribute::getValueInt16() const noexcept
	{
		return static_cast<int16>(atoi(_value.c_str()));
	}

	int32 XmlFile::Attribute::getValueInt32() const noexcept
	{
		return static_cast<int32>(atoi(_value.c_str()));
	}

	float XmlFile::Attribute::getValueFloat() const noexcept
	{
		return static_cast<float>(atof(_value.c_str()));
	}

	double XmlFile::Attribute::getValueDouble() const noexcept
	{
		return atof(_value.c_str());
	}


	XmlFile::Tag::Tag()
		: _type{ Type::Invalid }
	{
		__noop;
	}

	XmlFile::Tag::Tag(Type type, const std::string& name)
		: _type{ type }
		, _name{ name }
	{
		FS_ASSERT("김장원", type != Type::Invalid, "Tag의 type이 유효하지 않습니다.");
	}

	XmlFile::Tag::Tag(const Tag& rhs) noexcept
		: _type{ rhs._type }
		, _name{ rhs._name }
		, _attributeArray{ rhs._attributeArray }
	{
		__noop;
	}

	XmlFile::Tag::Tag(Tag&& rhs) noexcept
	{
		std::swap(_type, rhs._type);
		std::swap(_name, rhs._name);
		std::swap(_attributeArray, rhs._attributeArray);
	}

	XmlFile::Tag::~Tag()
	{
		__noop;
	}

	XmlFile::Tag& XmlFile::Tag::operator=(const Tag& rhs) noexcept
	{
		_type = rhs._type;
		_name = rhs._name;
		_attributeArray = rhs._attributeArray;
		return *this;
	}

	XmlFile::Tag& XmlFile::Tag::operator=(Tag&& rhs) noexcept
	{
		if (this != &rhs)
		{
			std::swap(_type, rhs._type);
			std::swap(_name, rhs._name);
			std::swap(_attributeArray, rhs._attributeArray);
		}
		return *this;
	}

	XmlFile::Tag::Type XmlFile::Tag::getType() const noexcept
	{
		return _type;
	}

	const std::string& XmlFile::Tag::getName() const noexcept
	{
		return _name;
	}

	const std::vector<XmlFile::Attribute>& XmlFile::Tag::getAttributes() const noexcept
	{
		return _attributeArray;
	}


	XmlFile::Element::Element()
	{
		__noop;
	}

	XmlFile::Element::Element(const Element& rhs) noexcept
		: _content{ rhs._content }
		, _tag{ rhs._tag }
	{
		__noop;
	}

	XmlFile::Element::Element(Element&& rhs) noexcept
	{
		std::swap(_content, rhs._content);
		std::swap(_tag, rhs._tag);
	}

	XmlFile::Element::~Element()
	{
		__noop;
	}

	XmlFile::Element& XmlFile::Element::operator=(const Element& rhs) noexcept
	{
		_content = rhs._content;
		_tag = rhs._tag;
		return *this;
	}

	XmlFile::Element& XmlFile::Element::operator=(Element&& rhs) noexcept
	{
		if (this != &rhs)
		{
			std::swap(_content, rhs._content);
			std::swap(_tag, rhs._tag);
		}
		return *this;
	}

	void XmlFile::Element::setContent(const std::string& content)
	{
		_content = content;
	}

	const std::string& XmlFile::Element::getContent() const noexcept
	{
		return _content;
	}

	bool XmlFile::Element::getContentBool() const noexcept
	{
		return (_content == "true") ? true : false;
	}

	int8 XmlFile::Element::getContentInt8() const noexcept
	{
		return static_cast<int8>(atoi(_content.c_str()));
	}

	int16 XmlFile::Element::getContentInt16() const noexcept
	{
		return static_cast<int16>(atoi(_content.c_str()));
	}

	int32 XmlFile::Element::getContentInt32() const noexcept
	{
		return static_cast<int32>(atoi(_content.c_str()));
	}

	float XmlFile::Element::getContentFloat() const noexcept
	{
		return static_cast<float>(atof(_content.c_str()));
	}

	double XmlFile::Element::getContentDouble() const noexcept
	{
		return atof(_content.c_str());
	}

	void XmlFile::Element::setTag(const Tag& tag)
	{
		_tag = tag;
	}

	const XmlFile::Tag& XmlFile::Element::getTag() const noexcept
	{
		return _tag;
	}


	XmlFile::Node::Node()
		: _parentNode{ nullptr }
	{
		__noop;
	}

	XmlFile::Node::Node(Node* const parentNode, const XmlElement& element)
		: _parentNode{ parentNode }
		, _element{ element }
	{
		__noop;
	}

	XmlFile::Node::~Node()
	{
		destroyChildren();
	}

	void XmlFile::Node::destroyChildren()
	{
		for (auto& childNode : _childNodeArray)
		{
			_destroyRecursive(childNode);
		}
	}

	void XmlFile::Node::_destroyRecursive(Node*& currentNode)
	{
		if (currentNode == nullptr)
		{
			return;
		}

		for (auto& childNode : currentNode->_childNodeArray)
		{
			_destroyRecursive(childNode);
		}

		FS_DELETE(currentNode);
	}

	XmlFile::Node* XmlFile::Node::getParent() noexcept
	{
		return _parentNode;
	}

	std::vector<XmlFile::Node*>& XmlFile::Node::getChildNodes() noexcept
	{
		return _childNodeArray;
	}

	XmlFile::Node* XmlFile::Node::getFirstChild() noexcept
	{
		return (_childNodeArray.empty() == false) ? _childNodeArray[0] : nullptr;
	}

	XmlFile::Node* XmlFile::Node::getNextSibling() noexcept
	{
		if (_parentNode != nullptr)
		{
			const auto& parentChildren{ _parentNode->getChildNodes() };
			const uint32 parentChildrenCount{ static_cast<uint32>(parentChildren.size()) };
			for (uint32 i = 0; i < parentChildrenCount; ++i)
			{
				if (parentChildren[i] == this)
				{
					return (i + 1 < parentChildrenCount) ? parentChildren[static_cast<uint64>(i) + 1] : nullptr;
				}
			}
		}
		return nullptr;
	}

	const XmlFile::Element& XmlFile::Node::getElement() const noexcept
	{
		return _element;
	}
}
