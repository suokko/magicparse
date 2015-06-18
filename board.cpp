#include "board.hpp"
#include "player.hpp"

PlayerHolder::PlayerHolder(int id) : p(Player::factory(id))
{}

PlayerHolder &PlayerHolder::operator=(int id)
{
	p = Player::factory(id);
	return *this;
}

