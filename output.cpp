#include "output.hpp"
#include "board.hpp"
#include "player.hpp"

#include <iostream>
#include <algorithm>

#include <iomanip>

static void push_player(std::vector<Player *> &vec, Player *p)
{
	/* TODO: O(p*b) */
	if (std::find(vec.begin(), vec.end(), p) != vec.end())
		return;

	vec.push_back(p);
}

template <typename T>
struct comma_separator : std::numpunct<T>
{
	typename std::numpunct<T>::char_type do_decimal_point() const
	{
		return ',';
	}
};

	template <typename T>
std::basic_ostream<T>& comma_sep(std::basic_ostream<T>& os)
{
	os.imbue(std::locale(std::locale(""), new comma_separator<T>));
	return os;
}

struct PlayOut {
	Play *p;
	PlayOut(Play *p) : p(p) {}
};

std::ostream &operator<<(std::ostream &os, const Compass &c);

std::ostream &operator<<(std::ostream &os, const PlayOut &po)
{
	bool nsscore = std::isdigit(po.p->result_[0]);
	os << std::right
		<< std::setw(3) << po.p->players_.n->number()
		<< std::setw(3) << po.p->players_.s->number()
		<< std::setw(3) << po.p->players_.e->number()
		<< std::setw(3) << po.p->players_.w->number()
		<< std::left
		<< "  "
		<< std::setw(4) << po.p->contract_
		<< std::setw(1) << po.p->declarer_
		<< std::right
		<< std::setw(3) << po.p->tricks_
		<< ' '
		<< std::left
		<< std::setw(3) << po.p->lead_
		<< std::right
		<< std::setw(nsscore ? 6 : 10) << po.p->result_
		<< std::setw(nsscore ? 10 : 6) << po.p->ns_score_
		<< std::setw(5) << po.p->ew_score_
		<< "  "
		;
	return os;
}


Output::Output(const std::string &header,
		const std::string &middle,
		const std::string &footer,
		std::vector<Board> &boards,
		std::vector<unsigned> &vp)
{
	std::vector<Player *> players;
	unsigned i;

	/* Score calculation */
	for (Board &b : boards) {
		for (Play &p : b.plays_) {
#if 1
			if (p.players_.n->number() > 12) {
				std::swap(p.players_.n, p.players_.e);
				std::swap(p.players_.s, p.players_.w);
			}
#endif
			for (i = 0; i < 4; i++) {
				push_player(players, p.players_[i].get());
				double score;
				double total;
				if (i % 2 == 0) {
					score = p.ns_score_;
					total = score + p.ew_score_;
				} else {
					score = p.ew_score_;
					total = score + p.ns_score_;
				}
				p.players_[i]->score(p.players_[i]->score() + score);
				p.players_[i]->maxscore(p.players_[i]->maxscore() + total);
			}
		}
	}

	std::sort(players.begin(), players.end(), [](Player *a, Player *b) { return a->percent() > b->percent(); });

	std::cout << header;

	unsigned pos = 1;
	double previous = 0.0;

	std::cout << comma_sep;

	for (Player *p: players) {
		if (std::abs(previous - p->percent()) < 0.000001)
			std::cout << std::setw(4) << std::right << "";
		else
			std::cout << std::setw(4) << std::right << pos;
		std::cout
			<< std::setw(9) << p->number()
			<< std::setprecision(1) << std::fixed
			<< std::setw(9) << p->score()
			<< std::setw(6) << p->percent()
			<< std::setw(2) << std::left << " "
			<< std::setw(p->mid() > 0 ? 20 : 18) << p->name();
		if (p->mid() > 0)
			std::cout
				<< std::setw(6) << p->mid()
				<< std::setw(15) << p->club();
		if (pos <= vp.size())
			std::cout << "\t\t" << vp[pos - 1];
		std::cout << '\n';
		previous = p->percent();
		if (pos % 5 == 0) {
			if (pos <= vp.size())
				std::cout << "\t\n";
			else
				std::cout << '\n';
		}
		pos++;
	}

	std::cout << middle;

	Board *b1;

	std::cout << '-'
		<< std::right
		<< std::setfill('-') << std::setw(50) << '-'
		<< std::setw(50) << '-'
		<< '\n';

	for (Board &b2: boards) {
		if (b2.number_ % 2 == 1) {
			b1 = &b2;
			continue;
		}

		std::cout << '!'
			<< std::setfill(' ') << std::setw(50) << '!'
			<< std::setw(50) << '!'
			<< '\n';

		std::cout << '!'
			<< std::left
			<< " Jako:  "
			<< std::setw(3) << b1->number_
			<< std::right
			<< std::setw(39) << '!'
			<< std::left
			<< " Jako:  "
			<< std::setw(3) << b2.number_
			<< std::right
			<< std::setw(39) << '!'
			<< '\n';

		std::cout << "!   Pelaaja    Sit      LK      Tulos   Pisteet   !   Pelaaja    Sit      LK      Tulos   Pisteet   !\n";

		for (pos = 0; pos < b1->plays_.size(); pos++) {
			PlayOut p1(&b1->plays_[pos]);
			PlayOut p2(&b2.plays_[pos]);
			std::cout << '!' << p1 << '!' << p2 << '!' << '\n';
		}


		std::cout << '!'
			<< std::right
			<< std::setfill(' ') << std::setw(50) << '!'
			<< std::setw(50) << '!'
			<< '\n';

		std::cout << '!'
			<< std::setfill(' ') << std::setw(50) << '!'
			<< std::setw(50) << '!'
			<< '\n';

		std::cout << '-'
			<< std::right
			<< std::setfill('-') << std::setw(50) << '-'
			<< std::setw(50) << '-'
			<< '\n';
	}

	std::cout << footer;
}
