# Navigate to your project directory (if not already there)
cd /home/ziayd/hive/firmware_touchscreens/leader

# 1. Make sure ESP-IDF environment is loaded
source ~/esp-idf/export.sh

echo "Choose an operation:"
echo "1) Build only"
echo "2) Erase flash only"
echo "3) Flash only"
echo "4) Monitor only"
echo "5) Full sequence (build, erase, flash, monitor)"
read -p "Enter your choice (1-5): " choice

case $choice in
    1)
        echo "Building project..."
        idf.py build
        ;;
    2)
        echo "Erasing flash..."
        idf.py -p /dev/ttyACM0 erase_flash
        ;;
    3)
        echo "Flashing firmware..."
        idf.py -p /dev/ttyACM0 flash
        echo "Resetting device..."
        idf.py -p /dev/ttyACM0 monitor --no-reset &  # Start monitor without reset
        sleep 2
        pkill -f "idf.py.*monitor"  # Kill the temporary monitor
        echo "Please unplug and replug your device"
        ;;
    4)
        echo "Starting monitor..."
        idf.py -p /dev/ttyACM0 monitor
        ;;
    5)
        echo "Running full sequence..."
        echo "Building project..."
        idf.py build
        if [ $? -ne 0 ]; then
            echo "Build failed"
            exit 1
        fi

        echo "Erasing flash..."
        idf.py -p /dev/ttyACM0 erase_flash
        if [ $? -ne 0 ]; then
            echo "Flash erase failed"
            exit 1
        fi
        sleep 2

        echo "Flashing firmware..."
        idf.py -p /dev/ttyACM0 flash
        if [ $? -ne 0 ]; then
            echo "Flash failed"
            exit 1
        fi
        
        echo "Resetting device..."
        echo "Please unplug and replug your device"
        sleep 3

        echo "Starting monitor..."
        idf.py -p /dev/ttyACM0 monitor
        ;;
    *)
        echo "Invalid choice"
        exit 1
        ;;
esac