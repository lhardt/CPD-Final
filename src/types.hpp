
#ifndef TYPES_H
#define TYPES_H


#include <iostream>
#include <vector>

// Quick class that times its existence from constructor to destructor.
class Clock {
public:
  Clock(std::string funcname);
  ~Clock();
  clock_t m_clock;
  std::string m_funcname;
};


class Hashable {
public:
  virtual int getHash() = 0;
  virtual bool isBlank() = 0;
};

class Player : public Hashable {
public:
  Player();
  Player(int id, std::string name);
  virtual int getHash();
  virtual bool isBlank();

  bool m_is_blank;
  int m_id;
  std::string m_name;
};

class TagItem {
public:
  int user_id;
  int player_id;
  std::string tag;
};

template<typename Item>
class HashSet {
public:
  HashSet(int n);
  void insert(Item item);

  std::vector<Item> m_items;
  int m_n;
};


#endif /* TYPES_H */
