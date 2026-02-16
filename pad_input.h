
// ========================================================================
//    Name : pad input (pad_input.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/01/13
// ========================================================================
#ifndef PAD_INPUT_H
#define PAD_INPUT_H

#include <cstdint>

void Pad_Update();
void Pad_Reset();

bool Pad_IsPressed(std::uint16_t button);
bool Pad_IsTrigger(std::uint16_t button);

#endif // !PAD_INPUT_H
