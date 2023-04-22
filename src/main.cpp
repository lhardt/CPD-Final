#include <stdio.h>
#include <future>
#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>
#include <iomanip>
#include <algorithm>
#include "types.h"

class Repo {
public:
	Repo();

	// A hash map of (id_player, avg_rating, n_ratings, list_of(position)).
	PlayerHashMap<20000> players;
	// A hash map of (user_id, list_of( user_id, rating) ).
	UserHashMap<300000> users;
	// A Trie of (player name -> player_id).
	Trie players_trie;
	// For each position, an ordered (by rating) list of (id, rating) pairs.
	std::array<OrderedRatingVector, N_POSITIONS> players_in_position;
	// List of player ids in parsing order.
	std::vector<int> player_ids;
	// A HashMap of (tag_name, list_of( player_ids )), list in order of growing id.
	TagHashMap<40000> tags;
};

Repo::Repo(){ }

int get_number(int x) {
	Clock this_clock("get_number");

	std::cout << "Inside get_number\n";
	return x + x;
}

void read_players_file(Repo & repo) {
	std::ifstream file;
	file.open("players.csv", std::ifstream::in);

	if (!file.good()) {
		std::cout << "Could'nt open players file!\ns";
	}
	// There are 18944 players.
	int n_players = 0;
	std::string first_line;
	std::getline(file, first_line);

	// Just to see which positions there are
	// std::set<std::string> all_positions;

	bool success = true;
	while (file.good() && success) {
		// std::cout <<" will get the next player \n";
		Player p;


		success = get_next_player(file, p);

		// std::cout <<" got the next player " << p.name << "\n"; return;
		if (success) {
			repo.players.insert(p);
			// std::cout <<" Inserting player <<" << p.positions.size() <<  ">>!\n";
			repo.player_ids.push_back(p.id);

				++n_players;
		}
	}

	std::cout << "\t# Players: " << n_players << '\n';
}

void generate_name_trie(Repo & repo) {
	for (auto& id : repo.player_ids) {
		Player * p = repo.players.find(id);
		std::string name = p->name;

		repo.players_trie.insert(name, id);
	}
}

void read_tags_file(Repo& repo) {
	// These are the columns of the tags file.
	// user_id,sofifa_id,tag
	// 17800, 158023, Team Player
	std::ifstream file;
	file.open("tags.csv", std::ifstream::in);

	if (!file.good()) {
		std::cout << "Could'nt open players file!\ns";
	}
	std::string first_line;
	std::getline(file, first_line);

	int repo_tags_size = 0;

	bool success = true;
	while (file.good() && success) {
		std::string tag_name; int player_id;
		success = get_next_taginfo(file, tag_name, player_id);
		if (success) {
			TagInfo* on_repo = repo.tags.find(tag_name);

			if (on_repo == NULL) {
				TagInfo new_info;
				new_info.player_ids.push_back(player_id);
				new_info.tag_name = tag_name;

				repo.tags.insert(new_info);
				++repo_tags_size;
			} else {
				// There are MANY repeated tags.
				if (on_repo->player_ids.find_ord(player_id) == -1) {
					on_repo->player_ids.ord_insert(player_id);
				}
			}
		}
	}

	std::cout <<"\t# Tags: " << repo_tags_size << "\n";

}

void read_ratings_file(Repo& repo) {
	// Using FILE here because ifstream would be too slow.

	FILE* c_file;
	c_file = fopen("rating.csv", "r");
	int n_ratings = 0;

	// There are 138493 different user ids.

	if (c_file == NULL) {
		std::cout << "Error!\n";
		return;
	}
	// ignore the first line
	fscanf(c_file, "%*s");

	bool success = true;
	std::string this_line;
	while (success) {
		int id_user, id_player; double rating;
		int ret_val = fscanf(c_file, " %d,%d,%lf", &id_user, &id_player, &rating);

		success = ret_val == 3;

		if (success) {

			Player* p = repo.players.find(id_player);
			p->rating += rating;
			p->rating_count += 1;
			++n_ratings;

			User* u = repo.users.find(id_user);
			if (u == NULL) {
				User user(id_user);
				user.add_rating(id_player, rating);
				repo.users.insert(user);
			} else {
				u->add_rating(id_player, rating);
			}
		}
	}

	{
		Clock this_clock("Fixing Ratings");
		for (auto& id : repo.player_ids) {
			Player* p = repo.players.find(id);
			p->rating = p->rating / p->rating_count;

			//		std::cout << "Rating is " << p->rating << "\n";
		}

	}
	std::cout << "\t# Ratings: " << n_ratings << '\n';

}

