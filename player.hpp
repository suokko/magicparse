#pragma once

#include <string>

class Player {
	std::string name_;
	std::string club_;
	double score_;
	double maxscore_;
	unsigned number_;
	unsigned mid_;
public:
	static Player *factory(unsigned number);
	Player();

	const std::string &name() const;
	const std::string &club() const;
	unsigned number() const;
	unsigned mid() const;
	double score() const;
	double maxscore() const;

	void name(const std::string &);
	void club(const std::string &);
	void number(unsigned);
	void mid(unsigned);
	void score(double);
	void maxscore(double);

	double percent() const;

	void store();
};
