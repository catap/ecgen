/*
 * ecgen, tool for generating Elliptic curve domain parameters
 * Copyright (C) 2017 J08nY
 */
#include "cli.h"
#include <string.h>
#include <unistd.h>
#include "config.h"

char cli_doc[] =
    "ecgen, tool for generating Elliptic curve domain parameters.\v(C) 2017 "
    "Eastern Seaboard Phishing Authority";
char cli_args_doc[] = "bits";

enum opt_keys {
	OPT_DATADIR = 'd',
	OPT_COUNT = 'c',
	OPT_PRIME = 'p',
	OPT_COFACTOR = 'k',
	OPT_RANDOM = 'r',
	OPT_SEED = 's',
	OPT_INVALID = 'i',
	OPT_ORDER = 'n',
	OPT_KOBLITZ = 'K',
	OPT_UNIQUE = 'u',
	OPT_FORMAT = 't',
	OPT_OUTPUT = 'o',
	OPT_INPUT = 'f',
	OPT_APPEND = 'a',
	OPT_VERBOSE = 'v',
	OPT_MEMORY = 'm',
	OPT_FP = 1,
	OPT_F2M,
	OPT_POINTS,
	OPT_THREADS,
	OPT_TSTACK,
	OPT_ANOMALOUS
};

// clang-format off
struct argp_option cli_options[] = {
	{0,              0,             0,        0,                 "Field specification:",                                                                  1},
	{"fp",           OPT_FP,        0,        0,                 "Prime field.",                                                                          1},
	{"f2m",          OPT_F2M,       0,        0,                 "Binary field.",                                                                         1},

	{0,              0,             0,        0,                 "Generation options:",                                                                   2},
	{"random",       OPT_RANDOM,    0,        0,                 "Generate a random curve (using Random approach).",                                      2},
	{"prime",        OPT_PRIME,     0,        0,                 "Generate a curve with prime order.",                                                    2},
	{"cofactor",     OPT_COFACTOR,  "BOUND",  0,                 "Generate a curve with cofactor up to BOUND.",                                           2},
	{"koblitz",      OPT_KOBLITZ,   0,        0,                 "Generate a Koblitz curve (a = 0).",                                                     2},
	{"unique",       OPT_UNIQUE,    0,        0,                 "Generate a curve with only one generator.",                                             2},
	{"anomalous",    OPT_ANOMALOUS, 0,        0,                 "Generate an anomalous curve (of trace one, with field order equal to curve order).",    2},
	{"points",       OPT_POINTS,    "TYPE",   0,                 "Generate points of given type (random/prime/all/none).",                                    2},
	{"seed",         OPT_SEED,      "SEED", OPTION_ARG_OPTIONAL, "Generate a curve from SEED (ANSI X9.62 verifiable procedure). **NOT IMPLEMENTED**",     2},
	{"invalid",      OPT_INVALID,   0,        0,                 "Generate a set of invalid curves, for a given curve (using Invalid curve algorithm).",  2},
	{"order",        OPT_ORDER,     "ORDER",  0,                 "Generate a curve with given order (using Complex Multiplication). **NOT IMPLEMENTED**", 2},
	{"count",        OPT_COUNT,     "COUNT",  0,                 "Generate multiple curves.",                                                             2},

	{0,              0,             0,        0,                 "Input/Output options:",                                                                 3},
	{"format",       OPT_FORMAT,    "FORMAT", 0,                 "Format to output in. One of [csv,json], default is json.",                              3},
	{"input",        OPT_INPUT,     "FILE",   0,                 "Input from file.",                                                                      3},
	{"output",       OPT_OUTPUT,    "FILE",   0,                 "Output into file. Overwrites any existing file!",                                       3},
	{"append",       OPT_APPEND,    0,        0,                 "Append to output file (don't overwrite).",                                              3},
	{"verbose",      OPT_VERBOSE,   "FILE", OPTION_ARG_OPTIONAL, "Verbose logging (to stdout or file).",                                                  3},

	{0,              0,             0,        0,                 "Other:",                                                                                4},
	{"data-dir",     OPT_DATADIR,   "DIR",    0,                 "Set PARI/GP data directory (containing seadata package).",                              4},
	{"memory",       OPT_MEMORY,    "SIZE",   0,                 "Use PARI stack of SIZE (can have suffix k/m/g).",                                       4},
	{"threads",      OPT_THREADS,   "NUM",    0,                 "Use NUM threads.",                                                                      4},
	{"thread-stack", OPT_TSTACK,    "SIZE",   0,                 "Use PARI stack of SIZE (per thread, can have suffix k/m/g).",                           4},
	{0}
};
// clang-format on

static unsigned long cli_parse_memory(const char *str) {
	char *suffix = NULL;
	unsigned long read = strtoul(str, &suffix, 10);
	if (suffix) {
		if (*suffix == 'k' || *suffix == 'K') {
			read *= 1000;
		} else if (*suffix == 'm' || *suffix == 'M') {
			read *= 1000000;
		} else if (*suffix == 'g' || *suffix == 'G') {
			read *= 1000000000;
		}
	}
	return read;
}

