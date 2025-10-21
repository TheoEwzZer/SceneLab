#pragma once

#include "GameObject.hpp"
#include <memory>
#include <vector>
#include <functional>

class SceneGraph {
  public:
    class Node {
      private:
        std::vector<std::unique_ptr<Node>> children;
        GameObject data;
        Node *parent = nullptr;

      public:
        Node() = default;
        ~Node() = default;

        Node(const Node &) = delete;
        Node &operator=(const Node &) = delete;

        void addChild(std::unique_ptr<Node> child)
        {
            child->parent = this;
            children.push_back(std::move(child));
        }

        Node *getParent()
        {
            return parent;
        }

        GameObject &getData()
        {
            return data;
        }

        void setData(const GameObject &newData)
        {
            data = newData;
        }

        Node *getChild(int index)
        {
            if (index < 0 || index >= static_cast<int>(children.size())) {
                return nullptr;
            }
            return children[index].get();
        }

        void traverse(std::function<void(GameObject &, int)> func, int depth = 0)
        {
            func(data, depth);
            for (auto &child : children) {
                child->traverse(func, depth + 1);
            }
        }

        void traverse(std::function<void(Node &, int)> func, int depth = 0)
        {
            func(*this, depth);
            for (auto &child : children) {
                child->traverse(func, depth + 1);
            }
        }

        void traverseWithTransform(
            std::function<void(GameObject &, const glm::mat4 &, int)> func,
            const glm::mat4 &parentTransform = glm::mat4(1.0f),
            int depth = 0)
        {
            const glm::mat4 &worldTransform = data.getWorldMatrix(parentTransform);
            func(data, worldTransform, depth);
            for (auto &child : children) {
                child->traverseWithTransform(func, worldTransform, depth + 1);
            }
        }

        glm::mat4 getParentWorldMatrix() const
        {
            if (parent == nullptr) {
                return glm::mat4(1.0f);
            }
            return parent->getData().getWorldMatrix(parent->getParentWorldMatrix());
        }

        glm::mat4 getWorldMatrix() const
        {
            return data.getWorldMatrix(getParentWorldMatrix());
        }
    };


  private:
    std::unique_ptr<Node> root;

  public:
    SceneGraph() = default;
    ~SceneGraph() = default;

    SceneGraph(const SceneGraph &) = delete;
    SceneGraph &operator=(const SceneGraph &) = delete;

    Node *getRoot()
    {
        return root.get();
    }

    void setRoot(std::unique_ptr<Node> newRoot)
    {
        root = std::move(newRoot);
    }


    void traverse(std::function<void(GameObject &, int)> func)
    {
        if (root) {
            root->traverse(func);
        }
    }

    void traverse(std::function<void(Node &, int)> func)
    {
        if (root) {
            root->traverse(func);
        }
    }

    void traverseWithTransform(
        std::function<void(GameObject &, const glm::mat4 &, int)> func,
        const glm::mat4 &parentTransform = glm::mat4(1.0f))
    {
        if (root) {
            root->traverseWithTransform(func, parentTransform);
        }
    }
};