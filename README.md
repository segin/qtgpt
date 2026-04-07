# QtGPT - A Qt6-based GUI for Disaster Party

A C++/Qt6 graphical interface for the Disaster Party LLM library.

## Project Structure

```
qtgpt/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── .gitignore
├── .git/
└── src/
    ├── main.cpp            # Application entry point
    ├── QtGPT.cpp/.h        # Main application class (singleton, config, DP context)
    ├── ChatWidget.cpp/.h   # Chat interface with message history
    ├── PipeHandler.cpp/.h  # Async pipe communication (like MotifGPT)
    ├── ThreadWorker.cpp/.h # Background thread management
    ├── PluginManager.cpp/.h # Plugin/tool system manager
    ├── Plugin.cpp/.h       # Plugin base class
    ├── Tool.cpp/.h         # Tool definition and execution
    ├── WeatherTool.cpp/.h  # Weather tool implementation (built-in)
    ├── StockTool.cpp/.h    # Stock tool implementation (built-in)
    ├── utils.cpp/.h        # Utility functions (XDG paths, base64, etc.)
    └── PipeMessage.h       # Pipe message types
```

## Key Components

1. **QtGPT** - Singleton application class managing:
   - Disaster Party context initialization
   - Configuration (API keys, models, providers)
   - Global settings via XDG config paths

2. **ChatWidget** - Chat interface with:
   - QScrollArea for message history
   - QTextEdit input with Enter/Shift+Enter behavior
   - Image attachment support (base64 encoding)
   - Status bar with thinking/generating indicators

3. **PipeHandler** - Async communication:
   - Pipe-based message passing (like MotifGPT)
   - Token streaming, thinking indicators
   - Tool call handling
   - Model listing

4. **PluginManager** - Comprehensive tool system:
   - Dynamic .so plugin loading
   - Built-in tools (Weather, Stocks)
   - Tool JSON schema validation
   - Tool call with enhanced metadata (tool_id)

5. **Utilities** - Helper functions:
   - XDG config paths (~/.config/qtgpt/)
   - Base64 encoding/decoding
   - Image MIME type detection
   - System prompt generation

## Build Requirements

### Required Packages (Arch Linux)
- `qt6` (Qt6 Core, Gui, Widgets)
- `cjson` or `libcjson` - JSON library
- `libcurl` - HTTP client
- `disasterparty` - Disaster Party LLM library development headers

### Build Instructions

```bash
mkdir build && cd build
cmake ..
make
```

### Build Status

**⚠️ This is a work in progress.**

The current build is incomplete and has the following known issues:

1. **Qt Signal/Slot System**: The Qt6 auto-MOC may not be generating proper vtables for classes using `Q_OBJECT`. This causes linker errors for Qt signal/slot implementations.

2. **Missing Implementation**: Some Qt methods (e.g., `ChatWidget::show()`) need to be properly implemented.

3. **disasterparty Library**: The library must be installed and the headers must be accessible. If not found, the build will fail at the header include stage.

## TODO

- [ ] Implement ChatWidget.cpp with full UI
- [ ] Implement SettingsDialog.cpp with all tabs
- [ ] Add actual LLM request thread implementation
- [ ] Implement streaming callback for Disaster Party
- [ ] Add image attachment functionality
- [ ] Complete plugin dynamic loading
- [ ] Add more built-in tools

## License

This is a personal project - license to be determined.
