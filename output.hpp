#pragma once

#include "board.hpp"
#include <string>
#include <vector>

class Output {
public:
	Output(const std::string &header,
			const std::string &middle,
			const std::string &footer,
			std::vector<Board> &boards,
			std::vector<unsigned> &vp
			);
};
