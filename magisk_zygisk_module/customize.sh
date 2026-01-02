##########################################################################################
#
# Magisk Module Installer Script
#
##########################################################################################
#
# This script will be executed in recovery mode or Magisk Manager
#
##########################################################################################

# Info
ui_print "******************************"
ui_print "* MLBB Readonly Observer"
ui_print "* by YourName"
ui_print "******************************"

# Set permissions
# The `set_perm_recursive` function is defined in the Magisk `util_functions.sh`
# and is available here because `update-binary` sourced it.

ui_print "- Setting permissions"

# Set default directory and file permissions
set_perm_recursive $MODPATH 0 0 0755 0644

# Set executable permission for our Zygisk library
# This is crucial for the system to be able to load it.
set_perm $MODPATH/zygisk/libMEOW.so 0 0 0755
