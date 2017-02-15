#include "node.h"

Node::ptr Node::firstChild() {
  return childNodes.front();
}

Node::ptr Node::lastChild() {
  return childNodes.back();
}

void Node::setAttribute(std::string name, std::string value) {
  attributes[name] = value;
}

std::string Node::getAttribute(std::string name) {
  return attributes[name];
}

void Node::removeAttribute(std::string name) {
  attributes.erase(name);
}

void Node::appendChild(ptr child) {
  childNodes.push_back(child);
  child->parentNode = this; // std::make_shared<Node>(this);
}

void Node::removeChild(ptr child) {
  childNodes.remove(child);
  child->parentNode = NULL;
}

void Node::insertBefore(ptr newChild, ptr refChild) {
  // fixme: not as per dom-level-1, will need to fix if we 
  // need to respect child order
  appendChild(newChild);
}

Node::ptr Node::replaceChild(ptr newChild, ptr oldChild) {
  insertBefore(newChild, oldChild);
  childNodes.remove(oldChild);
  return oldChild;
}

bool Node::hasChildNodes () {
  return !childNodes.empty();
}

Node::ptr Node::cloneNode (bool deep) {
  ptr clone(new Node(nodeName));

  for (auto const pair : attributes) {
    clone->setAttribute(pair.first, pair.second);
  }

  if (deep) {
    throw "todo";
  }

  return clone;
}

std::string Node::toString () {
  std::stringstream ss;

  ss << "<" << nodeName;

  for (auto const pair : attributes) {
      ss << " " << pair.first << "=\"" << pair.second << "\"";
  }

  // if (hasChildNodes()) {
    ss << ">";

    for (auto const n : childNodes) {
        ss << n->toString();
    }

    ss << "</" << nodeName << ">";
  // } else {
  //   ss << " />";
  // }

  return ss.str();
}

Node::Node (std::string name) {
  nodeName = name;
  nodeValue = 1;
}

Node::~Node () {
  std::cout << "<" << nodeName << "/> destroyed\n";
}
