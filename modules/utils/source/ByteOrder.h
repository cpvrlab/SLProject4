/**
 * \file      ByteOrder.h
 * \date      January 2021
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLPROJECT_BYTEORDER_H
#define SLPROJECT_BYTEORDER_H

#include <cstdint>
#include <climits>
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
//! Abort compilation if a char has not 8 bits, as functions for this case aren't implemented yet
#if CHAR_BIT != 8
#    error "Byte order functions require that the machine has a char width of 8 bit"
#endif
//-----------------------------------------------------------------------------
//! Utility functions for functions related to byte order conversions
namespace ByteOrder
{
//-----------------------------------------------------------------------------
void toBigEndian16(uint16_t src, char* dest);
void toBigEndian32(uint32_t src, char* dest);
void toBigEndian64(uint64_t src, char* dest);
//-----------------------------------------------------------------------------
void writeBigEndian16(uint16_t number, std::ostream& stream);
void writeBigEndian32(uint32_t number, std::ostream& stream);
void writeBigEndian64(uint64_t number, std::ostream& stream);
//-----------------------------------------------------------------------------
} // namespace ByteOrder
//-----------------------------------------------------------------------------

#endif // SLPROJECT_BYTEORDER_H
