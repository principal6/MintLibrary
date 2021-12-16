#pragma once


#include <stdafx.h>
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
        , _nodeId{ kInvalidNodeId }
    {
        __noop;
    }

    template<typename T>
    TreeNodeAccessor<T>::TreeNodeAccessor(Tree<T>* const tree, const uint32 slotIndex, const uint32 nodeId)
        : _tree{ tree }
        , _slotIndex{ slotIndex }
        , _nodeId{ nodeId }
    {
        __noop;
    }

    template<typename T>
    const bool TreeNodeAccessor<T>::operator==(const TreeNodeAccessor<T>& rhs) const noexcept
    {
        return (_nodeId == rhs._nodeId) && (_slotIndex == rhs._slotIndex);
    }

    template<typename T>
    const bool TreeNodeAccessor<T>::isValid() const noexcept
    {
        if (_tree != nullptr)
        {
            return _tree->isValidNode(*this);
        }
        return false;
    }

    template<typename T>
    void TreeNodeAccessor<T>::setNodeData(const T& data) noexcept
    {
        if (_tree != nullptr)
        {
            _tree->setNodeData(*this, data);
        }
    }

    template<typename T>
    const T& TreeNodeAccessor<T>::getNodeData() const noexcept
    {
        if (_tree != nullptr)
        {
            return _tree->getNodeData(*this);
        }
        return kInvalidData;
    }

    template<typename T>
    inline T& TreeNodeAccessor<T>::getNodeDataXXX() noexcept
    {
        if (_tree != nullptr)
        {
            return _tree->getNodeDataXXX(*this);
        }
        return nkInvalidData;
    }

    template<typename T>
    inline TreeNodeAccessor<T> TreeNodeAccessor<T>::getParentNode() const noexcept
    {
        if (_tree != nullptr)
        {
            return _tree->getParentNode(*this);
        }
        return TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
    }

    template<typename T>
    inline const uint32 TreeNodeAccessor<T>::getChildNodeCount() const noexcept
    {
        if (_tree != nullptr)
        {
            return _tree->getChildNodeCount(*this);
        }
        return 0;
    }

    template<typename T>
    inline TreeNodeAccessor<T> TreeNodeAccessor<T>::getChildNode(const uint32 childNodeIndex) const noexcept
    {
        if (_tree != nullptr)
        {
            return _tree->getChildNode(*this, childNodeIndex);
        }
        return TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
    }

    template<typename T>
    inline TreeNodeAccessor<T> TreeNodeAccessor<T>::getNextSiblingNode() const noexcept
    {
        if (_tree != nullptr)
        {
            return _tree->getNextSiblingNode(*this);
        }
        return TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
    }

    template<typename T>
    TreeNodeAccessor<T> TreeNodeAccessor<T>::insertChildNode(const T& data)
    {
        if (isValid() == true)
        {
            return _tree->insertChildNode(*this, data);
        }
        return TreeNodeAccessor::kInvalidTreeNodeAccessor;
    }

    template<typename T>
    inline void TreeNodeAccessor<T>::eraseChildNode(TreeNodeAccessor<T>& childNodeAccessor)
    {
        if (_tree != nullptr)
        {
            return _tree->eraseChildNode(*this, childNodeAccessor);
        }
    }

    template<typename T>
    inline void TreeNodeAccessor<T>::clearChildNodes()
    {
        if (_tree != nullptr)
        {
            return _tree->clearChildNodes(*this);
        }
    }

    template<typename T>
    void TreeNodeAccessor<T>::moveToParent(const TreeNodeAccessor<T>& newParentNodeAccessor)
    {
        if (isValid() == true)
        {
            _tree->moveToParent(*this, newParentNodeAccessor);
        }
    }


    template<typename T>
    inline const bool TreeNode<T>::isValid() const noexcept
    {
        return (_nodeId != TreeNodeAccessor<T>::kInvalidNodeId);
    }

    template<typename T>
    inline void TreeNode<T>::invalidate() noexcept
    {
        _nodeId = TreeNodeAccessor<T>::kInvalidNodeId;
        _data = T(); // T 가 동적 할당된 메모리를 들고 있다면 이게 더 비용이 쌀 가능성이 높으므로...
        _parentNodeAccessor = TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
    }


    template<typename T>
    inline Tree<T>::Tree()
        : _nextNodeId{ 0 }
        , _nodeCount{ 0 }
    {
        _nodeArray.resize(kDefaultNodeArraySize);
    }

    template<typename T>
    inline Tree<T>::~Tree()
    {
        __noop;
    }

    template<typename T>
    inline TreeNodeAccessor<T> Tree<T>::createRootNode(const T& rootNodeData)
    {
        if (_nodeCount == 0)
        {
            TreeNode rootNode{ _nextNodeId, TreeNodeAccessor<T>::kInvalidTreeNodeAccessor, rootNodeData };
            
            ++_nextNodeId;
            ++_nodeCount;

            _nodeArray[0] = rootNode;
        }

        return getRootNode();
    }

    template<typename T>
    inline void Tree<T>::destroyRootNode()
    {
        if (_nodeCount > 0)
        {
            TreeNodeAccessor<T> rootNodeAccessor = getRootNode();
            clearChildNodes(rootNodeAccessor);

            TreeNode<T>& rootNode = getNodeXXX(rootNodeAccessor);
            rootNode.invalidate();

            _nodeCount = 0;
        }
    }

    template<typename T>
    inline TreeNodeAccessor<T> Tree<T>::getRootNode() noexcept
    {
        const TreeNode<T>& rootNode = _nodeArray.front();
        return TreeNodeAccessor<T>::TreeNodeAccessor(this, static_cast<uint32>(0), rootNode._nodeId); // this == Tree<T>* 가 const 이면 안 되므로 이 함수는 const 함수일 수 없다!!!
    }

    template<typename T>
    TreeNodeAccessor<T> Tree<T>::findNode(const TreeNodeAccessor<T>& startNodeAccessor, const T& nodeData) const noexcept
    {
        if (getNodeData(startNodeAccessor) == nodeData)
        {
            return startNodeAccessor;
        }

        const TreeNode<T>& startNode = getNodeXXX(startNodeAccessor);
        const uint32 childCount = static_cast<uint32>(startNode._childNodeAccessorArray.size());
        for (uint32 childIndex = 0; childIndex < childCount; ++childIndex)
        {
            TreeNodeAccessor<T> foundNodeAccessor = findNode(startNode._childNodeAccessorArray[childIndex], nodeData);
            if (getNodeData(foundNodeAccessor) == nodeData)
            {
                return foundNodeAccessor;
            }
        }

        return TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
    }

    template<typename T>
    inline TreeNodeAccessor<T> Tree<T>::getParentNode(const TreeNodeAccessor<T>& nodeAccessor) const noexcept
    {
        if (isValidNode(nodeAccessor) == false)
        {
            return TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
        }

        const TreeNode<T>& node = getNodeXXX(nodeAccessor);
        return node._parentNodeAccessor;
    }

    template<typename T>
    inline const uint32 Tree<T>::getChildNodeCount(const TreeNodeAccessor<T>& nodeAccessor) const noexcept
    {
        if (isValidNode(nodeAccessor) == false)
        {
            return 0;
        }

        const TreeNode<T>& node = getNodeXXX(nodeAccessor);
        return static_cast<uint32>(node._childNodeAccessorArray.size());
    }

    template<typename T>
    inline TreeNodeAccessor<T> Tree<T>::getChildNode(const TreeNodeAccessor<T>& nodeAccessor, const uint32 childNodeIndex) const noexcept
    {
        if (isValidNode(nodeAccessor) == false)
        {
            return TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
        }

        const TreeNode<T>& node = getNodeXXX(nodeAccessor);
        return node._childNodeAccessorArray[childNodeIndex];
    }

    template<typename T>
    inline TreeNodeAccessor<T> Tree<T>::getNextSiblingNode(const TreeNodeAccessor<T>& nodeAccessor) const noexcept
    {
        if (isValidNode(nodeAccessor) == false)
        {
            return TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
        }

        const TreeNode<T>& node = getNodeXXX(nodeAccessor);
        const TreeNode<T>& parentNode = getNodeXXX(nodeAccessor.getParentNode());
        
        uint32 thisAt = 0;
        const uint32 parentChildCount = parentNode._childNodeAccessorArray.size();
        for (uint32 parentChildIndex = 0; parentChildIndex < parentChildCount; ++parentChildIndex)
        {
            if (parentNode._childNodeAccessorArray[parentChildIndex]._nodeId == nodeAccessor._nodeId) 
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
    const bool Tree<T>::isValidNode(const TreeNodeAccessor<T>& nodeAccessor) const noexcept
    {
        const TreeNode<T>& node = _nodeArray.at(nodeAccessor._slotIndex);
        return node._nodeId == nodeAccessor._nodeId;
    }

    template<typename T>
    inline void Tree<T>::setNodeData(const TreeNodeAccessor<T>& nodeAccessor, const T& nodeData) noexcept
    {
        if (isValidNode(nodeAccessor) == true)
        {
            TreeNode<T>& node = getNodeXXX(nodeAccessor);
            node._data = nodeData;
        }
    }

    template<typename T>
    inline const T& Tree<T>::getNodeData(const TreeNodeAccessor<T>& nodeAccessor) const noexcept
    {
        if (isValidNode(nodeAccessor) == true)
        {
            return getNodeXXX(nodeAccessor)._data;
        }

        return TreeNodeAccessor<T>::kInvalidData;
    }

    template<typename T>
    inline T& Tree<T>::getNodeDataXXX(const TreeNodeAccessor<T>& nodeAccessor) noexcept
    {
        if (isValidNode(nodeAccessor) == true)
        {
            return getNodeXXX(nodeAccessor)._data;
        }

        return TreeNodeAccessor<T>::nkInvalidData;
    }

    template<typename T>
    void Tree<T>::swapNodeData(const TreeNodeAccessor<T>& nodeAccessorA, const TreeNodeAccessor<T>& nodeAccessorB) noexcept
    {
        if (isValidNode(nodeAccessorA) == true && isValidNode(nodeAccessorB) == true)
        {
            TreeNode<T>& nodeA = getNodeXXX(nodeAccessorA);
            TreeNode<T>& nodeB = getNodeXXX(nodeAccessorB);

            std::swap(nodeA._data, nodeB._data);
        }
    }

    template<typename T>
    const TreeNode<T>& Tree<T>::getNodeXXX(const TreeNodeAccessor<T>& nodeAccessor) const
    {
        const TreeNode<T>& node = _nodeArray[nodeAccessor._slotIndex];
        return node;
    }

    template<typename T>
    TreeNode<T>& Tree<T>::getNodeXXX(const TreeNodeAccessor<T>& nodeAccessor)
    {
        TreeNode<T>& node = _nodeArray[nodeAccessor._slotIndex];
        return node;
    }

    template<typename T>
    inline TreeNodeAccessor<T> Tree<T>::insertChildNode(const TreeNodeAccessor<T>& nodeAccessor, const T& childNodeData)
    {
        if (isValidNode(nodeAccessor) == false)
        {
            return TreeNodeAccessor<T>::kInvalidTreeNodeAccessor;
        }

        const uint32 slotIndex = getAvailableNodeSlot();
        const uint32 nodeId = _nextNodeId;

        const TreeNode<T> childNode{ nodeId, nodeAccessor, childNodeData };
        TreeNodeAccessor<T> childNodeAccessor{ this, nodeId, slotIndex };
        _nodeArray[slotIndex] = childNode;

        TreeNode<T>& node = getNodeXXX(nodeAccessor);
        node._childNodeAccessorArray.push_back(childNodeAccessor);

        ++_nextNodeId;
        ++_nodeCount;

        return childNodeAccessor;
    }

    template<typename T>
    void Tree<T>::eraseChildNode(TreeNodeAccessor<T>& nodeAccessor, TreeNodeAccessor<T>& childNodeAccessor)
    {
        if (isValidNode(nodeAccessor) == true)
        {
            TreeNode<T>& node = getNodeXXX(nodeAccessor);
            
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
                MINT_ASSERT("김장원", false, "자식이 아닌 노드를 지우려 합니다...!!!");
                return;
            }

            {
                TreeNode<T>& childNode = getNodeXXX(childNodeAccessor);
                const uint32 grandChildCount = childNode._childNodeAccessorArray.size();
                for (uint32 grandChildIndex = 0; grandChildIndex < grandChildCount; ++grandChildIndex)
                {
                    TreeNodeAccessor<T>& grandChildNodeAccessor = childNode._childNodeAccessorArray[grandChildIndex];
                    eraseChildNode(childNodeAccessor, grandChildNodeAccessor);
                }
            }

            // childNode 를 _nodeArray 에서 제거
            TreeNode<T>& childNode = getNodeXXX(childNodeAccessor);
            childNode.invalidate();

            // childNodeAccessor 를 node 의 _childNodeAccessorArray 에서 제거
            node._childNodeAccessorArray.erase(node._childNodeAccessorArray.begin() + childAt);

            --_nodeCount;
        }
    }

    template<typename T>
    inline void Tree<T>::clearChildNodes(TreeNodeAccessor<T>& nodeAccessor)
    {
        if (isValidNode(nodeAccessor) == true)
        {
            TreeNode<T>& node = getNodeXXX(nodeAccessor);

            const uint32 childCount = static_cast<uint32>(node._childNodeAccessorArray.size());
            if (childCount > 0)
            {
                for (uint32 childIndex = childCount - 1; childIndex != kUint32Max; --childIndex)
                {
                    TreeNodeAccessor<T>& childNodeAccessor = node._childNodeAccessorArray[childIndex];

                    clearChildNodes(childNodeAccessor);

                    // childNode 를 _nodeArray 에서 제거
                    TreeNode<T>& childNode = getNodeXXX(childNodeAccessor);
                    childNode.invalidate();

                    // childNodeAccessor 를 node 의 _childNodeAccessorArray 에서 제거
                    node._childNodeAccessorArray.erase(childIndex);

                    --_nodeCount;
                }
            }
        }
    }

    template<typename T>
    inline void Tree<T>::moveToParent(TreeNodeAccessor<T>& nodeAccessor, const TreeNodeAccessor<T>& newParentNodeAccessor)
    {
        if (isValidNode(nodeAccessor) == false || isValidNode(newParentNodeAccessor) == false)
        {
            return;
        }

        if (getRootNode() == nodeAccessor)
        {
            MINT_ASSERT("김장원", false, "루트 노트를 옮길 수 없습니다!!!");
            return;
        }

        const TreeNodeAccessor<T>& oldParentNodeAccessor = _nodeArray[nodeAccessor._slotIndex]._parentNodeAccessor;
        TreeNode<T>& oldParnetNode = _nodeArray[oldParentNodeAccessor._slotIndex];
        {
            uint32 childAt = kUint32Max;
            const uint32 oldParentNodeChildCount = static_cast<uint32>(oldParnetNode._childNodeAccessorArray.size());
            for (uint32 childIndex = 0; childIndex < oldParentNodeChildCount; ++childIndex)
            {
                const TreeNodeAccessor<T>& childNodeAccessor = oldParnetNode._childNodeAccessorArray[childIndex];
                if (childNodeAccessor == nodeAccessor)
                {
                    childAt = childIndex;
                    break;
                }
            }

            MINT_ASSERT("김장원", childAt != kUint32Max, "발생하면 안 되는 상황!!!");

            oldParnetNode._childNodeAccessorArray.erase(childAt);
        }

        TreeNode<T>& node = _nodeArray[nodeAccessor._slotIndex];
        node._parentNodeAccessor = newParentNodeAccessor;
        
        TreeNode<T>& newParnetNode = _nodeArray[newParentNodeAccessor._slotIndex];
        newParnetNode._childNodeAccessorArray.push_back(nodeAccessor);
    }

    template<typename T>
    inline const uint32 Tree<T>::getAvailableNodeSlot()
    {
        while (true)
        {
            const uint32 slotCount = static_cast<uint32>(_nodeArray.size());
            for (uint32 slotIndex = 0; slotIndex < slotCount; ++slotIndex)
            {
                TreeNode<T>& node = _nodeArray[slotIndex];
                if (node.isValid() == false)
                {
                    return slotIndex;
                }
            }

            _nodeArray.resize(_nodeArray.size() * 2);
        }
    }

}
