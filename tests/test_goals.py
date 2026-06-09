import requests

BASE_URL = "http://localhost:8080"

def test_create_and_update_goal(new_user):
    goal_payload = {
        "user_id": new_user["user_id"],
        "name": "Ноутбук",
        "target_amount": 100000.0
    }
    resp = requests.post(f"{BASE_URL}/goals", json=goal_payload)
    assert resp.status_code == 200
    
    goals_resp = requests.get(f"{BASE_URL}/goals?user_id={new_user['user_id']}")
    goals = goals_resp.json()
    assert isinstance(goals, list)
    
    if len(goals) > 0:
        goal_id = goals[0]["id"]
        progress_payload = {
            "goal_id": goal_id,
            "current_amount": 15000.0
        }
        prog_resp = requests.post(f"{BASE_URL}/goals/progress", json=progress_payload)
        assert prog_resp.status_code == 200
