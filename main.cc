#include "node.h"

int main () {
  Node::ptr world(new Node("world"));

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