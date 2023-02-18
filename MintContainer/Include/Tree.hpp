#pragma once


#include <MintContainer/Include/Tree.h>

#include <MintContainer/Include/Vector.hpp>


namespace mint
{
	template<typename T>
	const TreeNodeAccessor<T> TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;

	template<typename T>
	const T TreeNodeAccessor<T>::kInvalidData;
	template<typename T>
	T TreeNodeAccessor<T>::nkInvalidData;

	template<typename T>
	TreeNodeAccessor<T>::TreeNodeAccessor()
		: _tree{ nullptr }
		, _slotIndex{ 0 }
		, _nodeID{ kInvalidNodeID }
	{
		__noop;
	}

	template<typename T>
	TreeNodeAccessor<T>::TreeNodeAccessor(Tree<T>* const tree, const uint32 slotIndex, const uint32 nodeID)
		: _tree{ tree }
		, _slotIndex{ slotIndex }
		, _nodeID{ nodeID }
	{
		__noop;
	}

	template<typename T>
	bool TreeNodeAccessor<T>::operator==(const TreeNodeAccessor<T>& rhs) const noexcept
	{
		return (_nodeID == rhs._nodeID) && (_slotIndex == rhs._slotIndex);
	}

	template<typename T>
	bool TreeNodeAccessor<T>::IsValid() const noexcept
	{
		if (_tree != nullptr)
		{
			return _tree->IsValidNode(*this);
		}
		return false;
	}

	template<typename T>
	void TreeNodeAccessor<T>::SetNodeData(const T& data) noexcept
	{
		if (_tree != nullptr)
		{
			_tree->SetNodeData(*this, data);
		}
	}

	template<typename T>
	const T& TreeNodeAccessor<T>::GetNodeData() const noexcept
	{
		if (_tree != nullptr)
		{
			return _tree->GetNodeData(*this);
		}
		return kInvalidData;
	}

	template<typename T>
	inline T& TreeNodeAccessor<T>::GetNodeDataXXX() noexcept
	{
		if (_tree != nullptr)
		{
			return _tree->GetNodeDataXXX(*this);
		}
		return nkInvalidData;
	}

	template<typename T>
	inline TreeNodeAccessor<T> TreeNodeAccessor<T>::GetParentNode() const noexcept
	{
		if (_tree != nullptr)
		{
			return _tree->GetParentNode(*this);
		}
		return TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
	}

	template<typename T>
	inline uint32 TreeNodeAccessor<T>::GetChildNodeCount() const noexcept
	{
		if (_tree != nullptr)
		{
			return _tree->GetChildNodeCount(*this);
		}
		return 0;
	}

	template<typename T>
	inline TreeNodeAccessor<T> TreeNodeAccessor<T>::GetChildNode(const uint32 childNodeIndex) const noexcept
	{
		if (_tree != nullptr)
		{
			return _tree->GetChildNode(*this, childNodeIndex);
		}
		return TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
	}

	template<typename T>
	inline TreeNodeAccessor<T> TreeNodeAccessor<T>::GetNextSiblingNode() const noexcept
	{
		if (_tree != nullptr)
		{
			return _tree->GetNextSiblingNode(*this);
		}
		return TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
	}

	template<typename T>
	TreeNodeAccessor<T> TreeNodeAccessor<T>::InsertChildNode(const T& data)
	{
		if (IsValid() == true)
		{
			return _tree->InsertChildNode(*this, data);
		}
		return TreeNodeAccessor::kInvalidTreeNodeAccessor;
	}

	template<typename T>
	inline void TreeNodeAccessor<T>::EraseChildNode(TreeNodeAccessor<T>& childNodeAccessor)
	{
		if (_tree != nullptr)
		{
			return _tree->EraseChildNode(*this, childNodeAccessor);
		}
	}

	template<typename T>
	inline void TreeNodeAccessor<T>::ClearChildNodes()
	{
		if (_tree != nullptr)
		{
			return _tree->ClearChildNodes(*this);
		}
	}

	template<typename T>
	void TreeNodeAccessor<T>::MoveToParent(const TreeNodeAccessor<T>& newParentNodeAccessor)
	{
		if (IsValid() == true)
		{
			_tree->MoveToParent(*this, newParentNodeAccessor);
		}
	}


	template<typename T>
	inline bool TreeNode<T>::IsValid() const noexcept
	{
		return (_nodeID != TreeNodeAccessor<T>::kInvalidNodeID);
	}

