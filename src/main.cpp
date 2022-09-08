#include <iostream>
#include <ctime>
#include <vector>
#include <iomanip>
#include <memory>
#include <fstream>


#include "types.hpp"
#include "parser.hpp"

class InfoRepository {
public:
  InfoRepository() : m_players(0) {}
  void load(std::string playerFileName, std::string ratingFileName, std::string tagsFileName);

  void searchPlayer(std::string namePrefix);
  void searchUserPlayers(int userId);
  void searchTopInPosition(std::string position);
  void searchPlayersByTags(std::vector<std::string> tags);

  void clockStart(std::string funcname);
  void clockEnd(std::string funcname);

private:
  void parsePlayersFile();

  HashSet<Player> m_players;

  clock_t clock;
  std::string funcName;
};

void InfoRepository::load(std::string playerFileName, std::string ratingFileName, std::string tagsFileName){
  m_players = HashSet<Player>(200);


  std::ifstream player_file;
  player_file.open(playerFileName, std::ifstream::in);

  int i_line = 0;
  while(player_file.good()){
    std::string this_line;
    std::getline(player_file, this_line);

    std::cout << "\t" << this_line << "\n";
    ++i_line;


    if(i_line > 10) break;
  }
}

// void InfoRepository::searchPlayer(std::string namePrefix);
// void InfoRepository::searchUserPlayers(int userId);
// void InfoRepository::searchTopInPosition(std::string position);
// void InfoRepository::searchPlayersByTags(std::vector<std::string> tags);


int main(){
  Clock clock("main");
  InfoRepository repo;

  {
    Clock loadClock("load");
    repo.load("files/players.csv", "files/ratings.csv", "files/tags.csv");

    HashSet<Player> players(2500);
  }


  return 0;
}