error_t cli_parse(int key, char *arg, struct argp_state *state) {
	config_t *cfg = state->input;

	switch (key) {
		case OPT_DATADIR:
			cfg->datadir = arg;
			break;
		case OPT_MEMORY:
			cfg->memory = cli_parse_memory(arg);
			break;
		case OPT_TSTACK:
			cfg->thread_memory = cli_parse_memory(arg);
			break;
		case OPT_THREADS:
			if (!strcmp(arg, "auto") || !strcmp(arg, "AUTO")) {
				long nprocs = sysconf(_SC_NPROCESSORS_ONLN);
				if (nprocs > 0) {
					cfg->threads = (unsigned long)nprocs;
				}
			} else {
				cfg->threads = strtoul(arg, NULL, 10);
				if (!cfg->threads) {
					argp_failure(state, 1, 0,
					             "Invalid number of threads specified.");
				}
			}
			break;
		case OPT_COUNT:
			cfg->count = strtoul(arg, NULL, 10);
			break;
		case OPT_FORMAT:
			if (!strcmp(arg, "csv")) {
				cfg->format = FORMAT_CSV;
			} else if (!strcmp(arg, "json")) {
				cfg->format = FORMAT_JSON;
			} else {
				argp_failure(state, 1, 0,
				             "Invalid format specified. One of [csv, json] "
				             "is valid.");
			}
			break;
		case OPT_INPUT:
			cfg->input = arg;
			break;
		case OPT_OUTPUT:
			cfg->output = arg;
			break;
		case OPT_APPEND:
			cfg->append = true;
			break;
		case OPT_VERBOSE:
			cfg->verbose++;
			if (arg) {
				cfg->verbose_log = arg;
			}
			break;
		case OPT_RANDOM:
			cfg->random = true;
			break;
		case OPT_PRIME:
			cfg->prime = true;
			break;
		case OPT_COFACTOR:
			cfg->cofactor = true;
			cfg->cofactor_bound = strtol(arg, NULL, 10);
			break;
		case OPT_INVALID:
			cfg->invalid = true;
			break;
		case OPT_ORDER:
			cfg->cm = true;
			if (arg) {
				cfg->order = arg;
			}
			break;
		case OPT_KOBLITZ:
			cfg->koblitz = true;
			break;
		case OPT_UNIQUE:
			cfg->unique = true;
			break;
		case OPT_ANOMALOUS:
			cfg->anomalous = true;
			break;
		case OPT_POINTS: {
			char *num_end;
			long amount = strtol(arg, &num_end, 10);
			cfg->points.amount = (size_t)amount;
			if (strstr(num_end, "random")) {
				cfg->points.type = POINTS_RANDOM;
			} else if (strstr(num_end, "prime")) {
				cfg->points.type = POINTS_PRIME;
			} else if (strstr(num_end, "all")) {
				cfg->points.type = POINTS_ALL;
			} else if (strstr(num_end, "none")) {
				cfg->points.type = POINTS_NONE;
			} else {
				argp_failure(state, 1, 0, "Unknown point type. %s", num_end);
			}
			break;
		}
		case OPT_SEED:
			cfg->from_seed = true;
			if (arg) {
				// ANSI X9.62 specifies seed as at least 160 bits in length.
				if (strlen(arg) < 20) {
					argp_failure(
					    state, 1, 0,
					    "SEED must be at least 160 bits (20 characters).");
				}
				cfg->seed = arg;
			}
			break;
		case OPT_FP:
			cfg->field = FIELD_PRIME;
			cfg->prime_field = true;
			break;
		case OPT_F2M:
			cfg->field = FIELD_BINARY;
			cfg->binary_field = true;
			break;
		case ARGP_KEY_ARG:
			if (state->arg_num >= 1) {
				argp_usage(state);
			}

			cfg->bits = strtoul(arg, NULL, 10);
			cfg->hex_digits =
			    2 * (cfg->bits / 8 + (cfg->bits % 8 != 0 ? 1 : 0));
			break;
		case ARGP_KEY_END:
			// validate all option states here.
			// Only one field
			if (!cfg->prime_field && !cfg->binary_field) {
				argp_failure(state, 1, 0,
				             "Specify field type, prime or binary, with --fp / "
				             "--f2m (but not both).");
			}
			// Invalid is not prime or seed by definition.
			if (cfg->invalid &&
			    (cfg->prime || cfg->from_seed || cfg->cofactor)) {
				// not seed, not prime
				argp_failure(state, 1, 0,
				             "Invalid curve generation can not generate curves "
				             "from seed, exhaustive or prime order.");
			}
			if (cfg->cm && (cfg->prime || cfg->from_seed || cfg->invalid ||
			                cfg->cofactor || cfg->anomalous)) {
				argp_failure(state, 1, 0,
				             "Fixed order curve generation can not generate "
				             "curves from seed, or invalid curves. Prime order "
				             "also doesn't make sense if the given one isn't "
				             "prime.");
			}
			if (cfg->anomalous &&
			    (cfg->binary_field || cfg->cofactor || cfg->from_seed ||
			     cfg->cm || cfg->invalid || cfg->koblitz)) {
				argp_failure(
				    state, 1, 0,
				    "Anomalous curve generation can not generate "
				    "binary field curves, curves with a cofactor, from seed "
				    "with fixed order, invalid or Koblitz curves.");
			}

			// default values
			if (!cfg->count) {
				cfg->count = 1;
			}
			if (!cfg->memory) {
				cfg->memory = 1000000000;
			}
			if (!cfg->threads) {
				cfg->threads = 1;
			}
			if (!cfg->thread_memory) {
				cfg->thread_memory = cfg->bits * 2000000;
			}
			break;
		case ARGP_KEY_NO_ARGS:
			argp_usage(state);
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

char *cli_filter(int key, const char *text, void *input) {
	return (char *)text;
}
