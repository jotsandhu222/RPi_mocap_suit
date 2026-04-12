from pydantic import BaseModel
from typing import List, Optional

class Token(BaseModel):
    access_token: str
    token_type: str

class FileInfo(BaseModel):
    name: str
    path: str
    is_dir: bool
    size: int
    modified_at: float

class SystemStatus(BaseModel):
    total_space: int
    used_space: int
    free_space: int
    server_ip: str

class AuthLogin(BaseModel):
    pin: str
