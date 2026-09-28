import json

import pytest

from host.edf import PeriodicTask, simulate
from host.events import parse_event


def test_edf_selects_earliest_absolute_deadline():
    tasks = [PeriodicTask("long_period", 100, 80, 20),
             PeriodicTask("short_deadline", 100, 10, 5)]
    rows = simulate(tasks, 1)
    assert [row["task"] for row in rows] == ["short_deadline", "long_period"]
    assert rows[0]["start_ms"] == 0


def test_simulator_marks_deadline_miss():
    rows = simulate([PeriodicTask("slow", 50, 10, 20)], 1)
    assert rows[0]["deadline_miss"] is True


def test_simulator_preempts_running_job_for_earlier_deadline():
    tasks = [PeriodicTask("running", 100, 90, 20),
             PeriodicTask("urgent", 100, 5, 2, offset_ms=5)]
    rows = simulate(tasks, 30)
    assert rows[0]["task"] == "urgent"
    assert rows[0]["start_ms"] == 5
    assert rows[1]["preemptions"] == 1
    assert rows[1]["finish_ms"] == 22


def test_simulator_rejects_invalid_period():
    with pytest.raises(ValueError):
        simulate([PeriodicTask("bad", 0, 10, 1)], 10)


def test_parse_event_accepts_release_json():
    event = {"event": "release", "task": "sensor", "job": 1,
             "release_us": 10, "deadline_us": 20}
    assert parse_event(json.dumps(event)) == event


def test_parse_event_ignores_comments():
    assert parse_event("# booting") is None
    assert parse_event("  ") is None


def test_parse_event_rejects_missing_fields():
    with pytest.raises(ValueError, match="missing fields"):
        parse_event('{"event":"finish","task":"sensor"}')
