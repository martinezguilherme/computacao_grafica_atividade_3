#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>

//enum class Input { SpeedUp, SpeedDown, TurnLeft, TurnRight, NoseUp , NoseDown, RotateLeft, RotateRight};
enum class State { Playing, GameOver, Win };

struct GameData {
  State m_state{State::Playing};
  //std::bitset<8> m_input;  // [throtle up/down, left, right]
};

#endif