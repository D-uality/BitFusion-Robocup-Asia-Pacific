import os

# Default settings
WIDTH, HEIGHT = 480, 480
FLIP = False
AUTOFOCUS = True
CROP_MIN, CROP_MAX = 100, HEIGHT

# Set SSH based on whether the script is running at startup
# You can check for a specific environment variable or some other condition
if os.getenv('STARTUP_MODE') == '1':
  SSH = False
else:
  SSH = True
