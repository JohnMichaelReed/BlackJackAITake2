#include <algorithm>    // std::random_shuffle

#include <iomanip>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <array>

using namespace std;




// array position of dealer's hole card:
unsigned int holePosition = 0;

// value of dealer's hole card:
int hole = 0;

// Gains or losses after each simulation runs:
double gains = 0.0;

// Depth delved into the recursive card combination simulation tree:
int j = 0;

// Amount of money bet by the player:
int bet = 10;

// Amount of money in the player's "bank":
int bank = 0;

// Controls whether dealer Hits on 17 or Stands on 17 (Ace, 7);
bool H17 = false;

// Controls whether the dealer peeks for 21 or not before  
bool peek = false;

// Controls the number of decks present.
int decks = 1;

// new save point.

int myrandom(int i) {
    return std::rand() % i;
}

// shoe of real cards. Excludes the hole card.
vector<int> shoe;

// shoe of virtual cards for the simulator. Includes the hole card.
vector<int> simShoe;

// The player and dealer are BlackJack participants.

struct BlackJackParticipant {
    // Actual and virtual score:
    int score;
    int simScore;

    // Actual and virtual soft aces:
    int numAces;
    int simNumAces;

    // Actual and virtual hard aces:
    int numHdAces;
    int simNumHdAces;

    // Contains the real cards that the dealer has, including the hidden hole card.
    vector<int> hand;

    // simulationDealerHand does not have the hole card. 
    vector<int> simHand;

    // draws card from real shoe (deck) into real hand.

    void drawCard(vector<int> &shoe) {
        // Remove card from deck.

        int card = shoe.back();
        shoe.pop_back();
        hand.push_back(card);
        score += card;

        if (card == 11) {
            ++numAces;
        }

        // Turns 11 valued aces into 1 valued aces from the back of the hand to the front.      
        for (unsigned int p = (hand.size() - 1); score > 21 && numAces > 0 && p < 1000; p--) {
            if (hand.at(p) == 11) {
                hand.at(p) = 1;
                score -= 10;
                numHdAces++;
                numAces--;
            }
        }
    }

    // draws card from simulation shoe (deck) into simulation hand.

    void simDrawCard(vector <int> &simShoe, const int &index) {
        int card = 0;
        card = simShoe.at(index);
        // 0 is a placeholder here.
        simShoe.at(index) = 0;
        if (card == 0) {
            cout << "Error simulator drew null placeholder." << endl;
        }

        simHand.push_back(card);
        simScore += card;


        if (card == 11) {
            ++simNumAces;
        }
        // Turns soft 11 aces into hard 1 valued aces from back of hand vector to front of hand vector.
        for (unsigned int p = (simHand.size() - 1); simScore > 21 && simNumAces > 0 && p < 1000; p--) {
            if (simHand.at(p) == 11) {
                simHand.at(p) = 1;
                simScore -= 10;
                simNumHdAces++;
                simNumAces--;
            }
        }
    }

    // Puts virtual cards back into virtual deck null placeholder slots during a simulation. 

    void simUnDrawCard(vector<int> &simShoe, const int &index, vector<int> &simHand) {
        // turns a hard ace into an 11 valued ace before placing back into deck.
        if (simHand.back() == 1) {
            simHand.back() = 11;
            simNumHdAces--;

            simShoe.at(index) = simHand.back();

            simScore -= 1;
            simHand.pop_back();
        } else if (simHand.back() == 11) {
            simNumAces--;
            simShoe.at(index) = simHand.back();
            simScore -= simHand.back();
            simHand.pop_back();
        } else {
            if (simShoe.at(index) != 0) {
                cout << "error simShoe.at(index) != 0" << endl;
            }
            simShoe.at(index) = simHand.back();
            simScore -= simHand.back();
            simHand.pop_back();
        }

        for (unsigned int g = 0; simScore < 12 && simNumHdAces > 0 && g < simHand.size(); g++) {
            // Turns hard 1 valued aces into soft 11 valued aces from front of hand vector to back of hand vector. 
            // Does the reverse of simDrawCard.
            if (simHand.at(g) == 1) {
                simHand.at(g) = 11;
                simNumHdAces--;
                simNumAces++;
                simScore += 10;
            }
        }
    }
};

// Creates a dealer.

struct Dealer : public ::BlackJackParticipant {
} dealer;

