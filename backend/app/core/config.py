from pydantic_settings import BaseSettings
import os

class Settings(BaseSettings):
    PROJECT_NAME: str = "Personal Cloud API"
    API_V1_STR: str = "/api/v1"
    SECRET_KEY: str = os.getenv("SECRET_KEY", "change-this-in-production")
    ACCESS_TOKEN_EXPIRE_MINUTES: int = 60 * 24 * 30  # 30 days
    DATA_DIR: str = os.getenv("DATA_DIR", "../data")
    DATABASE_URL: str = f"sqlite+aiosqlite:///{os.path.join(DATA_DIR, 'cloud.db')}"

    class Config:
        case_sensitive = True

settings = Settings()

# Ensure data directory exists
os.makedirs(settings.DATA_DIR, exist_ok=True)
