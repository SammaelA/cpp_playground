#pragma once

#include <memory>

namespace NBinTree {

template <typename T>
class TNode : public std::enable_shared_from_this<TNode<T>>{
public:
    using TNodePtr = std::shared_ptr<TNode<T>>;
    using TNodeWPtr = std::weak_ptr<TNode<T>>;
    using TNodeConstPtr = std::shared_ptr<const TNode<T>>;
    using TNodeConstWPtr = std::weak_ptr<const TNode<T>>;

    bool HasLeft() const {
        return Left;
    }

    bool HasRight() const {
        return Right;
    }

    bool HasParent() const {
        return Parent;
    }

    T& GetValue() {
        return Value;
    }

    const T& GetValue() const {
        return Value;
    }

    TNodePtr GetLeft() {
        return Left;
    }

    TNodeConstPtr GetLeft() const {
        return Left;
    }

    TNodePtr GetRight() {
        return Right;
    }

    TNodeConstPtr GetRight() const {
        return Right;
    }

    TNodePtr GetParent() {
        return Parent.lock();
    }

    TNodeConstPtr GetParent() const {
        return Parent.lock();
    }

    static TNodePtr CreateLeaf(T value) {
        return std::shared_ptr<TNode>(new TNode(value));
    }

    static TNodePtr Fork(T value, TNodePtr left, TNodePtr right) {
        TNodePtr ptr = std::shared_ptr<TNode>(new TNode(value, left, right));
        SetParent(ptr->GetLeft(), ptr);
        SetParent(ptr->GetRight(), ptr);
        return ptr;
    }

    TNodePtr ReplaceLeft(TNodePtr left) {
        SetParent(left, this->shared_from_this());
        SetParent(Left, nullptr);
        std::swap(left, Left);
        return left;
    }

    TNodePtr ReplaceRight(TNodePtr right) {
        SetParent(right, this->shared_from_this());
        SetParent(Right, nullptr);
        std::swap(right, Right);
        return right;
    }

    TNodePtr ReplaceRightWithLeaf(T value) {
        return ReplaceRight(CreateLeaf(value));
    }

    TNodePtr ReplaceLeftWithLeaf(T value) {
        return ReplaceLeft(CreateLeaf(value));
    }

    TNodePtr RemoveLeft() {
        return ReplaceLeft(nullptr);
    }

    TNodePtr RemoveRight() {
        return ReplaceRight(nullptr);
    }

private:
    T Value;
    TNodePtr Left = nullptr;
    TNodePtr Right = nullptr;
    TNodeWPtr Parent;

    TNode(T value)
        : Value(value)
    {
    }

    TNode(T value, TNodePtr left, TNodePtr right)
        : Value(value)
        , Left(left)
        , Right(right)
    {
    }

    static void SetParent(TNodePtr node, TNodePtr parent) {
        if (node) {
            node->Parent = parent;
        }
    }
};

} // namespace NBinTree
