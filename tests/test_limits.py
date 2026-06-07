import requests

BASE_URL = "http://localhost:8080"

def test_set_and_check_limit(new_user):
    user_id = new_user["user_id"]
    category = "Развлечения"
    
    limit_payload = {
        "user_id": user_id,
        "category": category,
        "limit_amount": 5000.0,
        "period": "month"
    }
    resp = requests.post(f"{BASE_URL}/limits", json=limit_payload)
    
    requests.post(f"{BASE_URL}/transactions", json={
        "user_id": user_id, "type": "expense", "amount": 2000.0,
        "category": category, "currency": "RUB", "description": "Кино"
    })
    
    check_resp = requests.get(f"{BASE_URL}/limits/check?user_id={user_id}&category={category}")
    data = check_resp.json()
    
    if "error" not in data:
        assert data.get("limit") == 5000.0
        assert data.get("spent") == 2000.0
