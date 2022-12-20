# A simple script to run teh different interpreters/JITs and create some
# plots how they perform.
import time
import subprocess
import matplotlib.pyplot as plt

PROGRAMS = ["brainint", "brainbyte"]
BENCHMARKS = ["mandelbrot.bf"]


def run(target):
    executable = "build/" + target[0]
    program = "examples/" + target[1]
    print(f"Running ./{executable} {program} ...")

    start = time.time_ns()
    subprocess.run([executable, program], capture_output=True)
    return (time.time_ns() - start) / 1000 / 1000


def plot(results):
    data = [t[1] for t in results["mandelbrot.bf"]]
    labels = [t[0] for t in results["mandelbrot.bf"]]
    plt.bar(
        labels,
        data,
        color=["#87CEFA", "#FFA07A"],
        edgecolor=["#000", "#000"],
        linewidth=2,
    )
    plt.ylabel("Time (ms)")
    plt.xlabel("Implementations")
    plt.title("Manelbrot Benchmark")
    plt.savefig("plot.png")


def main():
    # Create all targets
    targets = [(p, b) for b in BENCHMARKS for p in PROGRAMS]

    results = {b: [] for b in BENCHMARKS}
    for target in targets:
        elapsed_ms = run(target)
        results[target[1]].append((target[0], elapsed_ms))

    plot(results)


if __name__ == "__main__":
    main()
