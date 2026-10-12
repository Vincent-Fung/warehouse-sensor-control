import sys
sys.path.insert(0, "build")  # Ort der warehouse_py-.so

import warehouse_py as w
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel

app = FastAPI(title="Warehouse API")

ARTICLES = [w.Article(1, "Schraube M5", 1.0), w.Article(2, "Mutter M5", 1.0)]
LOCATIONS = [w.StorageLocation(i, 10.0, 0.0, None) for i in (1, 2, 3)]
warehouse = w.Warehouse(LOCATIONS, ARTICLES)


class StoreRequest(BaseModel):
    article_id: int
    quantity: float


@app.post("/store")
def store(req: StoreRequest):
    if req.quantity <= 0:
        raise HTTPException(status_code=422, detail="quantity must be > 0")
    ok = warehouse.store_article(req.article_id, req.quantity)
    if not ok:
        raise HTTPException(status_code=409, detail="no capacity or unknown article")
    return {"stored": True}
