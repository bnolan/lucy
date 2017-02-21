#include "node.h"

bool nodeMatchesSelector(Node::ptr node, std::string selector) {
  // fixme - make work properly or raise on invalid selector
  return node->nodeName == selector;
}

void internalQuerySelectorAll(Node::ptr node, std::string selector, std::list<Node::ptr> *results) {
  for (auto const n : node->childNodes) {
    if (nodeMatchesSelector(n, selector)) {
      // std::cout << "found " << n->nodeName << "\n";
      results->push_back(n);
    }

    internalQuerySelectorAll(n, selector, results);
  }
}

// fixme - only gets child nodes
std::list<Node::ptr> Node::querySelectorAll(std::string selector) {
  std::list<Node::ptr> results;

  internalQuerySelectorAll(self(), selector, &results);

  return results;
}

Node::ptr Node::self() {
  return std::make_shared<Node>(*this);
}

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
  // std::cout << "<" << nodeName << "/> destroyed\n";
}
