# A simple script to run teh different interpreters/JITs and create some
# plots how they perform.
import time
import subprocess
import pandas as pd
import concurrent.futures
import os

PROGRAMS = ["brainint", "brainbyte", "braindyn"]
BENCHMARKS = ["helloworld.bf", "99bottles.bf", "mandelbrot.bf", "hanoi.bf"]


def run(target):
    executable = "build/" + target[0]
    program = "examples/" + target[1]

    start = time.time_ns()
    subprocess.run([executable, program], capture_output=True)
    elapsed_ms = (time.time_ns() - start) / 1000 / 1000
    return elapsed_ms


def run_multiple(target, n):
    print(f"Running {target[1]} on {target[0]} ...\t", end="", flush=True)
    with concurrent.futures.ProcessPoolExecutor(max_workers=n) as executor:
        # Start the load operations and mark each future with its URL
        result = executor.map(run, [target for _ in range(n)])
    elapsed_ms = min(result)
    print(f" [{elapsed_ms:.2f}ms]")
    return elapsed_ms


def plot(results):
    # Create the correct data format
    data = []
    for benchmark in BENCHMARKS:
        out = [benchmark]
        interpreter_ms = results[benchmark]["brainint"]
        for program in PROGRAMS:
            out.append(results[benchmark][program] / interpreter_ms)
        data.append(out)
    df = pd.DataFrame(
        data,
        columns=["Programs"] + PROGRAMS,
    )

    # Create the plot
    ax = df.plot(
        x="Programs",
        kind="bar",
        stacked=False,
        title="Benchmarks of the interpreters.",
        rot=0,
    )

    ax.set_xlabel("Benchmarks")
    ax.set_ylabel("Time (normalized)")

    # Save the plot
    fig = ax.get_figure()
    fig.savefig("plot.png")


def main():
    # Create all targets
    targets = [(p, b) for b in BENCHMARKS for p in PROGRAMS]

    results = {b: dict() for b in BENCHMARKS}
    for target in targets:
        elapsed_ms = run_multiple(target, max(4, os.cpu_count() // 2))
        results[target[1]][target[0]] = elapsed_ms

    plot(results)


if __name__ == "__main__":
    main()
