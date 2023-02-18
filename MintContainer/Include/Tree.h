#pragma once


#ifndef _MINT_CONTAINER_TREE_H_
#define _MINT_CONTAINER_TREE_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>


namespace mint
{
	template <typename T>
	class Tree;

	template <typename T>
	class TreeNode;


	template <typename T>
	class TreeNodeAccessor
	{
		friend Tree<T>;
		friend TreeNode<T>;

		static constexpr uint32 kInvalidNodeID = kUint32Max;

	public:
		TreeNodeAccessor();
		TreeNodeAccessor(const TreeNodeAccessor& rhs) = default;
		TreeNodeAccessor(TreeNodeAccessor&& rhs) = default;

	private:
		TreeNodeAccessor(Tree<T>* const tree, const uint32 slotIndex, const uint32 nodeID);

	public:
		TreeNodeAccessor& operator=(const TreeNodeAccessor& rhs) = default;
		TreeNodeAccessor& operator=(TreeNodeAccessor&& rhs) noexcept = default;

	public:
		bool operator==(const TreeNodeAccessor& rhs) const noexcept;

	public:
		bool IsValid() const noexcept;

	public:
		void SetNodeData(const T& data) noexcept;
		const T& GetNodeData() const noexcept;
		T& GetNodeDataXXX() noexcept;

	public:
		TreeNodeAccessor<T> GetParentNode() const noexcept;
		uint32 GetChildNodeCount() const noexcept;
		TreeNodeAccessor<T> GetChildNode(const uint32 childNodeIndex) const noexcept;
		TreeNodeAccessor<T> GetNextSiblingNode() const noexcept;

	public:
		TreeNodeAccessor InsertChildNode(const T& data);
		void EraseChildNode(TreeNodeAccessor<T>& childNodeAccessor);
		void ClearChildNodes();

	public:
		void MoveToParent(const TreeNodeAccessor<T>& newParentNodeAccessor);

	private:
		Tree<T>* _tree;

	private:
		uint32 _slotIndex;
		uint32 _nodeID;

	public:
		static const TreeNodeAccessor kInvalidTreeNodeAccessor;
		static const T kInvalidData;
		static T nkInvalidData;
	};


	template <typename T>
	class TreeNode
	{
		friend Tree;
		friend TreeNodeAccessor;

	public:
		TreeNode()
			: _nodeID{ TreeNodeAccessor<T>::kInvalidNodeID }
			, _parentNodeAccessor{ TreeNodeAccessor<T>::kInvalidTreeNodeAccessor }
		{
			__noop;
		}

	private:
		TreeNode(const uint32 nodeID, const TreeNodeAccessor<T>& parentNodeAccessor, const T& data)
			: _nodeID{ nodeID }
			, _data{ data }
			, _parentNodeAccessor{ parentNodeAccessor }
		{
			__noop;
		}

		TreeNode(const uint32 nodeID, const TreeNodeAccessor<T>& parentNodeAccessor, T&& data)
			: _nodeID{ nodeID }
			, _data{ data }
			, _parentNodeAccessor{ parentNodeAccessor }
		{
			__noop;
		}

	public:
		bool IsValid() const noexcept;
		void Invalidate() noexcept;

	private:
		uint32 _nodeID;
		T _data;

		TreeNodeAccessor<T> _parentNodeAccessor;
		Vector<TreeNodeAccessor<T>> _childNodeAccessorArray;
	};


	template <typename T>
	class Tree
	{
		static constexpr uint32 kDefaultNodeArraySize = 16;
		static_assert(kDefaultNodeArraySize > 0, "kDefaultNodeArraySize must not be 0");

	public:
		Tree();
		~Tree();

	public:
		TreeNodeAccessor<T> CreateRootNode(const T& rootNodeData);
		void DestroyRootNode();
		TreeNodeAccessor<T> GetRootNode() noexcept;

	public:
		TreeNodeAccessor<T> FindNode(const TreeNodeAccessor<T>& startNodeAccessor, const T& nodeData) const noexcept;

	public:
		TreeNodeAccessor<T> GetParentNode(const TreeNodeAccessor<T>& nodeAccessor) const noexcept;
		uint32 GetChildNodeCount(const TreeNodeAccessor<T>& nodeAccessor) const noexcept;
		TreeNodeAccessor<T> GetChildNode(const TreeNodeAccessor<T>& nodeAccessor, const uint32 childNodeIndex) const noexcept;
		TreeNodeAccessor<T> GetNextSiblingNode(const TreeNodeAccessor<T>& nodeAccessor) const noexcept;

	public:
		bool IsValidNode(const TreeNodeAccessor<T>& nodeAccessor) const noexcept;
		void SetNodeData(const TreeNodeAccessor<T>& nodeAccessor, const T& nodeData) noexcept;
		const T& GetNodeData(const TreeNodeAccessor<T>& nodeAccessor) const noexcept;
		T& GetNodeDataXXX(const TreeNodeAccessor<T>& nodeAccessor) noexcept;
		void SwapNodeData(const TreeNodeAccessor<T>& nodeAccessorA, const TreeNodeAccessor<T>& nodeAccessorB) noexcept;

	private:
		const TreeNode<T>& GetNodeXXX(const TreeNodeAccessor<T>& nodeAccessor) const;
		TreeNode<T>& GetNodeXXX(const TreeNodeAccessor<T>& nodeAccessor);

	public:
		TreeNodeAccessor<T> [[nodiscard]] InsertChildNode(const TreeNodeAccessor<T>& nodeAccessor, const T& childNodeData);
		void EraseChildNode(TreeNodeAccessor<T>& nodeAccessor, TreeNodeAccessor<T>& childNodeAccessor);
		void ClearChildNodes(TreeNodeAccessor<T>& nodeAccessor);

	public:
		void MoveToParent(TreeNodeAccessor<T>& nodeAccessor, const TreeNodeAccessor<T>& newParentNodeAccessor);

	private:
		uint32 GetAvailableNodeSlot();

	private:
		Vector<TreeNode<T>> _nodeArray;
		uint32 _nextNodeID;
		uint32 _nodeCount;
	};
}


#endif // !_MINT_CONTAINER_TREE_H_
