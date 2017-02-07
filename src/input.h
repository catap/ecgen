/*
 * ecgen, tool for generating Elliptic curve domain parameters
 * Copyright (C) 2017 J08nY
 */
#ifndef ECGEN_INPUT_H
#define ECGEN_INPUT_H

#include "random.h"

typedef enum PARAM {
	PARAM_PRIME,
	PARAM_INT,
	PARAM_SHORT,
	PARAM_STRING
} param_t;

/**
 *
 * @param stream
 * @param prompt
 * @param bits
 * @param delim
 * @return
 */
GEN fread_prime(FILE *stream, const char *prompt, long bits, int delim);

/**
 *
 * @param stream
 * @param prompt
 * @param bits
 * @param delim
 * @return
 */
GEN fread_int(FILE *stream, const char *prompt, long bits, int delim);

/**
 *
 * @param stream
 * @param prompt
 * @param delim
 * @return
 */
GEN fread_short(FILE *stream, const char *prompt, int delim);

GEN fread_string(FILE *stream, const char *prompt, int delim);

GEN fread_param(param_t param, FILE *stream, const char *prompt, long bits,
                int delim);

GEN read_param(param_t param, const char *prompt, long bits, int delim);

extern FILE *in;

FILE *input_open(const char *input);

void input_close(FILE *in);

#endif  // ECGEN_INPUT_H
