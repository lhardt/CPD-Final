#include "types.hpp"
#include <vector>
#include <iomanip>


Clock::Clock(std::string funcname) : m_funcname(funcname){
  std::cout << "Clock started for " << funcname << std::endl;
  m_clock = clock();
}
Clock::~Clock(){
  double time = (clock() - m_clock)/(CLOCKS_PER_SEC + 0.0);
  std::cout << std::fixed << std::setw(11) << std::setprecision(6);
  std::cout << "Finished " << m_funcname << "(" << time  << ")\n";
}


Player::Player() { m_is_blank=true; }
Player::Player(int id, std::string name) : m_id(id), m_name(name), m_is_blank(false) {}
int Player::getHash() { return m_id; } // unique for each player.
bool Player::isBlank(){ return m_is_blank; }

template<typename Item>
HashSet<Item>::HashSet(int n) : m_n(n), m_items(n){

}
template<typename Item>
void HashSet<Item>::insert(Item item){
  int hash = item.getHash();

  int trial = 0;
  while( ! m_items[  (hash + trial)%m_n  ].isBlank() ){
    ++trial;
  }
  m_items[hash+trial] = item;
}


template class HashSet<Player>;
