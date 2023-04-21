#ifndef TYPES_H
#define TYPES_H

#pragma once

#include <string>
#include <ctime>
#include <vector>
#include <iostream>
#include <fstream>
#include <array>

constexpr int N_POSITIONS = 15;

class Player {
public:
	int id;
	std::string name;
	std::vector<std::string> positions;
	std::vector<int> position_ids;
	double rating;
	int rating_count;

	Player();

	operator std::string() const;
	std::string get_positions_str();
};

class Rating;

template<typename T>
class OrderedVector : public std::vector<T> {
public:
	void ord_insert(const T&);
	virtual bool compare(const T& a, const T& b) = 0;
	virtual bool equals(const T& a, const T& b) = 0;
	int find_ord(const T& val);

private:
	int binary_search(const T& value, int low, int high);
};

class OrderedIntVector : public OrderedVector<int> {
public:
	virtual bool compare(const int& r1, const int& r2);
	virtual bool equals(const int& a, const int& b);
};


class OrderedRatingVector : public OrderedVector<Rating> {
	virtual bool compare(const Rating& r1, const Rating& r2);
	virtual bool equals(const Rating& a, const Rating& b);
};

struct Rating {
public:
	int id_player;
	double rating;
};

struct User {
public:
	User();
	User(int id);
	void add_rating(int id_player, double rating);

	int id;
	OrderedRatingVector ratings;
};

template<typename K, typename T, unsigned int size>
class HashMap {
public:
	HashMap();
	T* find(K name);
	void insert(T item);

	virtual unsigned int get_key_hash(K) = 0;
	virtual K get_key(T) = 0;

private:
	std::array<T, size> items;
	std::array<K, size> keys;
	std::array<bool, size> item_used;
};

class TagInfo {
public:
	std::string tag_name;
	OrderedIntVector player_ids;
};

class Trie {
public:
	static const int N_SUBTREES = 27;

	Trie();
	// Recursively copying the string would be unnecessary
	int find(const std::string & key, int key_start_pos = 0);
	std::vector<int> find_all(const std::string& key, int key_start_pos = 0);
	std::vector<int> get_all();
	void insert(const std::string&  key, int value, int key_start_pos = 0 );

	bool has_value;
	int value;

	Trie* subtrees[27];
};

// a HashMap from player_id into Player.
template<unsigned int N>
class PlayerHashMap : public HashMap<int, Player, N> {
public:
	PlayerHashMap();
	virtual unsigned int get_key_hash(int);
	virtual int get_key(Player);
};

template<unsigned int N>
class UserHashMap : public HashMap<int, User, N>{
public:
	UserHashMap();
	virtual unsigned int get_key_hash(int);
	virtual int get_key(User);
};

template<unsigned int N>
class TagHashMap : public HashMap<std::string, TagInfo, N> {
public:
	TagHashMap();
	virtual unsigned int get_key_hash(std::string);
	virtual std::string get_key(TagInfo);
};



template<unsigned int N>
class PositionHashMap : public HashMap<std::string, int, N> {
public:
	PositionHashMap();
	virtual unsigned int get_key_hash(std::string);
	virtual int get_key();
};

// Times its existence from constructor to destructor.
// Prints both events to std::cout.
class Clock {
public:
	Clock(std::string funcname);
	~Clock();
	clock_t m_clock;
	std::string m_funcname;
};


bool get_next_player(std::ifstream &file, Player & p);
bool get_next_taginfo(std::ifstream& file, std::string& tag_name, int& player_id);
int  position_to_index(const std::string& pos);
std::string index_to_position(int i);


#endif /* TYPES_H */