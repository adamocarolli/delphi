#!/usr/bin/env python

"""Command line interface for delphi.

Why does this file exist, and why not put this in ``__main__``? You might be
tempted to import things from ``__main__`` later, but that will cause
problems--the code will get executed twice:

- When you run ``python3 -m delphi`` python will execute``__main__.py`` as a
  script. That means there won't be any ``delphi.__main__`` in ``sys.modules``.
- When you import __main__ it will get executed again (as a module) because
  there's no ``delphi.__main__`` in ``sys.modules``.

.. seealso:: http://click.pocoo.org/5/setuptools/#setuptools-integration
"""

import os
import sys
import pickle
from pathlib import Path
from typing import Any
from functools import partial
from argparse import (
    ArgumentParser,
    ArgumentTypeError,
    FileType,
    ArgumentDefaultsHelpFormatter,
)
from delphi.AnalysisGraph import AnalysisGraph


def _write_latent_state(G, f):
    for n in G.nodes(data=True):
        f.write(f"{str(G.t)},")
        f.write(",".join([n[0]] + [str(v) for v in n[1]["rv"].dataset]) + "\n")


def execute(args):
    from pandas import read_csv

    print("Executing model")
    with open(args.input_dressed_cag, 'rb') as f:
        G = pickle.load(f)

    G.assemble_transition_model_from_gradable_adjectives()
    G.sample_from_prior()
    G.initialize(args.input_variables)
    with open(args.output_sequences, "w") as f:
        f.write(
            ",".join(
                ["seq_no", "variable"]
                + [f"sample_{str(i)}" for i in range(1, G.res + 1)]
            )
            + "\n"
        )

        for t in range(args.steps):
            G.update()
            _write_latent_state(G, f)


def positive_real(arg, x):
    try:
        val = float(x)
    except ValueError:
        raise ArgumentTypeError(
            f"{arg} should be a positive real number (you entered {x})."
        )

    if not val > 0.0:
        raise ArgumentTypeError(
            f"{arg} should be a positive real number (you entered {x})."
        )
    return x


def positive_int(arg, x):
    try:
        val = int(x)
    except ValueError:
        raise ArgumentTypeError(
            f"{arg} should be a positive integer (you entered {x})."
        )

    if not val > 0:
        raise ArgumentTypeError(
            f"{arg} should be a positive integer (you entered {x})."
        )

    return val


def main():
    """Run the CLI."""
    parser = ArgumentParser(
        description="Dynamic Bayes Net Executable Model",
        formatter_class=ArgumentDefaultsHelpFormatter,
    )

    def add_flag(short_arg: str, long_arg: str, help: str):
        parser.add_argument(
            "-" + short_arg, "--" + long_arg, help=help, action="store_true"
        )

    subparsers = parser.add_subparsers()

    parser_execute = subparsers.add_parser("execute", help="Model execution")
    parser_execute.set_defaults(func=execute)

    # ==========================================================================
    # Model execution options
    # ==========================================================================

    parser_execute.add_argument(
        "--input_dressed_cag",
        help="Path to the input dressed cag",
        type=str,
        default="delphi_model.pkl",
    )

    parser_execute.add_argument(
        "--steps",
        help="Number of time steps to take",
        type=partial(positive_int, "steps"),
        default=5,
    )

    parser_execute.add_argument(
        "--samples",
        help="Number of sequences to sample",
        type=partial(positive_int, "samples"),
        default=100,
    )

    parser_execute.add_argument(
        "--output_sequences",
        help="Output file containing sampled sequences",
        type=str,
        default="dbn_sampled_sequences.csv",
    )

    parser_execute.add_argument(
        "--input_variables",
        help="Path to the variables of the input dressed cag",
        type=str,
        default="bmi_config.txt",
    )

    args = parser.parse_args()

    if len(sys.argv) == 1:
        parser.print_help()

    else:
        args.func(args)


if __name__ == "__main__":
    main()
