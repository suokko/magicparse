#pragma once

#include <string>
#include <vector>

class Player;

	class PlayerHolder {
		Player *p;
	public:
		PlayerHolder() : p(nullptr)
		{}

		PlayerHolder(int id);

		PlayerHolder &operator=(int id);

		Player *operator ->()
		{ return p;}
		const Player *operator ->() const
		{ return p;}

		Player *get() { return p; }

		operator bool() const { return !!p; }
	};

	struct Players {
		PlayerHolder n;
		PlayerHolder e;
		PlayerHolder s;
		PlayerHolder w;

		PlayerHolder operator[](size_t nr)
		{ if (nr > 3) return PlayerHolder(); else return (&n)[nr]; }
	};

	class Compass {
	public:
		enum direction {
			NORTH,
			EAST,
			SOUTH,
			WEST,
			EMPTY,
		};

		Compass(direction d = EMPTY) : dir_(d) {}

		direction dir() const { return dir_; }

		bool operator ==(const Compass &b) const { return dir_ == b.dir_; }
	private:
		direction dir_;
	};

struct Play {
	Players players_;
	std::string contract_;
	Compass declarer_;

	std::string tricks_;
	std::string lead_;
	std::string result_;
	double ns_score_;
	double ew_score_;
};

struct Board {
	unsigned number_;
	std::vector<Play> plays_;

	static unsigned total_;
};
