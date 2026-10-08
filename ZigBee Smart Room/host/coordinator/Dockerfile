FROM python:3.12-slim

ENV PYTHONDONTWRITEBYTECODE=1 \
    PYTHONUNBUFFERED=1

WORKDIR /app
COPY host/coordinator/room_coordinator ./room_coordinator
COPY host/coordinator/config.json ./config.json
RUN pip install --no-cache-dir 'paho-mqtt>=2.0,<3'

CMD ["python", "-m", "room_coordinator.main", "--config", "/app/config.json"]
