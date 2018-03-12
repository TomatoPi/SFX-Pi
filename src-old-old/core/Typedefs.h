/**********************************************************************
 * @file Typedefs.h
 * @author TomatoPi
 * @version 1.0
 *
 * File containing all variables redifinitions for the program
 **********************************************************************/
#ifndef DEF_TYPEDEFS_H
#define DEF_TYPEDEFS_H

#include <jack/jack.h>
#include <vector>
#include <string>

/**
 * ID on 1 octet, between 0x00 and 0xff
 **/
typedef uint8_t id1_t;

/**
 * ID on 2 octets, between 0x0000 and 0xffff
 **/
typedef uint16_t id2_t;

/**
 * Hexadicimal value representating a register value
 **/
typedef unsigned char hex_t;

/**
 * Simplify jack's sample_t writing
 **/
typedef jack_default_audio_sample_t sample_t;

/**
 * Binary flag used inside preset files
 **/
typedef uint8_t flag_t;

/**
 * Garantee that size_t is the same on lubuntu and archlinux
 **/
typedef uint32_t usize_t;

#endif
