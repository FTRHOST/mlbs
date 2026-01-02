MODDIR=${0%/*}

# Zygisk config
# For MLBB: com.mobile.legends
# Replace "libMEOW.so" with your actual library name if different
echo "libMEOW.so" > "$MODDIR/zygisk/com.mobile.legends.config"