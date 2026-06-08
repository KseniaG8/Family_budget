import requests
import uuid

BASE_URL = "http://localhost:8080"

def test_registration_validation():
    short_user = {"login": "a", "password": "123"}
    resp = requests.post(f"{BASE_URL}/register", json=short_user)
    assert resp.json().get("status") in ["short_login", "short_password"]

def test_successful_registration():
    valid_user = {"login": f"test_{uuid.uuid4().hex[:6]}", "password": "SecurePassword123!"}
    resp = requests.post(f"{BASE_URL}/register", json=valid_user)
    assert resp.json().get("status") == "success"

def test_duplicate_registration():
    valid_user = {"login": f"test_{uuid.uuid4().hex[:6]}", "password": "SecurePassword123!"}
    requests.post(f"{BASE_URL}/register", json=valid_user)
    resp = requests.post(f"{BASE_URL}/register", json=valid_user)
    assert resp.json().get("status") == "user_exists"

def test_invalid_login():
    bad_login = {"login": "nonexistent_user", "password": "WrongPassword"}
    resp = requests.post(f"{BASE_URL}/login", json=bad_login)
    assert resp.json().get("status") == "invalid_credentials"