// Analyzers have the power of statistical simulation analysis.

struct Analyzer : public ::BlackJackParticipant {
    // Finds out expected gains or losses if you hit once and then analyze all possible dealer combinations.
    // This function looks 1 move ahead.

    double analyzeHit(Dealer &simDealer, Analyzer &analyzer) {
        double gains = 0.0;
        ::gains = 0.0;

        double numerator = 0.0;
        for (unsigned int v = 0; v < ::simShoe.size(); v++) {
            if (::simShoe.at(v) != 0) {
                ::gains = 0.0;
                analyzer.simDrawCard(::simShoe, v);
                numerator += analyzeStand(simDealer, analyzer);
                ::gains = 0.0;
                analyzer.simUnDrawCard(::simShoe, v, analyzer.simHand);
            }
        }

        gains = numerator / ((double) (shoe.size() + 1));
        ::gains = 0.0;
        return gains;
    }
    // Finds out expected gains or losses if you hit once and then either hit again or analyze all possible dealer combinations
    // Depending on which move is smarter. This function looks 2 moves ahead.

    double analyzeHitHit(Dealer &simDealer, Analyzer &analyzer) {
        ::j = 0;
        double gains = 0.0;
        ::gains = 0.0;
        for (unsigned int v = 0; v < ::simShoe.size(); v++) {
            if (::simShoe.at(v) != 0) {
                analyzer.simDrawCard(::simShoe, v);
                double x = analyzeStand(simDealer, analyzer);
                ::gains = 0;
                double y = analyzeHit(simDealer, analyzer);
                if ((x < y) && (simDealer.simScore < 22)) {

                    for (unsigned int c = 0; c < ::simShoe.size(); c++) {
                        if (::simShoe.at(c) != 0) {
                            analyzer.simDrawCard(::simShoe, c);
                            ::gains = 0.0;
                            gains += (analyzeStand(simDealer, analyzer) / (((shoe.size() + 1) + 1.0)*((shoe.size() + 1) + 2.0)));
                            ::gains = 0.0;
                            analyzer.simUnDrawCard(::simShoe, c, analyzer.simHand);
                        }
                    }
                } else if ((x >= y) || (simDealer.simScore >= 22)) {
                    ::gains = 0.0;
                    gains += (analyzeStand(simDealer, analyzer) / ((shoe.size() + 1) + 1.0));
                    ::gains = 0.0;
                } else {
                    cout << "error x and y" << endl;
                }
                analyzer.simUnDrawCard(::simShoe, v, analyzer.simHand);
            }
        }
        ::j = 0;
        ::gains = 0.0;
        return gains;
    }

    // Permute uses permutations/combinations to find the likelihood of a particular set of cards occuring.
    // Permute multiplies this likelihood by the wager to calculate anticipated change in bank after a bet.

    double permute(int j) {
        double denominator = 1.0;
        while (j > 0) {
            denominator = denominator * ((double) (((shoe.size() + 1) + j)));
            --j;
        }
        double change = (((double) bet) / (denominator));
        return change;
    }

    // Finds out expected gains or losses if you analyze all possible dealer combinations and their effect on the player(analyzer).