void classify_players_position(Repo & repo) {
	for (int id_player : repo.player_ids) {
		Player* player = repo.players.find(id_player);

		if (player == NULL) continue;
		if (player->rating_count < 1000) continue;

		for (int pos_id : player->position_ids) {
			if (pos_id < 0 || pos_id >= N_POSITIONS) {
				std::cout << "Invalid player position with pid " << player->id << "?\n";
			}

			Rating r;
			r.id_player = id_player;
			r.rating = player->rating;
			repo.players_in_position[pos_id].ord_insert(r);
		}
	}

	for (int i = 0; i < N_POSITIONS; ++i) {
		std::string position_name = index_to_position(i);
		std::cout << "\tPlayers in position ";

		for(int i = 0; i < (4- position_name.size()); ++i) std::cout << " ";

		std::cout << position_name << ":";
		std::cout << repo.players_in_position[i].size() << "\n";
	}
}

/**
 * Prints user ratings in order.
 *
 * As users can only rate in discrete amounts
 * 0, 0.5, 1.0, 1.5, etc, we iterate through those
 * values to give his ratings in descending order.
 */
void print_user_rating(int user_id, const std::vector<Rating> & ratings, int print_limit, Repo & repo) {
	double nota = 5.0;
	int n_printed = 0;
	while (nota >= 0.0) {
		for (const Rating& r : ratings) {
			if (r.rating == nota) {
				Player* p = repo.players.find(r.id_player);
				std::cout
					<< std::setw(7) << p->id
					<< std::setw(40) << p->name
					<< std::setw(20) << p->get_positions_str()
					<< std::setw(12) << p->rating
					<< std::setw(6) << r.rating
					<< std::endl;
				++n_printed;

				if(n_printed == print_limit) return;
			}
		}
		nota -= 0.5;
	}
}

void load_repo(Repo & repo ) {
	Clock all_clock("Loading Repo");

	{
		Clock this_clock("Read Players File");
		read_players_file(repo);
	}
	{
		Clock this_clock("Read Tags File");
		read_tags_file(repo);
	}
	{
		Clock this_clock("Generate Name Trie");
		generate_name_trie(repo);
	}
	{
		Clock this_clock("Load Ratings");
		read_ratings_file(repo);
	}
	{
		Clock this_clock("Classify Players");
		classify_players_position(repo);
	}
}

std::vector<std::string> parse_tags_list(std::string param_str) {
	std::vector<std::string> tags_list;

	int i_start = param_str.find('\'');
	param_str = param_str.substr(i_start+1);

	i_start = 0;
	while (i_start < param_str.size()) {
		int i_end = param_str.find('\'');
		if (i_end == -1) { break; }

		std::string this_tag = param_str.substr(0, i_end );
		tags_list.push_back(this_tag);

		param_str = param_str.substr(i_end + 1);
		// consume the start of the next;
		int next_start = param_str.find('\'');
		if (next_start == -1) break;

		param_str = param_str.substr(next_start + 1);

	}

	return tags_list;
}

void tags_intersec_help(Repo & repo, std::vector<int> &intersec, std::string &tag) {
	TagInfo * info = repo.tags.find(tag);
	if (info == NULL) {
		std::cout << "Tag not found: <" << tag << ">\n";
		intersec.clear();
		return;
	} else {
		OrderedIntVector info_ids = info->player_ids;

		for (int i = 0; i < intersec.size(); ++i) {
			if (info_ids.find_ord(intersec[i]) == -1) {
				intersec.erase(intersec.begin() + i);
				--i;
			}
		}
	}
}

std::vector<int> tags_intersec(Repo& repo, std::vector<std::string> &tags) {
	if (tags.size() == 0) return std::vector<int>();

	TagInfo * first_info = repo.tags.find(tags[0]);

	if (first_info == NULL) {
		std::cout << "Tag not found <" << tags[0] << ">\n";
		return std::vector<int>();
	}

	std::vector<int> intersec(first_info->player_ids);

	for (int i = 1; i < tags.size(); ++i) {
		tags_intersec_help(repo, intersec, tags[i]);
	}

	return intersec;
}

void answer_player_query(Repo & repo, std::string param_str){
	std::vector<int> player_ids = repo.players_trie.find_all(param_str);

	if (player_ids.size() == 0) {
		std::cout << "No such players found.\n";
	} else {
		// This vector uses insertion sort to keep players.
		// This is not ideal, but enough for querying
		// players in real time. The best approach would be
		// a set.
		OrderedRatingVector player_ratings;

		for (auto& id : player_ids) {
			Player p = * (repo.players.find(id));
			player_ratings.ord_insert( (Rating){ .id_player = p.id, .rating = p.rating} );
		}

		std::cout
			<< std::setw(7) << "ID"
			<< std::setw(40) << "Player Name"
			<< std::setw(20) << "Positions"
			<< std::setw(12) << "Rating"
			<< std::setw(6) << "Count"
			<< std::endl;
		for (Rating& rating : player_ratings) {
			Player p = * (repo.players.find(rating.id_player));

			std::cout
				<< std::setw(7) << p.id
				<< std::setw(40) << p.name
				<< std::setw(20) << p.get_positions_str()
				<< std::setw(12) << p.rating
				<< std::setw(6) << p.rating_count
				<< std::endl;
		}
	}
}

