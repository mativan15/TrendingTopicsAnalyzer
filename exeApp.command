#!/bin/bash
DIR="$(cd "$(dirname "$0")" && pwd)"
BACKEND="$DIR/backend"
FRONTEND="$DIR/frontend"
osascript <<APPLESCRIPT
tell application "Terminal"
    activate
    do script "cd \"$BACKEND\"; make; ./backend; exec \$SHELL"
    delay 1
    do script "cd \"$FRONTEND\"; until nc -z localhost 8081; do echo 'Esperando que backend arranque...'; sleep 1; done; source env/bin/activate; flask --app webapp run --port 8080; exec \$SHELL"
end tell
APPLESCRIPT