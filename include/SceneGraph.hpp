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

      public:
        Node() = default;
        ~Node() = default;

        Node(const Node &) = delete;
        Node &operator=(const Node &) = delete;

        void addChild(std::unique_ptr<Node> child)
        {
            children.push_back(std::move(child));
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
};

