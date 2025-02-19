#pragma once
#include <math.h>
#include <stdlib.h>

namespace TChapman500 {
namespace Elo {

// Calculate delta specifying the winner.
int calculate_delta(int elo1, int elo2, int k, bool player1Won)
{
	// Calcualte probability of player 1 victory.
	double probability = 1.0 / (1.0 + pow(10.0, (((double)elo2 - (double)elo1) / 400.0)));
	double result;

	// Player 1 won the match.
	if (player1Won) result = round((double)k * (1.0 - probability));
	// Player 2 won the match.
	else result = round((double)k * (0.0 - probability));
	return (int)result;
}

// Calculate delta assuming player 1 won the match.
int calculate_delta(int elo1, int elo2, int k)
{
	// Calcualte probability of player 1 victory.
	double probability = 1.0 / (1.0 + pow(10.0, (((double)elo2 - (double)elo1) / 400.0)));
	// Player 1 won the match.
	double result = round((double)k * (1.0 - probability));
	return (int)result;
}

// Apply delta specifying the winner.
void apply_delta(int &elo1, int &elo2, int k, bool player1Won)
{
	int delta = calculate_delta(elo1, elo2, k, player1Won);
	elo1 += delta;
	elo2 -= delta;
}

// Apply delta assuming player 1 won the match.
void apply_delta(int &elo1, int &elo2, int k)
{
	int delta = calculate_delta(elo1, elo2, k);
	elo1 += delta;
	elo2 -= delta;
}




}}