	template<typename T>
	inline void TreeNode<T>::Invalidate() noexcept
	{
		_nodeID = TreeNodeAccessor<T>::kInvalidNodeID;
		_data = T(); // T 가 동적 할당된 메모리를 들고 있다면 이게 더 비용이 쌀 가능성이 높으므로...
		_parentNodeAccessor = TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
	}


	template<typename T>
	inline Tree<T>::Tree()
		: _nextNodeID{ 0 }
		, _nodeCount{ 0 }
	{
		_nodeArray.Resize(kDefaultNodeArraySize);
	}

	template<typename T>
	inline Tree<T>::~Tree()
	{
		__noop;
	}

	template<typename T>
	inline TreeNodeAccessor<T> Tree<T>::CreateRootNode(const T& rootNodeData)
	{
		if (_nodeCount == 0)
		{
			TreeNode rootNode{ _nextNodeID, TreeNodeAccessor<T>::kInvalidTreeNodeAccessor, rootNodeData };

			++_nextNodeID;
			++_nodeCount;

			_nodeArray[0] = rootNode;
		}

		return GetRootNode();
	}

	template<typename T>
	inline void Tree<T>::DestroyRootNode()
	{
		if (_nodeCount > 0)
		{
			TreeNodeAccessor<T> rootNodeAccessor = GetRootNode();
			ClearChildNodes(rootNodeAccessor);

			TreeNode<T>& rootNode = GetNodeXXX(rootNodeAccessor);
			rootNode.Invalidate();

			_nodeCount = 0;
		}
	}

	template<typename T>
	inline TreeNodeAccessor<T> Tree<T>::GetRootNode() noexcept
	{
		const TreeNode<T>& rootNode = _nodeArray.Front();
		return TreeNodeAccessor<T>::TreeNodeAccessor(this, static_cast<uint32>(0), rootNode._nodeID); // this == Tree<T>* 가 const 이면 안 되므로 이 함수는 const 함수일 수 없다!!!
	}

	template<typename T>
	TreeNodeAccessor<T> Tree<T>::FindNode(const TreeNodeAccessor<T>& startNodeAccessor, const T& nodeData) const noexcept
	{
		if (GetNodeData(startNodeAccessor) == nodeData)
		{
			return startNodeAccessor;
		}

		const TreeNode<T>& startNode = GetNodeXXX(startNodeAccessor);
		const uint32 childCount = static_cast<uint32>(startNode._childNodeAccessorArray.Size());
		for (uint32 childIndex = 0; childIndex < childCount; ++childIndex)
		{
			TreeNodeAccessor<T> foundNodeAccessor = FindNode(startNode._childNodeAccessorArray[childIndex], nodeData);
			if (GetNodeData(foundNodeAccessor) == nodeData)
			{
				return foundNodeAccessor;
			}
		}

		return TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
	}

	template<typename T>
	inline TreeNodeAccessor<T> Tree<T>::GetParentNode(const TreeNodeAccessor<T>& nodeAccessor) const noexcept
	{
		if (IsValidNode(nodeAccessor) == false)
		{
			return TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
		}

		const TreeNode<T>& node = GetNodeXXX(nodeAccessor);
		return node._parentNodeAccessor;
	}

	template<typename T>
	inline uint32 Tree<T>::GetChildNodeCount(const TreeNodeAccessor<T>& nodeAccessor) const noexcept
	{
		if (IsValidNode(nodeAccessor) == false)
		{
			return 0;
		}

		const TreeNode<T>& node = GetNodeXXX(nodeAccessor);
		return static_cast<uint32>(node._childNodeAccessorArray.Size());
	}

	template<typename T>
	inline TreeNodeAccessor<T> Tree<T>::GetChildNode(const TreeNodeAccessor<T>& nodeAccessor, const uint32 childNodeIndex) const noexcept
	{
		if (IsValidNode(nodeAccessor) == false)
		{
			return TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
		}

		const TreeNode<T>& node = GetNodeXXX(nodeAccessor);
		return node._childNodeAccessorArray[childNodeIndex];
	}

	template<typename T>
	inline TreeNodeAccessor<T> Tree<T>::GetNextSiblingNode(const TreeNodeAccessor<T>& nodeAccessor) const noexcept
	{
		if (IsValidNode(nodeAccessor) == false)
		{
			return TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
		}

		const TreeNode<T>& node = GetNodeXXX(nodeAccessor);
		const TreeNode<T>& parentNode = GetNodeXXX(nodeAccessor.GetParentNode());

		uint32 thisAt = 0;
		const uint32 parentChildCount = parentNode._childNodeAccessorArray.size();
		for (uint32 parentChildIndex = 0; parentChildIndex < parentChildCount; ++parentChildIndex)
		{
			if (parentNode._childNodeAccessorArray[parentChildIndex]._nodeID == nodeAccessor._nodeID)
			{
				thisAt = parentChildIndex;
				break;
			}
		}

		if (thisAt + 1 == parentChildCount)
		{
			return TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
		}
		return parentNode._childNodeAccessorArray[thisAt + 1];
	}