void answer_user_query(Repo & repo, std::string param_str){
	int id_user = std::stoi(param_str);

	User* u = repo.users.find(id_user);

	if (u == NULL) {
		std::cout << "Not found.\n";
		return;
	}
	if (u->ratings.size() == 0) {
		std::cout << "This user hasn't rated any player.\n";
		return;
	}

	std::cout << "His ratings: (maximum of 20)\n";

	std::cout
		<< std::setw(7) << "ID"
		<< std::setw(40) << "Player Name"
		<< std::setw(20) << "Positions"
		<< std::setw(12) << "Av. Rating"
		<< std::setw(6) << "Usr. Rating"
		<< std::endl;

	print_user_rating(id_user, u->ratings, 20, repo);
}

void answer_top_query(Repo & repo, int n_top, std::string param_str){
	param_str.erase(std::remove(param_str.begin(), param_str.end(), ' '), param_str.end());
	param_str.erase(std::remove(param_str.begin(), param_str.end(), '\''), param_str.end());

	int i_pos = position_to_index(param_str);
	if (i_pos == -1) {
		std::cout << "Did not find the position <" << param_str << ">.\n";
		return;
	}

	if (n_top > repo.players_in_position[i_pos].size()) {
		n_top = repo.players_in_position[i_pos].size();
		std::cout << "There are only " << n_top << " players in this position (w/ +1000 ratings).\n";
	}

	std::cout << "Top " << n_top << "of position '" << param_str << "':\n";

	for (int i = 0; i < n_top; ++i) {
		int id_player = repo.players_in_position[i_pos][i].id_player;

		Player p = *(repo.players.find(id_player));
		std::cout
			<< std::setw(7) << p.id
			<< std::setw(40) << p.name
			<< std::setw(20) << p.get_positions_str()
			<< std::setw(12) << p.rating
			<< std::setw(6) << p.rating_count
			<< std::endl;
	}
}

void answer_tags_query(Repo & repo, std::string param_str){
	std::vector<std::string> tags_list = parse_tags_list(param_str);

	std::vector<int> player_ids = tags_intersec(repo, tags_list);

	if(player_ids.size() == 0){
		std::cout << "No players with this combination of tags.\n";
		return;
	} else {
		std::cout
			<< std::setw(7) << "ID"
			<< std::setw(40) << "Player Name"
			<< std::setw(20) << "Positions"
			<< std::setw(12) << "Rating"
			<< std::setw(6) << "Count"
			<< std::endl;

		for (const int id : player_ids) {
			Player p = *(repo.players.find(id));
			std::cout
				<< std::setw(7) << p.id
				<< std::setw(40) << p.name
				<< std::setw(20) << p.get_positions_str()
				<< std::setw(12) << p.rating
				<< std::setw(6) << p.rating_count
				<< std::endl;
		}
	}
}

int main() {

	std::shared_ptr<Repo> repo(new Repo());

	load_repo(*repo);

	while (true) {
		std::cout << "Query [player,user,topN,tags,quit]:\n>>>\t";
		std::string query_string;
		std::getline(std::cin, query_string);

		if (query_string == "quit"){
			std::cout <<"Bye!\n";
			return 0;
		}

		int first_space = query_string.find_first_of(' ');
		if (first_space == -1) {
			std::cout << "Try again.\n";
			continue;
		}

		std::string until_first_space = query_string.substr(0, first_space);
		std::string param_str = query_string.substr(first_space + 1);

		// Aligning table values on the left.
		std::cout << std::setiosflags(std::ios::left);

		if (until_first_space == "player") {
			answer_player_query(*repo, param_str);
		} else if (until_first_space == "user") {
			answer_user_query(*repo, param_str);
		} else if (until_first_space.find("top") == 0) {
			// For strings 'top10' or 'top500' we should have n_top = 10 and 500
			std::string n_top_str = until_first_space.substr(3);
			int n_top = std::stoi(n_top_str);

			answer_top_query(*repo, n_top, param_str);
		} else if (until_first_space == "tags") {
			answer_tags_query(*repo, param_str);
		} else {
			std::cout << "Query not recognized. <" << until_first_space << ">\n";
		}
	}
	return 0;
}