    double analyzeStand(Dealer &simDealer, Analyzer &analyzer) {
        // Removes possibility of drawing an ace if deeler peeks at a ten hole card and declares no blackjack. 999 is a placeholder.
        if ((dealer.simHand.size() == 1) && (::peek == true) && (dealer.simHand.at(0) == 10) && j == 0) {
            for (unsigned int i = 0; i < ::simShoe.size(); i++) {
                if (::simShoe.at(i) == 11) {
                    ::simShoe.at(i) = 999;
                }
            }
        }            // Removes possibility of drawing a ten if deeler peeks at an ace hole card and declares no blackjack. 999 is a non-zero placeholder.
        else if ((dealer.simHand.size() == 1) && (::peek == true) && (dealer.simHand.at(0) == 11) && j == 0) {
            for (unsigned int i = 0; i < ::simShoe.size(); i++) {
                if (::simShoe.at(i) == 10) {
                    ::simShoe.at(i) = 999;
                }
            }

        }

        // The higher j is, the deeper the function has delved into the recursive tree.
        ::j++;

        // For every possible dealer hand, monetary gains or losses will accumulate over all possible combination.
        for (unsigned int i = 0; i < ::simShoe.size() - 3; i++) {
            // Do not draw 0 valued drawn cards or impossible dealer blackjack 999 cards. 
            if ((simShoe.at(i) != 0) && (simShoe.at(i) != 999)) {
                // take unknown virtual card from shoe into hole.
                simDealer.simDrawCard(::simShoe, i);

                // Dealer hits on 16 or below.
                if (simDealer.simScore <= 16) {
                    // The dealer must draw go into the next level of simulation and draw another card.
                    analyzeStand(simDealer, analyzer);
                }

                    // Dealer hits on soft 17 if H17 is the rule and there is an (11,6).
                else if ((simDealer.simScore == 17) && (::H17 == true) && (simDealer.simNumAces > 0)) {
                    // The dealer must draw go into the next level of simulation and draw another card.
                    analyzeStand(simDealer, analyzer);
                }
                else if ((simDealer.simScore == 21 && dealer.simHand.size() == 2) && (analyzer.simScore == 21 && analyzer.simHand.size() == 2)) {
                    // tie - do nothing.
                } else if ((simDealer.simScore == 21 && dealer.simHand.size() == 2)) {
                    //dealer blackjack. Should be impossible if 10's and 11's replaced with 999.
                    double change = analyzer.permute(j);
                    gains -= change;
                } else if ((analyzer.simScore == 21 && analyzer.simHand.size() == 2) && (simDealer.simScore >= 17)) {
                    //player blackjack.
                    double change = analyzer.permute(j);
                    //blackjack pays 3:2
                    gains += (3 * change) / 2;
                } else if (analyzer.simScore > 21) {
                    //player busts.
                    double change = analyzer.permute(j);
                    gains -= change;
                } else if (simDealer.simScore == analyzer.simScore) {
                    // tie - do nothing.
                } else if (simDealer.simScore > 21) {
                    double change = analyzer.permute(j);
                    gains += change;
                } else if (simDealer.simScore <= 21 && (analyzer.simScore < simDealer.simScore)) {
                    double change = analyzer.permute(j);
                    gains -= change;
                } else if (simDealer.simScore <= 21 && (analyzer.simScore > simDealer.simScore)) {
                    double change = analyzer.permute(j);
                    gains += change;
                } else {
                    cout << "error at end of calculation1." << endl;
                }

                simDealer.simUnDrawCard(::simShoe, i, simDealer.simHand);
            }
        }
        ::j--;

        // Put aces back into simulator if they were replaced with 999's
        if ((dealer.simHand.size() == 1) && (::peek == true) && (dealer.simHand.at(0) == 10) && (::j == 0)) {
            for (unsigned int i = 0; i < ::simShoe.size(); i++) {
                if (::simShoe.at(i) == 999) ::simShoe.at(i) = 11;
            }
        }            // Put tens back into simulator if they were replaced with 999's
        else if ((dealer.simHand.size() == 1) && (::peek == true) && (dealer.simHand.at(0) == 11) && (::j == 0)) {
            for (unsigned int i = 0; i < ::simShoe.size(); i++) {
                if (::simShoe.at(i) == 999) ::simShoe.at(i) = 10;
            }
        }
        return ::gains;
    }
};

// Player has the ability to conduct statistical analysis (is analyzer).

struct Player : public Analyzer {
} player;

// Planned computer AI player feature not yet availible.
// Computer AI below also has the ability to conduct statistical analysis.

//struct Comp : public Analyzer 
//{
//      
//} ;

