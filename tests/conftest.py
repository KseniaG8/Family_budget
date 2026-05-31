import pytest
import requests
import uuid

BASE_URL = "http://localhost:8080"

@pytest.fixture
def new_user():
    """Generates a new user and returns their credentials and ID"""
    user_data = {
        "login": f"test_{uuid.uuid4().hex[:6]}",
        "password": "SecurePassword123!"
    }
    requests.post(f"{BASE_URL}/register", json=user_data)
    
    resp = requests.post(f"{BASE_URL}/login", json=user_data)
    user_data["user_id"] = resp.json().get("user_id", 1) 
    
    return user_data
