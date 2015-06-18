
#include "player.hpp"
#include "parser.hpp"
#include "board.hpp"
#include "output.hpp"
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/bind.hpp>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

namespace qi = boost::spirit::qi;

struct special_policy : public qi::real_policies<double>
{
	template <typename Iterator>
	static bool parse_dot(Iterator& first, Iterator const& last)
	{
		if (first == last || *first != ',')
			return false;
		++first;
		return true;
	}

};

typedef qi::real_parser<double,special_policy> my_real_parser;

my_real_parser	my_double_;

std::ostream &operator<<(std::ostream &os, const Compass &c)
{
	static const char conv[] = "NESW ";
	return os << conv[c.dir()];
}

std::ostream &operator<<(std::ostream &os, const PlayerHolder &ph)
{
	if (!ph)
		return os << "null";
	return os << ph->number();
}

std::ostream &operator<<(std::ostream &os, const Play &p)
{
	return os << p.players_.n << ' '
		<< p.players_.s << ' '
		<< p.players_.e << ' '
		<< p.players_.w << ' '
		<< p.contract_ << ' '
		<< p.declarer_ << ' '
		<< p.tricks_ << ' '
		<< p.lead_ << ' '
		<< p.result_ << ' '
		<< p.ns_score_ << ' '
		<< p.ew_score_;
}

BOOST_FUSION_ADAPT_STRUCT(
		Players,
		(PlayerHolder, n)
		(PlayerHolder, s)
		(PlayerHolder, e)
		(PlayerHolder, w));

BOOST_FUSION_ADAPT_STRUCT(
		Play,
		(Players, players_)
		(std::string, contract_)
		(Compass, declarer_)
		(std::string, tricks_)
		(std::string, lead_)
		(std::string, result_)
		(double, ns_score_)
		(double, ew_score_)
		);


struct compass_symbol : public qi::symbols<char, Compass::direction>
{
	compass_symbol()
	{
		add
			("N", Compass::NORTH)
			("E", Compass::EAST)
			("S", Compass::SOUTH)
			("W", Compass::WEST)
		;
	}
} compass_symbol_;

template <typename Iterator>
struct players_parser : qi::grammar<Iterator, Players(), boost::spirit::ascii::space_type>
{
	players_parser() : players_parser::base_type(start)
	{
		using qi::int_;
		using qi::lexeme;
		using boost::spirit::iso8859_1::char_;

		nospace_string %= lexeme[+(char_ - char_(" !"))];

		start %=
			// players
			int_ >> int_ >> int_ >> int_
			;
	}
	qi::rule<Iterator, std::string(), boost::spirit::ascii::space_type> nospace_string;
	qi::rule<Iterator, Players(), boost::spirit::ascii::space_type> start;
};

template <typename Iterator>
struct play_parser : qi::grammar<Iterator, Play(), boost::spirit::ascii::space_type>
{
	play_parser() : play_parser::base_type(start)
	{
		using qi::int_;
		using qi::lit;
		using qi::attr;
		using qi::double_;
		using qi::lexeme;
		using boost::spirit::iso8859_1::char_;

		nospace_string %= lexeme[+(char_ - char_(" !"))];


		;

/*
!  3 20 34 39  2H  W -2 DA    100      21,0  1,0
*/
		start %=
			lit("!")
			>> players_
			// contract, declarer
			>> nospace_string >> compass_symbol_
			// tricks, lead
			>> nospace_string >> nospace_string
			// result
			>> nospace_string
			>> my_double_ >> my_double_ >> &lit("!") |
			lit("!")
			>> players_
			>> attr("") >> attr(Compass::EMPTY)
			>> attr("") >> attr("")
			>> nospace_string
			>> my_double_ >> my_double_ >> &lit("!")
			;
	}

	qi::rule<Iterator, std::string()> nospace_string;
	qi::rule<Iterator, Play(), boost::spirit::ascii::space_type> start;
	players_parser<Iterator> players_;
};

template <typename Iterator>
struct parser {
	unsigned i;
	bool found_start;
	std::string header;
	std::string middle;
	std::string footer;
	std::vector<unsigned> vp;
	std::vector<Board> boards;

