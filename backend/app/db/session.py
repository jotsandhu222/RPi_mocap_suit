import sqlite3
import os
from app.core.config import settings

def get_db():
    conn = sqlite3.connect(
        os.path.join(settings.DATA_DIR, 'cloud.db'),
        check_same_thread=False
    )
    conn.row_factory = sqlite3.Row
    try:
        yield conn
    finally:
        conn.close()

def init_db():
    conn = sqlite3.connect(os.path.join(settings.DATA_DIR, 'cloud.db'))
    cursor = conn.cursor()

    # Create users table
    cursor.execute('''
    CREATE TABLE IF NOT EXISTS users (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        username TEXT UNIQUE NOT NULL,
        hashed_password TEXT NOT NULL
    )
    ''')

    # Ensure admin user exists (default pin: 1234)
    # We will hash the password when creating the user properly, for now, init schema.
    conn.commit()
    conn.close()
