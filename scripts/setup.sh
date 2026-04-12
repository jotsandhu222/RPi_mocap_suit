#!/bin/bash

echo "==========================================="
echo "   Personal Cloud - Raspberry Pi Setup     "
echo "==========================================="

# Check if running as root
if [ "$EUID" -ne 0 ]; then
  echo "Please run as root (sudo ./setup.sh)"
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

echo "[2/4] Setting up external storage auto-mount..."

# Try to find the largest external USB partition to use as cloud storage
# Look for any disk partiton larger than 10GB that is not mounted as root (/) or boot
SSD_PARTITION=$(lsblk -rn -o NAME,TYPE,SIZE,MOUNTPOINT | grep 'part' | grep -v ' /$' | grep -v '/boot' | awk '$3 ~ /G|T/ && $3+0 > 10 {print "/dev/"$1}' | head -n 1)

if [ -n "$SSD_PARTITION" ]; then
    echo "Found available storage partition: $SSD_PARTITION"

    # Check if it's already mounted somewhere else by the OS (e.g. /media/pi/...)
    CURRENT_MOUNT=$(lsblk -rn -o MOUNTPOINT "$SSD_PARTITION")

    if [ -n "$CURRENT_MOUNT" ] && [ "$CURRENT_MOUNT" != "/mnt/cloud_data" ]; then
        echo "Partition is currently auto-mounted by OS at: $CURRENT_MOUNT"
        echo "Unmounting to remount at /mnt/cloud_data..."
        umount "$SSD_PARTITION"
    fi

    mkdir -p /mnt/cloud_data
    UUID=$(blkid -s UUID -o value "$SSD_PARTITION")

    if [ -n "$UUID" ]; then
        if ! grep -qs '/mnt/cloud_data' /proc/mounts; then
            echo "Mounting $SSD_PARTITION (UUID: $UUID) to /mnt/cloud_data"
            mount "$SSD_PARTITION" /mnt/cloud_data
        else
            echo "Storage already mounted at /mnt/cloud_data."
        fi

        # Add to fstab if not already present
        if ! grep -q "$UUID" /etc/fstab; then
            echo "Adding $SSD_PARTITION to /etc/fstab for persistent auto-mount..."
            echo "UUID=$UUID /mnt/cloud_data auto defaults,nofail 0 2" >> /etc/fstab
        fi
    else
        echo "Error: Could not determine UUID for $SSD_PARTITION. Please format it first."
        echo "Using local /mnt/cloud_data directory as fallback."
    fi
else
    echo "No suitable external storage partition found."
    mkdir -p /mnt/cloud_data
    echo "Using local /mnt/cloud_data directory as fallback."
fi

# Ensure storage directory exists and has correct permissions
mkdir -p /mnt/cloud_data/storage
if [ -n "$SUDO_USER" ]; then
    chown -R $SUDO_USER:$SUDO_USER /mnt/cloud_data
fi

# Remove old symlink approach if it exists
if [ -L "../data/storage" ]; then
    rm "../data/storage"
fi
mkdir -p ../data

echo "[3/4] Attempting to configure Tailscale for remote access..."
if ! command -v tailscale &> /dev/null; then
    curl -fsSL https://tailscale.com/install.sh | sh
    echo "Tailscale installed. Please run 'sudo tailscale up' to authenticate and connect to your VPN."
else
    echo "Tailscale is already installed."
fi

echo "[4/4] Starting Personal Cloud services..."
cd ..

# Export environment variable used in docker-compose.yml to mount the exact SSD path
export DATA_MOUNT_POINT="/mnt/cloud_data"
docker compose up -d

echo "==========================================="
echo "Setup Complete!"
echo "Local Access: http://$(hostname -I | awk '{print $1}')"
echo ""
echo "Remote Access (if Tailscale is configured):"
echo "http://$(tailscale ip -4 2>/dev/null || echo '<run tailscale up>')"
echo "==========================================="