	play_parser<Iterator> playp;
	void parse_names(Iterator first, Iterator last)
	{
		using qi::int_;
		using qi::lit;
		using qi::phrase_parse;
		using boost::spirit::ascii::space;
		using boost::spirit::iso8859_1::char_;
		using boost::phoenix::ref;
		using boost::phoenix::push_back;

		Iterator orig = first;

#if 1
		qi::rule<Iterator, std::string(), boost::spirit::ascii::space_type> name =
			qi::lexeme[+(char_ - ' ') >>
			+(!lit("  ") >>
				(char_(" ") >> +(char_ - ' ')))];

		qi::rule<Iterator, std::string(), boost::spirit::ascii::space_type> club =
			qi::lexeme[+(char_ - char_(" \t")) >>
			*(!(lit("  ") | lit(" \t") | lit("\t")) >>
				(char_(" ") >> +(char_ - char_(" \t"))))];
#endif
/*
   1       35    321,0  60,8  Christina Hertzman  1156  B-55                    15
*/
		if (i == 5) {
			i = 0;
			qi::parse(first, last, space);
			if (first == last) {
				/* skip every fifth empty line */
				return;
			}
		}
		Player player;
		bool r = phrase_parse(first, last,
			// sija
			((int_
			// numero
			>> int_[boost::bind(&Player::number, &player, _1)] >> !lit(",")) |
			int_[boost::bind(&Player::number, &player, _1)])
			// pisteet
			>> my_double_
			// prosentti
			>> my_double_
			// Nimi
			>> name[boost::bind(&Player::name, &player, _1)]
			// mid
			>> -int_[boost::bind(&Player::mid, &player, _1)]
			// Kerho
			>> -club[boost::bind(&Player::club, &player, _1)]
			>> -int_[push_back(boost::phoenix::ref(vp), qi::_1)]
			,
			space);

		if (r && first == last) {
			found_start = true;
			player.store();
			i++;
			return;
		}
		if (found_start) {
			parse = &parser<Iterator>::parse_traveler;
			middle.append(orig, last);
			middle += '\n';
			return;
		}
		header.append(first, last);
		header += '\n';
	}

	void parse_traveler(Iterator first, Iterator last)
	{
		using boost::spirit::iso8859_1::char_;
		Iterator orig = first;
		qi::parse(first, last, +(char_ - char_("-")));
		middle.append(orig, first);
		if (first == last) {
			middle += '\n';
		} else {
			parse = &parser<Iterator>::parse_board_number;
		}
	}

	void parse_footer(Iterator first, Iterator last)
	{
		footer.append(first, last);
		footer += '\n';
	}

	void parse_board_number(Iterator first, Iterator last)
	{
		using boost::spirit::ascii::space;
		using boost::spirit::lit;
		using boost::spirit::qi::int_;
		bool r;
		i = 0;
		if (*first == '<') {
			parse = &parser<Iterator>::parse_footer;
			parse_footer(first, last);
			return;
		}
		do {
			Board b;
			r = qi::phrase_parse(first, last,
					lit("!") >> lit("Jako:") >>
					int_[boost::phoenix::ref(b.number_) = qi::_1],
					space);
			if (r) {
				boards.push_back(b);
				parse = &parser<Iterator>::parse_play;
				found_start = false;
				i++;
			}
		} while (r);
	}

	void parse_play(Iterator first, Iterator last)
	{
		using boost::spirit::iso8859_1::char_;
		unsigned j = 0;
		bool r = false;
		do {
			Play p;
			r = qi::phrase_parse(first, last, playp
					, boost::spirit::ascii::space, p);
			if (r) {
				if (p.declarer_ == Compass())
					p.contract_.clear();
				boards[boards.size() - i + j].plays_.push_back(p);
				j++;
				found_start = true;
			} else if (found_start && j == 0) {
				parse = &parser<Iterator>::parse_board_number;
			}
		} while (r);
	}

	void (parser::*parse)(Iterator first, Iterator last);

	parser() : i(0), found_start(0), parse(&parser<Iterator>::parse_names)
	{}
};

void parse_result(std::istream &input)
{
	typedef parser<std::string::iterator> prsr;
	prsr p;
	std::string line;
	while(getline(std::cin, line)) {
		(p.*(p.parse))(line.begin(), line.end());
	}

	// Output the results
	Output(p.header, p.middle, p.footer, p.boards, p.vp);
}
