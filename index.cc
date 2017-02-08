#include <string>
#include <memory>
#include <list>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <map>

class Node {

public:
  typedef std::shared_ptr<Node> ptr;

  std::string nodeName;

  int nodeValue; 

  std::list<ptr> childNodes;
  std::map<std::string, std::string> attributes;

  ptr parentNode;

  ptr firstChild() {
    return childNodes.front();
  }

  ptr lastChild() {
    return childNodes.back();
  }

  void setAttribute(std::string name, std::string value) {
    attributes[name] = value;
  }

  std::string getAttribute(std::string name) {
    return attributes[name];
  }

  void removeAttribute(std::string name) {
    attributes.erase(name);
  }

  void appendChild(ptr child) {
    childNodes.push_back(child);
  }

  void removeChild(ptr child) {
    throw "todo";
  }

  bool hasChildNodes () {
    return !childNodes.empty();
  }

  ptr cloneNode (bool deep) {
    ptr clone(new Node(nodeName));

    for (auto const pair : attributes) {
      clone->setAttribute(pair.first, pair.second);
    }

    if (deep) {
      throw "todo";
    }

    return clone;
  }

  std::string toString () {
    std::stringstream ss;

    ss << "<" << nodeName;

    for (auto const pair : attributes) {
        ss << " " << pair.first << "=\"" << pair.second << "\"";
    }

    if (hasChildNodes()) {
      ss << ">";

      for (auto const n : childNodes) {
          ss << n->toString();
      }

      ss << "</" << nodeName << ">";
    } else {
      ss << " />";
    }

    return ss.str();
  }

  Node (std::string name) {
    nodeName = name;
    // sharedPtr = std::make_shared<Node>(this);
  }

  ~Node () {
    std::cout << "<" << nodeName << "/> destroyed\n";
  }
};

// class Document : public Node {
// public:

//   Node::ptr documentElement;

//   Document () : Node("") {
//     // documentElement = new Node("scene")->sharedPointer;
//   }

//   Node::ptr createElement (std::string name) {
//     Node::ptr child(new Node(name));
//     child->ownerDocument = this->sharedPtr;
//     return child;
//   }

//   std::string toString () {
//     return documentElement->toString();
//   }  
// };

// using nodePtr = std::shared_ptr<Node>;

int main () {
  Node::ptr world(new Node("world"));
  // scene->nodeName = "scene";

  Node::ptr a(new Node("box"));
  a->setAttribute("position", "1 2 3");
  world->appendChild(a);

  Node::ptr b(new Node("box"));
  b->setAttribute("position", "3 2 3");
  world->appendChild(b);

  Node::ptr c(new Node("box"));
  c->setAttribute("position", "6 2 3");
  world->appendChild(c);

  std::cout << "Scene state:\n\n" << world->toString() << "\n\n";
}