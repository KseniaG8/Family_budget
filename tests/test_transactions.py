import requests

BASE_URL = "http://localhost:8080"

def test_add_standard_transaction(new_user):
    payload = {
        "user_id": new_user["user_id"],
        "type": "expense",
        "amount": 1500.0,
        "category": "Еда",
        "currency": "RUB",
        "description": "Поход в ресторан"
    }
    resp = requests.post(f"{BASE_URL}/transactions", json=payload)
    assert resp.json().get("status") == "success"

def test_ml_categorization(new_user):
    payload = {
        "user_id": new_user["user_id"],
        "type": "expense",
        "amount": 350.0,
        "category": "", 
        "currency": "RUB",
        "description": "Покупка продуктов в Шестерочке"
    }
    resp = requests.post(f"{BASE_URL}/transactions", json=payload)
    assert resp.json().get("status") == "success"

def test_get_balance(new_user):
    requests.post(f"{BASE_URL}/transactions", json={
        "user_id": new_user["user_id"], "type": "income", "amount": 5000.0,
        "category": "Зарплата", "currency": "RUB", "description": ""
    })
    resp = requests.get(f"{BASE_URL}/balance?user_id={new_user['user_id']}")
    assert resp.json().get("balance") == 5000.0
