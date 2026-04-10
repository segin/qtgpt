# Architecture Overview
This document serves as a comprehensive guide to the QtGPT codebase architecture, enabling efficient navigation and contribution.

## 1. Project Structure
The project is a C++/Qt application organized primarily within the `src/` directory.

/home/segin/qtgpt/
├── CMakeLists.txt        # Build system configuration
├── README.md             # Project overview
├── build/                # Build artifacts
└── src/                  # Main source code
    ├── main.cpp          # Application entry point
    ├── QtGPT.cpp/h       # Core application logic and orchestrator (Singleton)
    ├── ChatWidget.cpp/h  # Main chat interface (QMainWindow)
    ├── MenuBar.cpp/h     # Application menu bar
    ├── SettingsDialog.cpp/h # Configuration interface
    ├── ChatHistoryViewer.cpp/h # Viewing saved conversations
    ├── PipeHandler.cpp/h # Asynchronous communication handler
    ├── ThreadWorker.cpp/h # Background thread management for LLM tasks
    ├── PluginManager.cpp/h # Dynamic loading and management of tools
    ├── Plugin.cpp/h      # Base class for plugins
    ├── Tool.cpp/h        # Base class for tools/functions
    ├── WeatherTool.cpp/h # Built-in Weather tool
    ├── StockTool.cpp/h   # Built-in Stock tool
    ├── utils.cpp/h       # Common utilities
    ├── PipeMessage.h     # Protocol definitions for IPC
    └── disasterparty_wrapper.h # C++ wrapper for the Disaster Party C library

## 2. High-Level System Diagram
The application follows a Model-View-Controller (MVC) like pattern where `QtGPT` acts as the controller/model orchestrator and `ChatWidget` as the primary view.

[User] <--> [ChatWidget (UI)] <--> [QtGPT (Orchestrator)] <--> [CompletionWorker (Thread)] <--> [Disaster Party (Library)] <--> [External AI APIs]
                                        |
                                        +--> [PluginManager] <--> [External/Built-in Tools]

## 3. Core Components

### 3.1. Frontend
**Name:** QtGPT UI
**Description:** The user interface built with Qt6, providing a modern chat experience. It handles message rendering (with Markdown support), input processing, and configuration management.
**Technologies:** Qt6 (Widgets, Network), C++17.

### 3.2. Orchestrator
**Name:** QtGPT Class
**Description:** A singleton object that manages the application state, chat history, and configuration. It coordinates between the UI, the background workers, and the plugin system.
**Technologies:** C++, Qt Signals/Slots.

### 3.3. Backend Workers
**Name:** CompletionWorker
**Description:** A specialized `ThreadWorker` that executes LLM requests using the `disasterparty` library in a background thread to keep the UI responsive. It handles streaming tokens and thinking/reasoning deltas.
**Technologies:** QThread, Disaster Party Library.

## 4. Data Stores

### 4.1. Configuration
**Name:** settings.conf
**Type:** INI-style configuration file.
**Purpose:** Stores API keys, model preferences, UI settings, and plugin paths.
**Location:** Located in the user's standard config directory (e.g., `~/.config/opencode/qtgpt/settings.conf`).

### 4.2. Conversation History
**Type:** In-memory (transient) and JSON (persistent).
**Purpose:** Chat history is maintained in-memory during a session and can be serialized to JSON files for persistence.

## 5. External Integrations / APIs

**Service:** Disaster Party Library
**Purpose:** A C library that provides a unified interface for multiple LLM providers.
**Integration:** Linked as a shared library (`libdisasterparty.so`).

**Service:** Google Gemini / OpenAI / Anthropic
**Purpose:** LLM providers for generating responses.
**Integration:** Accessed via the Disaster Party library over HTTPS.

## 6. Deployment & Infrastructure
**Build System:** CMake.
**Platform:** Primarily Linux (Arch Linux tested).
**Dependencies:** Qt6, libdisasterparty, libcjson, libcurl.

## 7. Security Considerations
**Authentication:** API keys are stored in a local configuration file.
**Data Encryption:** Communication with AI providers is performed over TLS (via libcurl in Disaster Party).
**Privacy:** Conversions are stored locally; no telemetry is currently implemented.

## 8. Development & Testing Environment
**Local Setup:** 
1. Install dependencies (Qt6, cmake, disasterparty).
2. `mkdir build && cd build`
3. `cmake .. && make`
**Testing:** Basic unit tests and manual verification of UI parity.

## 9. Future Considerations / Roadmap
- Implement full GFM (GitHub Flavored Markdown) support.
- Expand the plugin system to support more tool types.
- Add support for local models (via Ollama or similar).
- Implement collapsible reasoning blocks.

## 10. Project Identification
**Project Name:** QtGPT
**Repository URL:** https://github.com/segin/qtgpt
**Primary Contact:** Kirn Gill II (segin)
**Date of Last Update:** 2026-04-09

## 11. Glossary / Acronyms
- **LLM:** Large Language Model.
- **DP:** Disaster Party (The underlying LLM abstraction library).
- **IPC:** Inter-Process Communication (handled via PipeHandler).
- **SSE:** Server-Sent Events (used for streaming completions).
