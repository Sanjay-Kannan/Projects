"""Summarize captured ESP32 scheduler events and optionally plot a schedule."""
import argparse
import json
from collections import defaultdict
from pathlib import Path


def load_finished(path: str) -> list[dict]:
    releases = {}
    starts = {}
    finished = []
    with open(path, encoding="utf-8") as source:
        for line_number, line in enumerate(source, 1):
            try:
                event = json.loads(line)
            except json.JSONDecodeError as exc:
                raise ValueError(f"invalid JSON on line {line_number}: {exc}") from exc
            key = (event.get("task"), event.get("job"))
            if event.get("event") == "release":
                releases[key] = event
            elif event.get("event") == "start":
                starts[key] = event
            elif event.get("event") == "finish" and key in starts:
                start = starts.pop(key)
                release = releases.get(key)
                finished.append({"task": key[0], "job": key[1],
                                 "release_us": release["release_us"] if release else None,
                                 "start_us": start["time_us"], "finish_us": event["time_us"],
                                 "deadline_us": event["deadline_us"],
                                 "elapsed_us": event["elapsed_us"],
                                 "deadline_miss": event["deadline_miss"]})
    return finished


def load_drops(path: str) -> list[dict]:
    with open(path, encoding="utf-8") as source:
        return [event for line in source if (event := json.loads(line)).get("event") == "drop"]


def analyze(path: str, plot: str | None = None) -> dict:
    jobs = load_finished(path)
    drops = load_drops(path)
    if not jobs:
        raise ValueError("no complete start/finish job pairs found")
    origin = min(job["start_us"] for job in jobs)
    by_task = defaultdict(list)
    for job in jobs:
        job["start_ms"] = (job["start_us"] - origin) / 1000
        job["finish_ms"] = (job["finish_us"] - origin) / 1000
        job["response_ms"] = ((job["finish_us"] - job["release_us"]) / 1000
                              if job["release_us"] is not None else None)
        by_task[job["task"]].append(job)
    def average_response(rows: list[dict]) -> float | None:
        values = [job["response_ms"] for job in rows if job["response_ms"] is not None]
        return sum(values) / len(values) if values else None

    def maximum_response(rows: list[dict]) -> float | None:
        values = [job["response_ms"] for job in rows if job["response_ms"] is not None]
        return max(values) if values else None

    summary = {"jobs": len(jobs), "dropped_jobs": len(drops),
               "deadline_misses": sum(j["deadline_miss"] for j in jobs),
               "tasks": {name: {"jobs": len(rows),
                                "deadline_misses": sum(j["deadline_miss"] for j in rows),
                                "avg_elapsed_ms": sum(j["elapsed_us"] for j in rows) / len(rows) / 1000,
                                "max_elapsed_ms": max(j["elapsed_us"] for j in rows) / 1000,
                                "avg_response_ms": average_response(rows),
                                "max_response_ms": maximum_response(rows)}
                          for name, rows in sorted(by_task.items())}}
    if plot:
        import matplotlib.pyplot as plt
        names = sorted(by_task)
        y = {name: index for index, name in enumerate(names)}
        fig, axis = plt.subplots(figsize=(12, max(3, 0.8 * len(names))))
        for job in jobs:
            left = job["start_ms"]
            width = job["finish_ms"] - left
            axis.barh(y[job["task"]], width, left=left, height=0.55,
                      color="#d9534f" if job["deadline_miss"] else "#2878b5")
            axis.text(left + width / 2, y[job["task"]], str(job["job"]),
                      ha="center", va="center", color="white", fontsize=7)
        axis.set_yticks(range(len(names)), names)
        axis.set_xlabel("Time from first job start (ms)")
        axis.set_title("EDF job response intervals (bars include preemption)")
        axis.grid(axis="x", alpha=0.25)
        fig.tight_layout()
        fig.savefig(Path(plot), dpi=160)
        plt.close(fig)
    return summary


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("log", help="JSONL capture from host.collect")
    parser.add_argument("--plot", help="save a schedule chart, e.g. schedule.png")
    args = parser.parse_args()
    print(json.dumps(analyze(args.log, args.plot), indent=2))


if __name__ == "__main__":
    main()
