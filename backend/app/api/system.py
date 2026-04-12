from fastapi import APIRouter, Depends
import shutil
import socket
from app.api.auth import get_current_user
from app.schemas.schemas import SystemStatus
from app.core.config import settings
import os

router = APIRouter()

@router.get("/status", response_model=SystemStatus)
def get_system_status(current_user: dict = Depends(get_current_user)):
    storage_path = os.path.join(settings.DATA_DIR, 'storage')
    os.makedirs(storage_path, exist_ok=True)

    total, used, free = shutil.disk_usage(storage_path)

    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(('10.255.255.255', 1))
        IP = s.getsockname()[0]
    except Exception:
        IP = '127.0.0.1'
    finally:
        s.close()

    return {
        "total_space": total,
        "used_space": used,
        "free_space": free,
        "server_ip": IP
    }
