# 15puzzle

This is a solver for the generalized 15-puzzle written in C++.

It is a fork of https://github.com/MichaelKim/15puzzle, with a few additions:
* Use of a forbidden word dictionary to prune duplicate nodes in the IDA* search. This reduces the branching factor of the dfs.
* The multiprocessing flag `-p` uses BFS+IDA* search to find a solution.

See [blog post](https://domslee.com/npuzzle/pruning/dfs/2021/11/25/solving-the-24-puzzle.html).

## Suggested Improvements

These are implementations of Richard E. Korf's work. There are more ideas on better hybrid searches in his recent papers:

* (2021): A*+BFHS: A Hybrid Heuristic Search Algorithm.
* (2019): A*+IDA*: A Simple Hybrid Search Algorithm.

This implementation uses BFS+IDA*, which is a slower version of A*+IDA*.

## Building

Use `make -j`.

## Usage

```
Syntax:
    puzzle [OPTIONS]

Options:
    -b <file>
        Board files
    -d <file>
        Use database file
    -h, --help
        Print this help
    -i, --interactive
        Show a playback of each solution
    -f, --fsmDepthLimit
        Specify a depth limit for constructing the FSM
    --fsmFile
        Specify a file of forbidden words to use for the FSM
    -e, --evaluateBranchFactor
        Evaluate the branching factor of an exhaustive DFS
    -i, --interactive
        Show a playback of each solution
    -p
        Use parallel BFS + IDA* search
```


## Profiling
May need `libprocps-dev`.

https://gperftools.github.io/gperftools/cpuprofile.html

```
pprof --web bin/puzzle out.prof
~/go/bin/pprof -top ./bin/puzzle prof/out.prof
```
