# Warehouse Sensor Control

C++20-Projekt zu Lagerlogik und Sensordatenverarbeitung, mit Python-Anbindung.

## Features
- **Lagerlogik:** `Warehouse`, `StorageLocation`, `Article`. Die Platzierung ist per Strategy Pattern austauschbar (`FirstFitStrategy`, `ConsolidationStrategy`).
- **Nebenläufigkeit:** thread-sichere `SensorQueue` (`mutex` + `condition_variable`) mit sauberem Shutdown über `close()`.
- **Netzwerk:** einfacher TCP-Server und -Client (POSIX-Sockets).
- **Python-Anbindung:** pybind11-Modul `warehouse_py`.
- **Tests:** 12 GoogleTest-Fälle, inklusive Threading-Verhalten.
- **Container:** Multi-Stage-Dockerfile.

## Build
```bash
mkdir build && cd build
cmake ..
cmake --build .
ctest --output-on-failure
```

## Python
```python
import warehouse_py as w
wh = w.Warehouse([w.StorageLocation(1, 10.0, 0.0, None)], [w.Article(1, "Schraube", 1.0)])
print(wh.store_article(1, 5.0))  # True
```

## Docker
```bash
docker build -t warehouse-sensor-control .
docker run warehouse-sensor-control
```

## Architektur
`core/` enthält die Bibliothek `warehouse_core`, `network/` die TCP-Beispiele, `python/` die Bindings und `tests/` die GoogleTest-Suite.