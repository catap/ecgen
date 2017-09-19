/*
 * ecgen, tool for generating Elliptic curve domain parameters
 * Copyright (C) 2017 J08nY
 */
/**
 * @file anomalous.h
 */
#ifndef ECGEN_ANOMALOUS_H
#define ECGEN_ANOMALOUS_H

#include <pari/pari.h>
#include "arg.h"
#include "io/cli.h"
#include "misc/types.h"

typedef struct disc_t {
	GEN d;
	GEN j;
	GEN alpha;
} disc_t;

/**
 * GENERATOR(gen_t)
 *
 * @param curve A curve_t being generated
 * @param cfg An application config
 * @param args the index of the discriminant to use, in the disc_table
 * @return state diff
 */
GENERATOR(anomalous_gen_field);

/**
 * GENERATOR(gen_t)
 *
 * @param curve A curve_t being generated
 * @param cfg An application config
 * @param args the index of the discriminant to use, in the disc_table
 * @return state diff
 */
GENERATOR(anomalous_gen_equation);

/**
 * GENERATOR(gen_t)
 *
 * @param curve A curve_t being generated
 * @param cfg An application config
 * @param args unused
 * @return state diff
 */
GENERATOR(anomalous_gen_order);

/**
 * @brief Initialize anomalous generation, allocate and set the disc_table.
 */
void anomalous_init();

/**
 * @brief Deinitialize anomalous generation, free the discriminants from the
 * disc_table.
 */
void anomalous_quit();

#endif  // ECGEN_ANOMALOUS_H
