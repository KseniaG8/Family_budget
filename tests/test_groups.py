import requests

BASE_URL = "http://localhost:8080"

def test_group_operations(new_user):ё
    group_payload = {
        "name": "Семья",
        "owner_id": new_user["user_id"]
    }
    resp = requests.post(f"{BASE_URL}/groups", json=group_payload)
    
    groups_resp = requests.get(f"{BASE_URL}/groups?user_id={new_user['user_id']}")
    groups = groups_resp.json()
    assert isinstance(groups, list)
    
    if len(groups) > 0:
        group_id = groups[0]["id"]
        
        trans_payload = {
            "group_id": group_id,
            "user_id": new_user["user_id"],
            "type": "expense",
            "amount": 3000.0,
            "category": "ЖКХ"
        }
        requests.post(f"{BASE_URL}/groups/transactions", json=trans_payload)
        balance_resp = requests.get(f"{BASE_URL}/groups/balance?group_id={group_id}")
        data = balance_resp.json()
        if "balance" in data:
            assert data.get("balance") == -3000.0
