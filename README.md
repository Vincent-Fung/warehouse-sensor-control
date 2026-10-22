# Warehouse Sensor Control

C++20-Projekt: Lagerlogik, Sensordatenverarbeitung, Netzwerk und Python-Anbindung.

## Architektur
```
sensor_client (n×) → TCP (Längenpräfix) → sensor_server (poll) → SensorQueue → Consumer
Python/FastAPI → pybind11 (warehouse_py) → Warehouse + PlacementStrategy (C++)
tls_client ⇄ TLS 1.2+/OpenSSL ⇄ tls_server
```

## Bausteine
- `core/`: `Warehouse`, `StorageLocation`, `Article`, Strategy Pattern (`FirstFit`, `Consolidation`), thread-sichere `SensorQueue` mit Shutdown über `close()`
- `network/`: TCP-Server mit `poll()` und Längenpräfix-Protokoll, TLS-Echo mit Zertifikatsprüfung
- `python/` und `api/`: pybind11-Bindings und FastAPI-Schicht
- `tests/` und `tests_py/`: GoogleTest (12 Tests) und pytest (4 Tests)

## Build und Tests
```bash
mkdir build && cd build && cmake .. && cmake --build . && ctest --output-on-failure
cd .. && python3 -m pytest tests_py -v
```

## Ausführen
```bash
./build/sensor_server               # Terminal 1
./build/sensor_client 1 5           # Terminal 2
# TLS (Zertifikat im build-Ordner erzeugen, siehe unten)
./tls_server & ./tls_client "Sensor 1: 23.5"
```

## Qualitätssicherung
ThreadSanitizer-Lauf (`-fsanitize=thread`) auf Tests und Producer/Consumer, Docker-Multi-Stage-Build.

## Bekannte Grenzen
- Das globale `warehouse`-Objekt in der API hat kein Locking
- TLS-Beispiel arbeitet blockierend, ohne `poll()` und ohne mTLS
- Sensor-Server hat keine Timeouts pro Client
- Keine Zertifikatsketten und keine Sperrlisten