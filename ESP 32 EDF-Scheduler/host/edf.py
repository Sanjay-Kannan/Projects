"""Event-driven preemptive EDF simulator for periodic task sets."""
from dataclasses import dataclass
import heapq
from typing import Iterable


@dataclass(frozen=True)
class PeriodicTask:
    name: str
    period_ms: int
    deadline_ms: int
    execution_ms: int
    offset_ms: int = 0


def simulate(tasks: Iterable[PeriodicTask], duration_ms: int) -> list[dict]:
    """Simulate one preemptive processor and return completed job records."""
    tasks = list(tasks)
    if duration_ms < 0:
        raise ValueError("duration_ms must be non-negative")
    for task in tasks:
        if min(task.period_ms, task.deadline_ms, task.execution_ms) <= 0 or task.offset_ms < 0:
            raise ValueError(f"task {task.name!r} has invalid timing values")

    releases = []
    for task_index, task in enumerate(tasks):
        release = task.offset_ms
        job_number = 1
        while release < duration_ms:
            releases.append((release, task_index, job_number, task))
            release += task.period_ms
            job_number += 1
    releases.sort(key=lambda item: (item[0], item[1], item[2]))

    ready = []
    release_index = 0
    completed = []
    current = None
    now = 0

    def job_key(job):
        return (job["deadline_ms"], job["release_ms"], job["task_index"], job["job"])

    while release_index < len(releases) or ready or current is not None:
        while release_index < len(releases) and releases[release_index][0] <= now:
            release, task_index, job_number, task = releases[release_index]
            job = {"task": task.name, "task_index": task_index, "job": job_number,
                   "release_ms": release, "deadline_ms": release + task.deadline_ms,
                   "remaining_ms": task.execution_ms, "execution_ms": task.execution_ms,
                   "start_ms": None, "preemptions": 0}
            heapq.heappush(ready, (*job_key(job), job))
            release_index += 1

        if current is not None and ready and ready[0][:4] < job_key(current):
            current["preemptions"] += 1
            heapq.heappush(ready, (*job_key(current), current))
            current = None

        if current is None:
            if ready:
                current = heapq.heappop(ready)[-1]
                if current["start_ms"] is None:
                    current["start_ms"] = now
            elif release_index < len(releases):
                now = releases[release_index][0]
                continue
            else:
                break

        next_release = releases[release_index][0] if release_index < len(releases) else None
        run_ms = current["remaining_ms"]
        if next_release is not None:
            run_ms = min(run_ms, next_release - now)
        current["remaining_ms"] -= run_ms
        now += run_ms

        if current["remaining_ms"] == 0:
            finish = now
            completed.append({"task": current["task"], "job": current["job"],
                              "release_ms": current["release_ms"],
                              "start_ms": current["start_ms"], "finish_ms": finish,
                              "deadline_ms": current["deadline_ms"],
                              "execution_ms": current["execution_ms"],
                              "response_ms": finish - current["release_ms"],
                              "preemptions": current["preemptions"],
                              "deadline_miss": finish > current["deadline_ms"]})
            current = None

    return completed
