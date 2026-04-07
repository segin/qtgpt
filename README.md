# QtGPT - A Qt6-based GUI for Disaster Party

A C++/Qt6 graphical interface for the Disaster Party LLM library, following the MotifGPT pattern.

## Project Structure

```
qtgpt/
├── main.cpp              # Application entry point
├── QtGPT.cpp/.h          # Main application class (singleton, config, DP context)
├── ChatWidget.cpp/.h     # Chat interface with message history
├── SettingsDialog.h      # Settings dialog (stub, needs implementation)
├── PipeHandler.cpp/.h    # Async pipe communication (like MotifGPT)
├── ThreadWorker.cpp/.h   # Background thread management
├── PluginManager.cpp/.h  # Plugin/tool system manager
├── Plugin.cpp/.h         # Plugin base class
├── Tool.cpp/.h           # Tool definition and execution
├── WeatherTool.cpp/.h    # Weather tool implementation
├── StockTool.cpp/.h      # Stock tool implementation
├── PipeMessage.h         # Pipe message types and structure
├── utils.cpp/.h          # Utility functions (XDG paths, base64, etc.)
├── Makefile              # Qt6 + C++17 build configuration
└── .gitignore
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

3. **SettingsDialog** - Configuration dialog with tabs:
   - Provider selection (Gemini, OpenAI, Anthropic)
   - API keys and model configuration
   - System prompt and history limits
   - Plugin directory management

4. **PipeHandler** - Async communication:
   - Pipe-based message passing (like MotifGPT)
   - Token streaming, thinking indicators
   - Tool call handling
   - Model listing

5. **PluginManager** - Comprehensive tool system:
   - Dynamic .so plugin loading
   - Built-in tools (Weather, Stocks)
   - Tool JSON schema validation
   - Tool call with enhanced metadata (tool_id)

6. **Utilities** - Helper functions:
   - XDG config paths (~/.config/qtgpt/)
   - Base64 encoding/decoding
   - Image MIME type detection
   - System prompt generation

## Build Requirements

- Qt6 (Widgets, Core)
- C++17 compiler
- libdisasterparty
- libcurl
- libcjson

## Build Instructions

```bash
mkdir build && cd build
cmake ..
make
```

Note: The build requires the following libraries to be installed:
- libdisasterparty (development headers)
- libcurl (development headers)
- libcjson (development headers)

If these libraries are not installed, the build will fail at the header include stage.

## TODO

- Implement ChatWidget.cpp with full UI
- Implement SettingsDialog.cpp with all tabs
- Add actual LLM request thread implementation
- Implement streaming callback for Disaster Party
- Add image attachment functionality
- Complete plugin dynamic loading
- Add more built-in tools
