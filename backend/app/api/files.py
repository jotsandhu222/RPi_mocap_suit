from fastapi import APIRouter, Depends, HTTPException, UploadFile, File, Form
from fastapi.responses import FileResponse, StreamingResponse
import os
import shutil
from typing import List
from app.api.auth import get_current_user
from app.schemas.schemas import FileInfo
from app.core.config import settings

router = APIRouter()

def get_real_path(path: str) -> str:
    # Normalize path to prevent directory traversal
    clean_path = os.path.normpath(f"/{path}").lstrip('/')
    real_path = os.path.join(settings.DATA_DIR, 'storage', clean_path)

    if not os.path.abspath(real_path).startswith(os.path.abspath(os.path.join(settings.DATA_DIR, 'storage'))):
        raise HTTPException(status_code=403, detail="Access denied")
    return real_path

@router.get("/list", response_model=List[FileInfo])
def list_files(path: str = "", current_user: dict = Depends(get_current_user)):
    real_path = get_real_path(path)
    os.makedirs(os.path.join(settings.DATA_DIR, 'storage'), exist_ok=True)

    if not os.path.exists(real_path) or not os.path.isdir(real_path):
        raise HTTPException(status_code=404, detail="Directory not found")

    files = []
    for f in os.listdir(real_path):
        f_path = os.path.join(real_path, f)
        stat = os.stat(f_path)
        rel_path = os.path.join(path, f).strip('/')
        files.append({
            "name": f,
            "path": rel_path,
            "is_dir": os.path.isdir(f_path),
            "size": stat.st_size,
            "modified_at": stat.st_mtime
        })
    return files

@router.post("/upload")
def upload_file(path: str = Form(""), file: UploadFile = File(...), current_user: dict = Depends(get_current_user)):
    real_dir = get_real_path(path)
    os.makedirs(real_dir, exist_ok=True)

    file_path = os.path.join(real_dir, os.path.basename(file.filename))
    with open(file_path, "wb") as buffer:
        shutil.copyfileobj(file.file, buffer)

    return {"message": "File uploaded successfully", "filename": file.filename}

@router.post("/mkdir")
def create_directory(path: str, folder_name: str, current_user: dict = Depends(get_current_user)):
    real_dir = get_real_path(os.path.join(path, folder_name))
    os.makedirs(real_dir, exist_ok=True)
    return {"message": "Directory created"}

@router.delete("/delete")
def delete_item(path: str, current_user: dict = Depends(get_current_user)):
    if not path:
        raise HTTPException(status_code=400, detail="Cannot delete root directory")

    real_path = get_real_path(path)
    if not os.path.exists(real_path):
        raise HTTPException(status_code=404, detail="Item not found")

    if os.path.isdir(real_path):
        shutil.rmtree(real_path)
    else:
        os.remove(real_path)
    return {"message": "Item deleted"}

@router.get("/download")
def download_file(path: str, current_user: dict = Depends(get_current_user)):
    real_path = get_real_path(path)
    if not os.path.exists(real_path) or os.path.isdir(real_path):
        raise HTTPException(status_code=404, detail="File not found")
    return FileResponse(real_path, filename=os.path.basename(real_path))

@router.get("/stream")
def stream_media(path: str, current_user: dict = Depends(get_current_user)):
    real_path = get_real_path(path)
    if not os.path.exists(real_path) or os.path.isdir(real_path):
        raise HTTPException(status_code=404, detail="File not found")

    # Standard FileResponse handles range requests automatically in FastAPI
    return FileResponse(real_path)
