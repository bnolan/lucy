#pragma once

#include <string>
#include <memory>
#include <list>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <map>

class Node: public std::enable_shared_from_this<Node> {

public:
  typedef std::shared_ptr<Node> ptr;
  typedef Node* weakptr;
  std::string nodeName;
  std::string innerText;
  int nodeValue; 
  std::list<ptr> childNodes;
  std::map<std::string, std::string> attributes;
  weakptr parentNode;

  Node (std::string name);

  ~Node ();

  ptr firstChild();

  ptr lastChild();

  void setAttribute(std::string name, std::string value);

  std::string getAttribute(std::string name);

  void removeAttribute(std::string name);
  void appendChild(ptr child);
  void removeChild(ptr child);
  ptr replaceChild(ptr newChild, ptr oldChild);
  void insertBefore(ptr newChild, ptr refChild);

  std::list<ptr> querySelectorAll(std::string selector);

  bool hasChildNodes ();

  ptr cloneNode (bool deep);

  std::string toString ();

  ptr self();

};
