import sys
sys.path.insert(0, ".")

from fastapi.testclient import TestClient
from api.main import app

client = TestClient(app)


def test_store_ok():
    r = client.post("/store", json={"article_id": 1, "quantity": 4})
    assert r.status_code == 200
    assert r.json() == {"stored": True}


def test_store_too_much_gives_409():
    r = client.post("/store", json={"article_id": 1, "quantity": 100})
    assert r.status_code == 409


def test_unknown_article_gives_409():
    r = client.post("/store", json={"article_id": 999, "quantity": 1})
    assert r.status_code == 409


def test_invalid_quantity_gives_422():
    r = client.post("/store", json={"article_id": 1, "quantity": -5})
    assert r.status_code == 422
