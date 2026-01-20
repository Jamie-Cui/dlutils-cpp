# dlutils-cpp Documentation

This directory contains PlantUML diagrams documenting the architecture and usage of the dlutils-cpp library.

## Diagrams

### 1. Architecture Diagram (`architecture.puml`)

Class diagram showing the core components of dlutils-cpp:

- `DlFun<R, Args...>` - Template class for type-safe function pointer wrappers
- `DlLibBase` - Base class for dynamic library loading
- `internal::MakeString` - Utility for error message construction
- Example user class `LibCrypto` extending `DlLibBase`
- POSIX API integration (dlopen, dlsym, dlclose, dlerror)

**To render:**
```bash
plantuml docs/architecture.puml
# Or use online: https://plantuml.com/plantuml/uml/
```

### 2. Loading Flow Diagram (`loading-flow.puml`)

Flowchart showing the step-by-step process of loading a dynamic library and its symbols:

- Singleton instance creation
- Constructor calling `LoadAll()`
- `SelfDlOpen()` loading the shared library
- `SelfDlSym()` loading each function symbol
- Exception handling at each step

**To render:**
```bash
plantuml docs/loading-flow.puml
```

### 3. Usage Sequence Diagram (`usage-sequence.puml`)

Sequence diagram showing runtime interaction between components:

- Initialization phase (first `GetInstance()` call)
- Runtime function calls through `DlFun` wrappers
- Error handling when function pointers are null
- Cleanup on program exit

**To render:**
```bash
plantuml docs/usage-sequence.puml
```

## Viewing PlantUML Diagrams

### Online Viewer

1. Go to [PlantUML Online Editor](https://plantuml.com/plantuml/uml/)
2. Copy the contents of any `.puml` file
3. Paste into the editor to view/render

### Local Rendering

Install PlantUML:

**On macOS:**
```bash
brew install plantuml
```

**On Ubuntu/Debian:**
```bash
sudo apt-get install plantuml
```

**On Fedora:**
```bash
sudo dnf install plantuml
```

**Using Docker:**
```bash
docker run -v $PWD:/data -w /data plantuml/plantuml *.puml
```

Render all diagrams:
```bash
plantuml docs/*.puml
```

This will generate PNG images in the `docs/` directory.

### VS Code Extension

Install the [PlantUML extension](https://marketplace.visualstudio.com/items?itemName=jebbs.plantuml) for VS Code:

1. Install the extension
2. Open any `.puml` file
3. Press `Alt+D` to preview the diagram

### IntelliJ IDEA

PlantUML plugin is available:
1. Go to Settings → Plugins
2. Search for "PlantUML"
3. Install and restart IDE
4. Open `.puml` files to view diagrams