	template<typename T>
	bool Tree<T>::IsValidNode(const TreeNodeAccessor<T>& nodeAccessor) const noexcept
	{
		const TreeNode<T>& node = _nodeArray.At(nodeAccessor._slotIndex);
		return node._nodeID == nodeAccessor._nodeID;
	}

	template<typename T>
	inline void Tree<T>::SetNodeData(const TreeNodeAccessor<T>& nodeAccessor, const T& nodeData) noexcept
	{
		if (IsValidNode(nodeAccessor) == true)
		{
			TreeNode<T>& node = GetNodeXXX(nodeAccessor);
			node._data = nodeData;
		}
	}

	template<typename T>
	inline const T& Tree<T>::GetNodeData(const TreeNodeAccessor<T>& nodeAccessor) const noexcept
	{
		if (IsValidNode(nodeAccessor) == true)
		{
			return GetNodeXXX(nodeAccessor)._data;
		}

		return TreeNodeAccessor<T>::kInvalidData;
	}

	template<typename T>
	inline T& Tree<T>::GetNodeDataXXX(const TreeNodeAccessor<T>& nodeAccessor) noexcept
	{
		if (IsValidNode(nodeAccessor) == true)
		{
			return GetNodeXXX(nodeAccessor)._data;
		}

		return TreeNodeAccessor<T>::nkInvalidData;
	}

	template<typename T>
	void Tree<T>::SwapNodeData(const TreeNodeAccessor<T>& nodeAccessorA, const TreeNodeAccessor<T>& nodeAccessorB) noexcept
	{
		if (IsValidNode(nodeAccessorA) == true && IsValidNode(nodeAccessorB) == true)
		{
			TreeNode<T>& nodeA = GetNodeXXX(nodeAccessorA);
			TreeNode<T>& nodeB = GetNodeXXX(nodeAccessorB);

			std::swap(nodeA._data, nodeB._data);
		}
	}

	template<typename T>
	const TreeNode<T>& Tree<T>::GetNodeXXX(const TreeNodeAccessor<T>& nodeAccessor) const
	{
		const TreeNode<T>& node = _nodeArray[nodeAccessor._slotIndex];
		return node;
	}

	template<typename T>
	TreeNode<T>& Tree<T>::GetNodeXXX(const TreeNodeAccessor<T>& nodeAccessor)
	{
		TreeNode<T>& node = _nodeArray[nodeAccessor._slotIndex];
		return node;
	}

	template<typename T>
	inline TreeNodeAccessor<T> Tree<T>::InsertChildNode(const TreeNodeAccessor<T>& nodeAccessor, const T& childNodeData)
	{
		if (IsValidNode(nodeAccessor) == false)
		{
			return TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
		}

		const uint32 slotIndex = GetAvailableNodeSlot();
		const uint32 nodeID = _nextNodeID;

		const TreeNode<T> childNode{ nodeID, nodeAccessor, childNodeData };
		TreeNodeAccessor<T> childNodeAccessor{ this, nodeID, slotIndex };
		_nodeArray[slotIndex] = childNode;

		TreeNode<T>& node = GetNodeXXX(nodeAccessor);
		node._childNodeAccessorArray.PushBack(childNodeAccessor);

		++_nextNodeID;
		++_nodeCount;

		return childNodeAccessor;
	}

