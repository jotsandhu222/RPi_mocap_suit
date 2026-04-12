from fastapi import APIRouter, Depends, HTTPException, status, Request
from fastapi.security import OAuth2PasswordBearer
import sqlite3
from typing import Optional
from app.db.session import get_db
from app.core.security import verify_password, create_access_token, get_password_hash
from app.schemas.schemas import Token, AuthLogin
from jose import jwt, JWTError
from app.core.config import settings
import socket

router = APIRouter()
oauth2_scheme = OAuth2PasswordBearer(tokenUrl=f"{settings.API_V1_STR}/auth/login", auto_error=False)

def get_current_user(request: Request, token: Optional[str] = Depends(oauth2_scheme), db: sqlite3.Connection = Depends(get_db)):
    if not token:
        token = request.query_params.get("token")

    credentials_exception = HTTPException(
        status_code=status.HTTP_401_UNAUTHORIZED,
        detail="Could not validate credentials",
        headers={"WWW-Authenticate": "Bearer"},
    )

    if not token:
        raise credentials_exception

    try:
        payload = jwt.decode(token, settings.SECRET_KEY, algorithms=["HS256"])
        username: str = payload.get("sub")
        if username is None:
            raise credentials_exception
    except JWTError:
        raise credentials_exception

    cursor = db.cursor()
    cursor.execute("SELECT * FROM users WHERE username = ?", (username,))
    user = cursor.fetchone()
    if user is None:
        raise credentials_exception
    return user

@router.post("/login", response_model=Token)
def login(login_data: AuthLogin, db: sqlite3.Connection = Depends(get_db)):
    cursor = db.cursor()
    cursor.execute("SELECT * FROM users WHERE username = 'admin'")
    user = cursor.fetchone()

    if not user:
        hashed_password = get_password_hash(login_data.pin)
        cursor.execute("INSERT INTO users (username, hashed_password) VALUES (?, ?)", ('admin', hashed_password))
        db.commit()
    elif not verify_password(login_data.pin, user['hashed_password']):
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Incorrect PIN",
            headers={"WWW-Authenticate": "Bearer"},
        )

    access_token = create_access_token(subject='admin')
    return {"access_token": access_token, "token_type": "bearer"}

@router.get("/pairing-info")
def get_pairing_info(db: sqlite3.Connection = Depends(get_db)):
    cursor = db.cursor()
    cursor.execute("SELECT * FROM users WHERE username = 'admin'")
    user = cursor.fetchone()
    if user:
        raise HTTPException(status_code=403, detail="System already paired. Login directly.")

    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(('10.255.255.255', 1))
        IP = s.getsockname()[0]
    except Exception:
        IP = '127.0.0.1'
    finally:
        s.close()

    return {"server_ip": IP}
