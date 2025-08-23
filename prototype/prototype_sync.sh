#!/bin/bash
# GOES Prototype UI - Sync Script

set -e

# Get script directory (prototype folder)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
PROTOTYPE_DIR="$SCRIPT_DIR"
WEB_PAGE_FILE="$PROJECT_ROOT/src/web/web_page.h"

echo "🔄 GOES Prototype UI Sync Script"
echo "=================================="

# Function to extract HTML from web_page.h
extract_from_production() {
    echo "📤 Extracting HTML from production (web_page.h)..."
    
    awk '/R"rawliteral\(/{flag=1; gsub(/.*R"rawliteral\(/,""); if(length($0)>0) print} flag && !/\)rawliteral"/{print} /\)rawliteral"/{gsub(/\)rawliteral".*/,""); if(length($0)>0) print; flag=0}' "$WEB_PAGE_FILE" > "$PROTOTYPE_DIR/extracted.html"
    
    # Remove duplicate first line if exists
    if [[ $(head -n 2 "$PROTOTYPE_DIR/extracted.html" | uniq | wc -l) -eq 1 ]]; then
        sed -i '' '1d' "$PROTOTYPE_DIR/extracted.html"
    fi
    
    echo "✅ Extracted to prototype/extracted.html"
    echo "💡 Review changes and manually merge with prototype/index.html"
}

# Function to serve prototype locally
serve_prototype() {
    echo "🌐 Starting local server for prototype..."
    echo "📍 URL: http://localhost:8080"
    echo "🛑 Press Ctrl+C to stop"
    
    cd "$PROTOTYPE_DIR"
    python3 -m http.server 8080
}

# Function to show prototype status
show_status() {
    echo "📊 Prototype Status:"
    echo "-------------------"
    echo "📁 Prototype Directory: $PROTOTYPE_DIR"
    echo "📄 Production File: $WEB_PAGE_FILE"
    
    if [[ -f "$PROTOTYPE_DIR/index.html" ]]; then
        echo "✅ Prototype exists"
        LINES=$(wc -l < "$PROTOTYPE_DIR/index.html")
        echo "📏 Lines: $LINES"
    else
        echo "❌ Prototype not found"
    fi
    
    if [[ -f "$WEB_PAGE_FILE" ]]; then
        echo "✅ Production file exists"
        PROD_LINES=$(grep -c "rawliteral" "$WEB_PAGE_FILE" || echo "0")
        echo "🔧 Raw literal blocks: $PROD_LINES"
    else
        echo "❌ Production file not found"
    fi
}

# Function to open prototype in browser
open_prototype() {
    echo "🌐 Opening prototype in browser..."
    open "file://$PROTOTYPE_DIR/index.html"
}

# Main menu
case "${1:-menu}" in
    "extract"|"e")
        extract_from_production
        ;;
    "serve"|"s")
        serve_prototype
        ;;
    "open"|"o")
        open_prototype
        ;;
    "status"|"st")
        show_status
        ;;
    "menu"|"m"|*)
        echo "Usage: $0 [command]"
        echo ""
        echo "Commands:"
        echo "  extract, e    Extract HTML from production to prototype"
        echo "  serve, s      Serve prototype on local server (port 8080)"
        echo "  open, o       Open prototype in default browser"
        echo "  status, st    Show prototype status"
        echo "  menu, m       Show this menu (default)"
        echo ""
        echo "Examples:"
        echo "  $0 extract    # Extract latest UI from ESP32 code"
        echo "  $0 serve      # Test prototype locally"
        echo "  $0 open       # Quick preview"
        ;;
esac