	template<typename T>
	void Tree<T>::EraseChildNode(TreeNodeAccessor<T>& nodeAccessor, TreeNodeAccessor<T>& childNodeAccessor)
	{
		if (IsValidNode(nodeAccessor) == true)
		{
			TreeNode<T>& node = GetNodeXXX(nodeAccessor);

			uint32 childAt = kUint32Max;
			const uint32 childCount = node._childNodeAccessorArray.size();
			for (uint32 childIndex = 0; childIndex < childCount; ++childIndex)
			{
				const TreeNodeAccessor<T>& currentChildNodeAccessor = node._childNodeAccessorArray[childIndex];
				if (currentChildNodeAccessor == childNodeAccessor)
				{
					childAt = childIndex;
					break;
				}
			}

			if (childAt == kUint32Max)
			{
				MINT_ASSERT(false, "자식이 아닌 노드를 지우려 합니다...!!!");
				return;
			}

			{
				TreeNode<T>& childNode = GetNodeXXX(childNodeAccessor);
				const uint32 grandChildCount = childNode._childNodeAccessorArray.size();
				for (uint32 grandChildIndex = 0; grandChildIndex < grandChildCount; ++grandChildIndex)
				{
					TreeNodeAccessor<T>& grandChildNodeAccessor = childNode._childNodeAccessorArray[grandChildIndex];
					EraseChildNode(childNodeAccessor, grandChildNodeAccessor);
				}
			}

			// childNode 를 _nodeArray 에서 제거
			TreeNode<T>& childNode = GetNodeXXX(childNodeAccessor);
			childNode.Invalidate();

			// childNodeAccessor 를 node 의 _childNodeAccessorArray 에서 제거
			node._childNodeAccessorArray.erase(node._childNodeAccessorArray.begin() + childAt);

			--_nodeCount;
		}
	}

	template<typename T>
	inline void Tree<T>::ClearChildNodes(TreeNodeAccessor<T>& nodeAccessor)
	{
		if (IsValidNode(nodeAccessor) == true)
		{
			TreeNode<T>& node = GetNodeXXX(nodeAccessor);

			const uint32 childCount = static_cast<uint32>(node._childNodeAccessorArray.Size());
			if (childCount > 0)
			{
				for (uint32 childIndex = childCount - 1; childIndex != kUint32Max; --childIndex)
				{
					TreeNodeAccessor<T>& childNodeAccessor = node._childNodeAccessorArray[childIndex];

					ClearChildNodes(childNodeAccessor);

					// childNode 를 _nodeArray 에서 제거
					TreeNode<T>& childNode = GetNodeXXX(childNodeAccessor);
					childNode.Invalidate();

					// childNodeAccessor 를 node 의 _childNodeAccessorArray 에서 제거
					node._childNodeAccessorArray.Erase(childIndex);

					--_nodeCount;
				}
			}
		}
	}

	template<typename T>
	inline void Tree<T>::MoveToParent(TreeNodeAccessor<T>& nodeAccessor, const TreeNodeAccessor<T>& newParentNodeAccessor)
	{
		if (IsValidNode(nodeAccessor) == false || IsValidNode(newParentNodeAccessor) == false)
		{
			return;
		}

		if (GetRootNode() == nodeAccessor)
		{
			MINT_ASSERT(false, "루트 노트를 옮길 수 없습니다!!!");
			return;
		}

		const TreeNodeAccessor<T>& oldParentNodeAccessor = _nodeArray[nodeAccessor._slotIndex]._parentNodeAccessor;
		TreeNode<T>& oldParnetNode = _nodeArray[oldParentNodeAccessor._slotIndex];
		{
			uint32 childAt = kUint32Max;
			const uint32 oldParentNodeChildCount = static_cast<uint32>(oldParnetNode._childNodeAccessorArray.Size());
			for (uint32 childIndex = 0; childIndex < oldParentNodeChildCount; ++childIndex)
			{
				const TreeNodeAccessor<T>& childNodeAccessor = oldParnetNode._childNodeAccessorArray[childIndex];
				if (childNodeAccessor == nodeAccessor)
				{
					childAt = childIndex;
					break;
				}
			}

			MINT_ASSERT(childAt != kUint32Max, "발생하면 안 되는 상황!!!");

			oldParnetNode._childNodeAccessorArray.Erase(childAt);
		}

		TreeNode<T>& node = _nodeArray[nodeAccessor._slotIndex];
		node._parentNodeAccessor = newParentNodeAccessor;

		TreeNode<T>& newParnetNode = _nodeArray[newParentNodeAccessor._slotIndex];
		newParnetNode._childNodeAccessorArray.PushBack(nodeAccessor);
	}

	template<typename T>
	inline uint32 Tree<T>::GetAvailableNodeSlot()
	{
		while (true)
		{
			const uint32 slotCount = static_cast<uint32>(_nodeArray.Size());
			for (uint32 slotIndex = 0; slotIndex < slotCount; ++slotIndex)
			{
				TreeNode<T>& node = _nodeArray[slotIndex];
				if (node.IsValid() == false)
				{
					return slotIndex;
				}
			}

			_nodeArray.Resize(_nodeArray.Size() * 2);
		}
	}

}
