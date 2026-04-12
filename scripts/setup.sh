#!/bin/bash

echo "==========================================="
echo "   Personal Cloud - Raspberry Pi Setup     "
echo "==========================================="

# Ensure we are in the root directory of the project where docker-compose.yml lives
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
cd "$PROJECT_ROOT"

# Check if running as root
if [ "$EUID" -ne 0 ]; then
  echo "Please run as root (sudo ./scripts/setup.sh)"
  exit 1
fi

echo "[1/4] Installing Docker and dependencies..."
if ! command -v docker &> /dev/null; then
    curl -fsSL https://get.docker.com -o get-docker.sh
    sh get-docker.sh
    if [ -n "$SUDO_USER" ]; then
        usermod -aG docker $SUDO_USER
    fi
    rm get-docker.sh
    echo "Docker installed."
else
    echo "Docker is already installed."
fi

if ! command -v docker-compose &> /dev/null; then
    apt-get update
    apt-get install -y docker-compose-plugin
    echo "Docker Compose installed."
else
    echo "Docker Compose is already installed."
fi

# Make sure exFAT and NTFS are supported to prevent Input/output errors
apt-get update
apt-get install -y exfat-fuse ntfs-3g

echo "[2/4] Setting up external storage auto-mount..."

# Hardcoded to explicitly use /dev/sda2 based on user provided hardware configuration
SSD_PARTITION="/dev/sda2"

if lsblk "$SSD_PARTITION" > /dev/null 2>&1; then
    echo "Found forced storage partition: $SSD_PARTITION"

    # Check if it's already mounted somewhere else by the OS (e.g. /media/pi/...)
    CURRENT_MOUNT=$(lsblk -rn -o MOUNTPOINT "$SSD_PARTITION")

    if [ -n "$CURRENT_MOUNT" ] && [ "$CURRENT_MOUNT" != "/mnt/cloud_data" ]; then
        echo "Partition is currently auto-mounted by OS at: $CURRENT_MOUNT"
        echo "Unmounting to remount at /mnt/cloud_data..."
        umount -f "$SSD_PARTITION" || true
    fi

    # Try to clean up broken mounts
    umount -l /mnt/cloud_data 2>/dev/null || true

    mkdir -p /mnt/cloud_data
    UUID=$(blkid -s UUID -o value "$SSD_PARTITION")
    TYPE=$(blkid -s TYPE -o value "$SSD_PARTITION")

    if [ -n "$UUID" ]; then
        # For exFAT/NTFS on Linux, we need uid/gid options so Docker can read/write it
        # Apply standard exfat mount options directly via CLI mount first to prevent I/O errors
        if ! grep -qs '/mnt/cloud_data' /proc/mounts; then
            echo "Mounting $SSD_PARTITION (UUID: $UUID, TYPE: $TYPE) to /mnt/cloud_data"
            if [ "$TYPE" == "exfat" ] || [ "$TYPE" == "ntfs" ]; then
                mount -t "$TYPE" -o defaults,auto,umask=000,users,rw "$SSD_PARTITION" /mnt/cloud_data
            else
                mount "$SSD_PARTITION" /mnt/cloud_data
            fi
        else
            echo "Storage already mounted at /mnt/cloud_data."
        fi

        # Add to fstab if not already present
        if ! grep -q "$UUID" /etc/fstab; then
            echo "Adding $SSD_PARTITION to /etc/fstab for persistent auto-mount..."
            if [ "$TYPE" == "exfat" ] || [ "$TYPE" == "ntfs" ]; then
                echo "UUID=$UUID /mnt/cloud_data $TYPE defaults,auto,umask=000,users,rw 0 0" >> /etc/fstab
            else
                echo "UUID=$UUID /mnt/cloud_data auto defaults,nofail 0 2" >> /etc/fstab
            fi
        fi
    else
        echo "Error: Could not determine UUID for $SSD_PARTITION. Please format it first."
        echo "Using local /mnt/cloud_data directory as fallback."
    fi
else
    echo "Error: Partition $SSD_PARTITION not found. Ensure the drive is plugged in."
    mkdir -p /mnt/cloud_data
    echo "Using local /mnt/cloud_data directory as fallback."
fi

# Ensure storage directory exists and has correct permissions
mkdir -p /mnt/cloud_data/storage
if [ -n "$SUDO_USER" ]; then
    chown -R $SUDO_USER:$SUDO_USER /mnt/cloud_data || echo "Warning: chown failed, likely an exFAT/NTFS permission boundary. Continuing."
fi

# Remove old symlink approach if it exists
if [ -L "./data/storage" ]; then
    rm "./data/storage"
fi
mkdir -p ./data

echo "[3/4] Attempting to configure Tailscale for remote access..."
if ! command -v tailscale &> /dev/null; then
    curl -fsSL https://tailscale.com/install.sh | sh
    echo "Tailscale installed. Please run 'sudo tailscale up' to authenticate and connect to your VPN."
else
    echo "Tailscale is already installed."
fi

echo "[4/4] Starting Personal Cloud services..."

# Export environment variable used in docker-compose.yml to mount the exact SSD path
export DATA_MOUNT_POINT="/mnt/cloud_data"

# Stop existing containers to ensure the new volume mount takes effect
docker compose down || true
docker compose up -d

echo "==========================================="
echo "Setup Complete!"
echo "Local Access: http://$(hostname -I | awk '{print $1}')"
echo ""
echo "Remote Access (if Tailscale is configured):"
echo "http://$(tailscale ip -4 2>/dev/null || echo '<run tailscale up>')"
echo "==========================================="