int main() {
    cout << "Welcome to C++ BlackJack simulator 1.5 by John-Michael Reed." << endl;
    cout << "The goal of this game is to learn how to beat the dealer." << endl;
    cout << "How many dollars will you be converting into chips? \n Note the casino always rounds chips and payouts down." << endl;
    cin >> ::bank;
    cout << endl;
    std::srand(unsigned ( std::time(0)));

    bool decideH17 = false;
    bool decidePeeking = false;
    bool decideBet = false;
    bool insured = false;
    char g;

    // Should the dealer shuffle a new deck?
    bool decideNewDeck = false;
    while (decideNewDeck == false) {
        cout << "How many decks would you like to play with?\n Enter -2,1,2,3,4,5,6,7, or 8." << endl;
        cin >> ::decks;
        if (::decks == -2) {
            std::array <int, 26> shoeArray = {2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11};
            shoe.assign(&shoeArray[0], &shoeArray[0] + shoeArray.size());
        } else if (::decks == 1) {
            std::array <int, 52> shoeArray = {2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11};
            shoe.assign(&shoeArray[0], &shoeArray[0] + shoeArray.size());
        } else if (::decks == 2) {
            std::array <int, 104> shoeArray = {2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11};
            shoe.assign(&shoeArray[0], &shoeArray[0] + shoeArray.size());
        } else if (::decks == 3) {
            std::array <int, 156> shoeArray = {2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11};
            shoe.assign(&shoeArray[0], &shoeArray[0] + shoeArray.size());
        } else if (::decks == 4) {
            std::array <int, 208> shoeArray = {2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11};
            shoe.assign(&shoeArray[0], &shoeArray[0] + shoeArray.size());
        } else if (::decks == 5) {
            std::array <int, 260> shoeArray = {2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11};
            shoe.assign(&shoeArray[0], &shoeArray[0] + shoeArray.size());
        } else if (::decks == 6) {
            std::array <int, 312> shoeArray = {2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11};
            shoe.assign(&shoeArray[0], &shoeArray[0] + shoeArray.size());
        } else if (::decks == 7) {
            std::array <int, 364> shoeArray = {2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11};
            shoe.assign(&shoeArray[0], &shoeArray[0] + shoeArray.size());
        } else if (::decks == 8) {
            std::array <int, 416> shoeArray = {2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11};
            shoe.assign(&shoeArray[0], &shoeArray[0] + shoeArray.size());
        } else cout << "I'm sorry, but we only offer -2,1,2,3,4,5,6,7, and 8 deck blackjack.\n 1 deck blackjack will be chosen by default." << endl;
        cout << endl;
        if (decks == -2) {
            cout << "'-2' indicates half a deck is being used.\n This speeds up simulations." << endl;
        }


        while (decideH17 == false) {
            char H17 = '0';
            cout << "Enter '1' for dealer hits on soft 17 or \n Enter '0' for dealer stands on soft 17." << endl;
            cin >> H17;
            //cout << endl;
            if (H17 == '1') {
                cout << "H17" << endl;
                ::H17 = true;
                decideH17 = true;
            } else if (H17 == '0') {
                cout << "S17" << endl;
                ::H17 = false;
                decideH17 = true;
            } else {
                cout << "error with char/bool dealer H17" << endl;
            }
            cout << endl;
        }


        while (decidePeeking == false) {
            char peeking = '0';
            cout << "Enter '1' for dealer-peeks-at-hole-card blackjack or \n Enter '0' to play with no dealer peeking." << endl;
            cin >> peeking;
            if (peeking == '1') {
                cout << "peeking" << endl;
                ::peek = true;
                decidePeeking = true;
            } else if (peeking == '0') {
                cout << "no-peeking" << endl;
                ::peek = false;
                decidePeeking = true;
            } else {
                cout << "error char/bool peeking" << endl;
            }
        }

        std::random_shuffle(shoe.begin(), shoe.end(), ::myrandom);
        std::random_shuffle(shoe.begin(), shoe.end());
        simShoe.assign(&shoe[0], &shoe[0] + shoe.size());

        // Start a new_Hand after standing or busting.
newHand:

        // Clear all player and dealer variables:
        player.hand.clear();
        player.simHand.clear();
        dealer.hand.clear();
        dealer.simHand.clear();
        dealer.score = 0;
        dealer.simScore = 0;
        player.score = 0;
        player.simScore = 0;
        dealer.numAces = 0;
        dealer.simNumAces = 0;
        dealer.numHdAces = 0;
        dealer.simNumHdAces = 0;
        player.numAces = 0;
        player.simNumAces = 0;
        player.numHdAces = 0;
        player.simNumHdAces = 0;

        // Decide how much money to bet:

        while (decideBet == false) {
            cout << "Your bank has $ " << ::bank << endl;
            cout << "Enter your bet." << endl;
            cin >> ::bet;
            cout << endl;

            if (::bet == 0) {
                cout << "You cannot bet zero." << endl;
            } else if (::bet < 0) {
                cout << "You cannot bet negatives." << endl;
            } else if (::bet > bank) {
                cout << "too high" << endl;
            } else {
                decideBet = true;
            }
        }

        //Player draws the first two face up cards into real and virtual hands:
        player.drawCard(shoe);
        player.simDrawCard(::simShoe, (int) shoe.size());

        player.drawCard(shoe);
        player.simDrawCard(::simShoe, (int) shoe.size());

        // Dealer draws one face up card.
        dealer.drawCard(shoe);
        dealer.simDrawCard(::simShoe, (int) shoe.size());

        holePosition = shoe.size() - 1;

        // The hole card does NOT got into the virtual hand. 
        //The simulator does not know the hole card.
        dealer.drawCard(shoe);

        hole = simShoe.at(holePosition);

        // if insured is true, the bet has insurance that pays 2:1


        //Insurance only availible if the dealer can peek at the hole card. Stops "Early Surrender".
        if (dealer.hand.size() == 2 && ::peek == true) {
            if (dealer.hand.at(0) == 11) {
                cout << "The dealer reveals an ace" << endl;
                cout << "You have a hand of: ";
                for (unsigned int h = 0; h < player.hand.size(); h++) {
                    cout << player.hand.at(h) << " ";
                }
                cout << "." << endl;

                cout << "The number of undrawn 10's remaining is: " << endl;
                int tens = 0;
                for (unsigned int y = 0; y < ::simShoe.size(); y++) {
                    if (::simShoe.at(y) == 10) tens++;
                }
                cout << tens << endl;

                cout << "The number of unknown cards remaining is: " << endl;
                cout << (shoe.size() - 1) << endl;

                cout << "Press '4' to make a 2:1 insurance bet of half your origional bet.";
                cout << "Press any other key to continue.";
                char ins = '0';
                cin >> ins;
                if (ins == '4' && ::bank >= (::bet * 3) / 2) {
                    insured = true;
                } else if (::bank < (::bet * 3) / 2) {
                    cout << "You can't afford an insurance bet." << endl;
                    insured = false;
                } else {
                    insured = false;
                }
            }

            if (dealer.score == 21 && insured == false) {
                cout << "The dealer has: " << dealer.hand.at(0) << ' ' << dealer.hand.at(1) << endl;
                cout << "A natural blackjack." << endl;
                if (player.score == 21) cout << "You push." << endl;
                else {
                    cout << "You lose your bet" << endl;
                    bank = bank - bet;
                }

                if (bank <= 0) {
                    cout << "You leave broke";
                    cout << "Press any key to quit." << endl;
                    char f = 'l';
                    cin >> f;
                    return 0;
                } else {
                    dealer.simDrawCard(::simShoe, holePosition);
                    goto newHand;
                }
            } else if (dealer.score != 21 && insured == false) {
                cout << "The dealer has a " << dealer.hand.at(0) << ',' << endl;
                cout << "But the dealer doesn't have blackjack." << endl;
            } else if (dealer.score == 21 && insured == true) {
                cout << "The dealer has: " << dealer.hand.at(0) << ' ' << dealer.hand.at(1) << endl;
                cout << "Your insurance stops the dealer's blackjack\n from affecting your piggy bank.";
                dealer.simDrawCard(::simShoe, holePosition);
                goto newHand;
            } else if (dealer.score != 21 && insured == true) {


                cout << "No dealer blackjack.\n You lose half your origional bet." << endl;
                bank = bank - (bet / 2);
                if (bank <= 0) {
                    cout << "You leave broke" << endl;
                    cout << "Press any key to quit." << endl;
                    char f = 'l';
                    cin >> f;
                    return 0;
                }
            }
        }

        // The player makes a new hit/stand decision based on the statistical analysis at the decide point.
decide:

        cout << "The dealer's faceup card is " << dealer.hand.at(0) << endl;
        cout << "You have a hand of: ";
        cout.setf(ios::fixed);
        std::cout.precision(5);
        for (unsigned int h = 0; h < player.hand.size(); h++) {
            cout << player.hand.at(h) << " ";
        }
        cout << "." << endl;

        // If there are too many decks in the shoe, the simulation analyzer will take too long.
        if (::decks < 4) {
            cout << "With this configuration, you will gain or lose an average of: $";
            cout << player.analyzeStand(dealer, player);
            ::gains = 0.0;
            ::j = 0;
            cout << "\n if you stand." << endl;
        }

        if (::decks < 3) {
            cout << "Also, you will gain or lose an average of: $";
            cout << player.analyzeHit(dealer, player);
            ::gains = 0.0;
            ::j = 0;
            cout << "\n if you hit once and then stand." << endl;
        }

        // Looking 2 moves ahead takes the most time. Especially for low cards.
        if (::decks < 2 && dealer.simHand.at(0) > 6 && dealer.simNumAces == 0) {
            cout << "Finally, you will gain or lose an average of $";
            cout << player.analyzeHitHit(dealer, player);
            ::gains = 0.0;
            ::j = 0;
            cout << "\n if you hit once and then \n hit or stand." << endl;
        }

        // The player chooses an action: stand, hit, split, or surrender.
chooseAction:

        decideNewDeck = true;
        if (shoe.size() < 13) {
            cout << "The dealer reaches a cut card, then replaces and shuffles the near-empty deck " << endl;
            decideNewDeck = false;
            decideH17 = false;
            decidePeeking = false;
            decideBet = false;
            insured = false;
        }
    }

    cout << "Press '0' to stand, '1' to hit, '2' to double down,\n '3' to split, or '5' to late surrender." << endl;
    cin >> g;
    cout << '\n' << endl;

    if (g == '0') {
    } else if (g == '1') {
        player.drawCard(shoe);
        player.simDrawCard(::simShoe, (int) shoe.size());

        cout << "You drew a " << player.hand.back() << endl;
        if (player.score <= 21) {
            goto decide;
        }
    } else if (g == '2') {
        if (player.hand.size() > 2) {
            cout << "You can only double down on a new or split hand" << endl;
            goto decide;
        }

        bool decideDoubleD = true;
        while (decideDoubleD == true) {
            int doubleD = 0;
            cout << "How many dollars would you like to double down with? ";
            cin >> doubleD;
            if ((bet + doubleD) > ::bank) {
                cout << "You don't have enough money." << endl;
            } else if (doubleD > (2 * ::bet)) {
                cout << "You can't double down more than your wager." << endl;
            }
            else {
                ::bet = ::bet + doubleD;
                decideDoubleD = false;
            }
        }
        player.drawCard(shoe);
        player.simDrawCard(::simShoe, (int) shoe.size());
        cout << "You drew a " << player.hand.back() << endl;
    } else if (g == '3') {

        if (dealer.hand.size() != 2) {
            cout << "Sorry, you can only split with two cards in your hand." << endl;
            goto chooseAction;
        } else if (::bank < 2 * ::bet) {
            cout << "Sorry, but you are too poor to split your entire bet" << endl;
            goto chooseAction;
        }
            // You can only split cards if your 0 index card is equal in value to your 1 index card.
        else if (player.hand[0] == player.hand[1]) {
            cout << "You separate off one of your two equal valued cards and \n" << endl;

            player.score -= player.hand.back();
            player.simScore -= dealer.simHand.back();

            player.hand.pop_back();
            player.simHand.pop_back();

            cout << "You place a bet equal to your origional bet." << endl;
            ::bet = ::bet * 2;
            if (player.hand.at(0) == 11) {
                cout << "You only get one hit on a split ace.";
                player.drawCard(shoe);
                player.simDrawCard(::simShoe, (int) shoe.size());
                cout << "You drew a " << player.hand.back() << endl;
            } else {
                cout << "And then you play as usual." << endl;
                goto decide;
            }
        }
        else {
            cout << "Sorry, you can only split with equal valued cards" << endl;
            goto chooseAction;
        }
    } else if (g == '5') {
        if (player.hand.size() != 2) {
            cout << "You can only surrender when you have 2 cards in your hand" << endl;
            goto chooseAction;
        } else {
            cout << "You give up your hand and half your bet. \n New hand" << endl;
            ::bank = bank - (bet / 2);
            // Dispose of the hole card
            cout << "The hole card was a " << hole << endl;
            if (dealer.simHand.size() == 1) {
                dealer.simDrawCard(::simShoe, holePosition);
            }
            goto newHand;
        }
    } else {
        cout << "Invalid decision." << endl;
        goto chooseAction;
    }

    if (simShoe.at(::holePosition) != 0) {
        // Remove hole card. Now the virtual and real decks are the same for the next round.
        dealer.simDrawCard(::simShoe, holePosition);
    }

    bool recalculate = false;
    do {
        // Dealer hits on 16 or below
        if (dealer.score <= 16) {
            // The dealer must draw go into the next level of simulation and draw another card.
            dealer.drawCard(shoe);
            dealer.simDrawCard(::simShoe, (int) shoe.size());
            recalculate = true;
        }

            // Dealer hits on a soft 17 (6, 11)
        else if ((dealer.score == 17) && (::H17 == true) && (dealer.simNumAces > 0)) {
            // The dealer must draw go into the next level of simulation and draw another card.
            dealer.drawCard(shoe);
            dealer.simDrawCard(::simShoe, (int) shoe.size());
            recalculate = true;

        }
        else if ((dealer.score == 21 && dealer.hand.size() == 2) && (player.score == 21 && player.hand.size() == 2)) {
            cout << "The dealer's hand is:";
            for (unsigned int p = 0; p < dealer.hand.size(); p++) {
                cout << ' ' << dealer.hand.at(p);
            }
            cout << '.' << endl;
            cout << "You both drew BlackJacks. Push." << endl;
            goto newHand;
        } else if ((dealer.score == 21 && dealer.hand.size() == 2)) {
            cout << "The dealer's hand is:";
            for (unsigned int p = 0; p < dealer.hand.size(); p++) {
                cout << ' ' << dealer.hand.at(p);
            }
            cout << '.' << endl;
            cout << "Dealer blackjack." << endl;
            bank -= bet;
            if (bank <= 0) {
                cout << "You leave broke" << endl;
                cout << "Press any key to quit." << endl;
                char f = 'l';
                cin >> f;
                return 0;
            } else goto newHand;
        } else if ((player.score == 21 && player.hand.size() == 2) && (dealer.score >= 17)) {
            cout << "The dealer's hand is:";
            for (unsigned int p = 0; p < dealer.hand.size(); p++) {
                cout << ' ' << dealer.hand.at(p);
            }
            cout << '.' << endl;
            cout << "But you have a BlackJack! 3:2!" << endl;
            bank += (3 * bet) / 2;
            goto newHand;
        } else if (player.score > 21) {
            cout << "The dealer's hand is:";
            for (unsigned int p = 0; p < dealer.hand.size(); p++) {
                cout << ' ' << dealer.hand.at(p);
            }
            cout << '.' << endl;
            cout << "and you busted" << endl;
            bank -= bet;
            if (bank <= 0) {
                cout << "You leave broke" << endl;
                cout << "Press any key to quit." << endl;
                char f = 'l';
                cin >> f;
                return 0;
            } else goto newHand;
        } else if ((dealer.score >= 17) && dealer.score == player.simScore) {
            cout << "The dealer's hand is:";
            for (unsigned int p = 0; p < dealer.hand.size(); p++) {
                cout << ' ' << dealer.hand.at(p);
            }
            cout << '.' << endl;
            cout << "You tied with the dealer." << endl;
            goto newHand;
        } else if (dealer.score > 21) {
            cout << "The dealer's hand is:";
            for (unsigned int p = 0; p < dealer.hand.size(); p++) {
                cout << ' ' << dealer.hand.at(p);
            }
            cout << '.' << endl;
            cout << "Dealer bust" << endl;
            bank += bet;
            goto newHand;
        } else if ((dealer.score >= 17) && dealer.score <= 21 && (player.score < dealer.score)) {
            cout << "The dealer's hand is:";
            for (unsigned int p = 0; p < dealer.hand.size(); p++) {
                cout << ' ' << dealer.hand.at(p);
            }
            cout << '.' << endl;
            cout << "You lose the hand." << endl;
            bank -= bet;
            if (bank <= 0) {
                cout << "You leave broke" << endl;
                cout << "Press any key to quit." << endl;
                char f = 'l';
                cin >> f;
                return 0;
            }
            goto newHand;
        } else if ((dealer.score >= 17) && dealer.score <= 21 && (player.score > dealer.score)) {
            cout << "The dealer's hand is:";
            for (unsigned int p = 0; p < dealer.hand.size(); p++) {
                cout << ' ' << dealer.hand.at(p);
            }
            cout << '.' << endl;
            cout << "You drew higher than the dealer." << endl;
            bank += bet;
            goto newHand;
        } else {
            cout << "error at end of calculation." << endl;
        }
    } while (recalculate == true);

}



