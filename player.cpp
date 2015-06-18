#include "player.hpp"
#include <map>

std::map<unsigned, Player> players;

Player::Player() :
	name_(),
	club_(),
	score_(0),
	number_(0),
	mid_(0)
{
}

void Player::store()
{
	players.insert(std::make_pair(number_, *this));
}

Player *Player::factory(unsigned number)
{
	auto iter = players.find(number);
	if (iter != players.end())
		return &iter->second;
	return nullptr;
}

const std::string & Player::name() const
{
	return name_;
}

void Player::name(const std::string &name)
{
	name_ = name;
}

const std::string & Player::club() const
{
	return club_;
}

void Player::club(const std::string &club)
{
	club_ = club;
}
unsigned Player::number() const
{
	return number_;
}

void Player::number(unsigned number)
{
	number_ = number;
}
unsigned Player::mid() const
{
	return mid_;
}

void Player::mid(unsigned mid)
{
	mid_ = mid;
}
double Player::score() const
{
	return score_;
}

void Player::score(double score)
{
	score_ = score;
}

double Player::maxscore() const
{
	return maxscore_;
}

void Player::maxscore(double maxscore)
{
	maxscore_ = maxscore;
}

double Player::percent() const
{
	return score_*100.0/maxscore_;
}
