#pragma once


#ifndef FS_XML_FILE_H
#define FS_XML_FILE_H


#include <CommonDefinitions.h>
#include <vector>
#include <string>


namespace fs
{
	class XmlFile;


	// <startTag attributeName="value">content</endTag> == element
	// <emptyElementTag attributeName="value"/>

	// Load XML file into a tree structure
	// Save XML file
	class XmlFile
	{
	public:
		class Attribute final
		{
		public:
											Attribute();
											Attribute(const std::string& name, const std::string& value);
											Attribute(const Attribute& rhs) noexcept;
											Attribute(Attribute&& rhs) noexcept;
											~Attribute();

		public:
			Attribute&						operator=(const Attribute& rhs) noexcept;
			Attribute&						operator=(Attribute&& rhs) noexcept;

		public:
			const std::string&				getName() const noexcept;
			const std::string&				getValue() const noexcept;
			bool							getValueBool() const noexcept;
			int8							getValueInt8() const noexcept;
			int16							getValueInt16() const noexcept;
			int32							getValueInt32() const noexcept;
			float							getValueFloat() const noexcept;
			double							getValueDouble() const noexcept;

		private:
			std::string						_name;
			std::string						_value;
		};
		using XmlAttribute					= Attribute;


		class Tag final
		{
			friend XmlFile;

		public:
			enum class Type
			{
				Invalid,
				StartEndTag,
				EmptyElementTag,
				XmlDeclaration,
			};
			using TagType					= Tag::Type;

		public:
											Tag();
											Tag(Type type, const std::string& name);
											Tag(const Tag& rhs) noexcept;
											Tag(Tag&& rhs) noexcept;
											~Tag();

		public:
			Tag&							operator=(const Tag& rhs) noexcept;
			Tag&							operator=(Tag&& rhs) noexcept;

		public:
			Type							getType() const noexcept;
			const std::string&				getName() const noexcept;
			const std::vector<Attribute>&	getAttributes() const noexcept;

		private:
			Type							_type;
			std::string						_name;
			std::vector<Attribute>			_attributeArray;
		};
		using XmlTag						= Tag;


		class Element
		{
		public:
											Element();
											Element(const Element& rhs) noexcept;
											Element(Element&& rhs) noexcept;
			virtual							~Element();

		public:
			Element&						operator=(const Element& rhs) noexcept;
			Element&						operator=(Element&& rhs) noexcept;

		public:
			void							setContent(const std::string& content);
			const std::string&				getContent() const noexcept;
			bool							getContentBool() const noexcept;
			int8							getContentInt8() const noexcept;
			int16							getContentInt16() const noexcept;
			int32							getContentInt32() const noexcept;
			float							getContentFloat() const noexcept;
			double							getContentDouble() const noexcept;
			void							setTag(const Tag& tag);
			const Tag&						getTag() const noexcept;

		private:
			Tag								_tag;
			std::string						_content;
		};
		using XmlElement					= Element;


		class Node
		{
			friend XmlFile;

		public:
											Node();
											Node(Node* const parentNode, const XmlElement& element);
											~Node();

		public:
			void							destroyChildren();

		private:
			void							_destroyRecursive(Node*& currentNode);

		public:
			Node*							getParent() noexcept;
			std::vector<Node*>&				getChildNodes() noexcept;
			Node*							getFirstChild() noexcept;
			Node*							getNextSibling() noexcept;
			const XmlElement&				getElement() const noexcept;

		private:
			Node*							_parentNode;
			std::vector<Node*>				_childNodeArray;
			Element							_element;
		};
		using XmlNode						= Node;


	public:
		explicit							XmlFile();
		explicit							XmlFile(const char* const fileName);
											XmlFile(const XmlFile& rhs) = delete;
											XmlFile(XmlFile&& rhs) noexcept;
											~XmlFile();

	public:
		XmlFile&							operator=(const XmlFile& rhs) = delete;
		XmlFile&							operator=(XmlFile&& rhs) noexcept;

	public:
		bool								isValid() const noexcept;

	public:
		bool								openFile(const char* const fileName);
		bool								saveFile(const char* const fileName) const;

	private:
		void								nodeToStringRecursive(const XmlNode* const node, std::string& string) const noexcept;

	public:
		const Node*							getRootNode() const noexcept;
		const XmlElement&					getXmlDeclaration() const noexcept;

	public:
		Node*								createRootNode();
		Node*								insertChildNode(Node* const parentNode);

	private:
		static void							parseTagName(const char* const tag, std::string& string) noexcept;
		static void							parseAttributes(const char* const tag, std::vector<XmlAttribute>& attributeArray) noexcept;
		static void							appendDeclarationTagToString(const Tag& tag, std::string& string);
		static void							appendStartTagToString(const Tag& tag, std::string& string);
		static void							appendEndTagToString(const Tag& tag, std::string& string);
		static void							appendEmptyElementTagToString(const Tag& tag, std::string& string);

	private:
		static bool							isWhiteSpace(char ch) noexcept;
		static bool							isQuote(char ch) noexcept;
		static bool							isEqualSign(char ch) noexcept;

	private:
		XmlElement							_xmlDeclaration;
		Node*								_rootNode;
	};
}


#endif // !FS_XML_FILE_H
