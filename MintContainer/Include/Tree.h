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

        static constexpr uint32         kInvalidNodeID = kUint32Max;

    public:
                                        TreeNodeAccessor();
                                        TreeNodeAccessor(const TreeNodeAccessor& rhs) = default;
                                        TreeNodeAccessor(TreeNodeAccessor&& rhs) = default;

    private:
                                        TreeNodeAccessor(Tree<T>* const tree, const uint32 slotIndex, const uint32 nodeID);

    public:
        TreeNodeAccessor&               operator=(const TreeNodeAccessor& rhs) = default;
        TreeNodeAccessor&               operator=(TreeNodeAccessor&& rhs) noexcept = default;

    public:
        bool                            operator==(const TreeNodeAccessor& rhs) const noexcept;

    public:
        bool                            isValid() const noexcept;
    
    public:
        void                            setNodeData(const T& data) noexcept;
        const T&                        getNodeData() const noexcept;
        T&                              getNodeDataXXX() noexcept;

    public:
        TreeNodeAccessor<T>             getParentNode() const noexcept;
        uint32                          getChildNodeCount() const noexcept;
        TreeNodeAccessor<T>             getChildNode(const uint32 childNodeIndex) const noexcept;
        TreeNodeAccessor<T>             getNextSiblingNode() const noexcept;

    public:
        TreeNodeAccessor                insertChildNode(const T& data);
        void                            eraseChildNode(TreeNodeAccessor<T>& childNodeAccessor);
        void                            clearChildNodes();

    public:
        void                            moveToParent(const TreeNodeAccessor<T>& newParentNodeAccessor);

    private:
        Tree<T>*                        _tree;

    private:
        uint32                          _slotIndex;
        uint32                          _nodeID;

    public:
        static const TreeNodeAccessor   kInvalidTreeNodeAccessor;
        static const T                  kInvalidData;
        static T                        nkInvalidData;
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
        bool                            isValid() const noexcept;
        void                            invalidate() noexcept;

    private:
        uint32                          _nodeID;
        T                               _data;

        TreeNodeAccessor<T>             _parentNodeAccessor;
        Vector<TreeNodeAccessor<T>>     _childNodeAccessorArray;
    };


    template <typename T>
    class Tree
    {
        static constexpr uint32             kDefaultNodeArraySize = 16;
        static_assert(kDefaultNodeArraySize > 0, "kDefaultNodeArraySize must not be 0");

    public:
                                            Tree();
                                            ~Tree();

    public:
        TreeNodeAccessor<T>                 createRootNode(const T& rootNodeData);
        void                                destroyRootNode();
        TreeNodeAccessor<T>                 getRootNode() noexcept;
    
    public:
        TreeNodeAccessor<T>                 findNode(const TreeNodeAccessor<T>& startNodeAccessor, const T& nodeData) const noexcept;
    
    public:
        TreeNodeAccessor<T>                 getParentNode(const TreeNodeAccessor<T>& nodeAccessor) const noexcept;
        uint32                              getChildNodeCount(const TreeNodeAccessor<T>& nodeAccessor) const noexcept;
        TreeNodeAccessor<T>                 getChildNode(const TreeNodeAccessor<T>& nodeAccessor, const uint32 childNodeIndex) const noexcept;
        TreeNodeAccessor<T>                 getNextSiblingNode(const TreeNodeAccessor<T>& nodeAccessor) const noexcept;

    public:
        bool                                isValidNode(const TreeNodeAccessor<T>& nodeAccessor) const noexcept;
        void                                setNodeData(const TreeNodeAccessor<T>& nodeAccessor, const T& nodeData) noexcept;
        const T&                            getNodeData(const TreeNodeAccessor<T>& nodeAccessor) const noexcept;
        T&                                  getNodeDataXXX(const TreeNodeAccessor<T>& nodeAccessor) noexcept;
        void                                swapNodeData(const TreeNodeAccessor<T>& nodeAccessorA, const TreeNodeAccessor<T>& nodeAccessorB) noexcept;

    private:
        const TreeNode<T>&                  getNodeXXX(const TreeNodeAccessor<T>& nodeAccessor) const;
        TreeNode<T>&                        getNodeXXX(const TreeNodeAccessor<T>& nodeAccessor);

    public:
        TreeNodeAccessor<T> [[nodiscard]]   insertChildNode(const TreeNodeAccessor<T>& nodeAccessor, const T& childNodeData);
        void                                eraseChildNode(TreeNodeAccessor<T>& nodeAccessor, TreeNodeAccessor<T>& childNodeAccessor);
        void                                clearChildNodes(TreeNodeAccessor<T>& nodeAccessor);

    public:
        void                                moveToParent(TreeNodeAccessor<T>& nodeAccessor, const TreeNodeAccessor<T>& newParentNodeAccessor);

    private:
        uint32                              getAvailableNodeSlot();

    private:
        Vector<TreeNode<T>>                 _nodeArray;
        uint32                              _nextNodeID;
        uint32                              _nodeCount;
    };
}


#endif // !_MINT_CONTAINER_TREE_H_
