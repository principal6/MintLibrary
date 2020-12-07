#pragma once


#ifndef FS_TREE_H
#define FS_TREE_H


#include <CommonDefinitions.h>
#include <Container/Vector.h>


namespace fs
{
	template <typename T>
	class Tree;

	template <typename T>
	class TreeNode;


	template <typename T>
	class TreeNodeAccessor
	{
		friend Tree;
		friend TreeNode;

		static constexpr uint32			kInvalidNodeId = kUint32Max;

	public:
										TreeNodeAccessor();
										TreeNodeAccessor(const TreeNodeAccessor& rhs) = default;
										TreeNodeAccessor(TreeNodeAccessor&& rhs) = default;

	private:
										TreeNodeAccessor(Tree<T>* const tree, const uint32 slotIndex, const uint32 nodeId);

	public:
		TreeNodeAccessor&				operator=(const TreeNodeAccessor& rhs) = default;
		TreeNodeAccessor&				operator=(TreeNodeAccessor&& rhs) noexcept = default;

	public:
		const bool						operator==(const TreeNodeAccessor& rhs) const noexcept;

	public:
		const bool						isValid() const noexcept;
	
	public:
		void							setNodeData(const T& data) noexcept;
		const T&						getNodeData() const noexcept;
		T&								getNodeDataXXX() noexcept;

	public:
		TreeNodeAccessor<T>				getParentNode() const noexcept;
		const uint32					getChildNodeCount() const noexcept;
		TreeNodeAccessor<T>				getChildNode(const uint32 childNodeIndex) const noexcept;
		TreeNodeAccessor<T>				getNextSiblingNode() const noexcept;

	public:
		TreeNodeAccessor				insertChildNode(const T& data);
		void							eraseChildNode(TreeNodeAccessor<T>& childNodeAccessor);
		void							clearChildNodes();

	public:
		void							moveToParent(const TreeNodeAccessor<T>& newParentNodeAccessor);

	private:
		Tree<T>*						_tree;

	private:
		uint32							_slotIndex;
		uint32							_nodeId;

	public:
		static const TreeNodeAccessor	kInvalidTreeNodeAccessor;
		static const T					kInvalidData;
		static T						nkInvalidData;
	};


	template <typename T>
	class TreeNode
	{
		friend Tree;
		friend TreeNodeAccessor;

	public:
		TreeNode()
			: _nodeId{ TreeNodeAccessor<T>::kInvalidNodeId }
			, _parentNodeAccessor{ TreeNodeAccessor<T>::kInvalidTreeNodeAccessor }
		{
			__noop;
		}

	private:
		TreeNode(const uint32 nodeId, const TreeNodeAccessor<T>& parentNodeAccessor, const T& data)
			: _nodeId{ nodeId }
			, _data{ data }
			, _parentNodeAccessor{ parentNodeAccessor }
		{
			__noop;
		}

		TreeNode(const uint32 nodeId, const TreeNodeAccessor<T>& parentNodeAccessor, T&& data)
			: _nodeId{ nodeId }
			, _data{ data }
			, _parentNodeAccessor{ parentNodeAccessor }
		{
			__noop;
		}

	public:
		const bool							isValid() const noexcept;
		void								invalidate() noexcept;

	private:
		uint32								_nodeId;
		T									_data;

		TreeNodeAccessor<T>					_parentNodeAccessor;
		fs::Vector<TreeNodeAccessor<T>>		_childNodeAccessorArray;
	};


	template <typename T>
	class Tree
	{
		static constexpr uint32					kDefaultNodeArraySize = 16;
	public:
												Tree();
												~Tree();

	public:
		TreeNodeAccessor<T>						createRootNode(const T& rootNodeData);
		void									destroyRootNode();
		TreeNodeAccessor<T>						getRootNode() noexcept;
		
	public:
		TreeNodeAccessor<T>						findNode(const TreeNodeAccessor<T>& startNodeAccessor, const T& nodeData) const noexcept;
		
	public:
		TreeNodeAccessor<T>						getParentNode(const TreeNodeAccessor<T>& nodeAccessor) const noexcept;
		const uint32							getChildNodeCount(const TreeNodeAccessor<T>& nodeAccessor) const noexcept;
		TreeNodeAccessor<T>						getChildNode(const TreeNodeAccessor<T>& nodeAccessor, const uint32 childNodeIndex) const noexcept;
		TreeNodeAccessor<T>						getNextSiblingNode(const TreeNodeAccessor<T>& nodeAccessor) const noexcept;

	public:
		const bool								isValidNode(const TreeNodeAccessor<T>& nodeAccessor) const noexcept;
		void									setNodeData(const TreeNodeAccessor<T>& nodeAccessor, const T& nodeData) noexcept;
		const T&								getNodeData(const TreeNodeAccessor<T>& nodeAccessor) const noexcept;
		T&										getNodeDataXXX(const TreeNodeAccessor<T>& nodeAccessor) noexcept;
		void									swapNodeData(const TreeNodeAccessor<T>& nodeAccessorA, const TreeNodeAccessor<T>& nodeAccessorB) noexcept;

	private:
		const TreeNode<T>&						getNodeXXX(const TreeNodeAccessor<T>& nodeAccessor) const;
		TreeNode<T>&							getNodeXXX(const TreeNodeAccessor<T>& nodeAccessor);

	public:
		TreeNodeAccessor<T> [[nodiscard]]		insertChildNode(const TreeNodeAccessor<T>& nodeAccessor, const T& childNodeData);
		void									eraseChildNode(TreeNodeAccessor<T>& nodeAccessor, TreeNodeAccessor<T>& childNodeAccessor);
		void									clearChildNodes(TreeNodeAccessor<T>& nodeAccessor);
	
	public:
		void									moveToParent(TreeNodeAccessor<T>& nodeAccessor, const TreeNodeAccessor<T>& newParentNodeAccessor);

	private:
		const uint32							getAvailableNodeSlot();

	private:
		fs::Vector<TreeNode<T>>					_nodeArray;
		uint32									_nextNodeId;
		uint32									_nodeCount;
	};
}


#endif // !FS_TREE_H
