# 🚀 MICRONODE – A Mini JavaScript Runtime (Node.js Clone)

**MICRONODE** is a lightweight JavaScript runtime built using **QuickJS** and **C++**, inspired by Node.js. It's designed to help developers understand how JavaScript runtimes work under the hood by implementing core features from scratch.

---

## 🧠 What is MICRONODE?

MICRONODE is a hands-on educational project that replicates a minimal version of Node.js. It allows you to:

- Run JavaScript files using a terminal command  
- Understand how a JavaScript runtime connects JS code to system-level operations  
- Understand how Node.js modules are implemented
- How js code is executed by js engine under the hood

---

## ⚙️ Build Instructions

### 🔧 Requirements

- `clang++` (or any C++11-compatible compiler)  
- `QuickJS` (included as static library in `quickjs/libquickjs.a`)  
- Unix-like environment (Linux/macOS)  

### 🛠️ Build Command

```bash
clang++ -std=c++11 micronode.cpp quickjs/libquickjs.a -o micronode -pthread -ldl
```
This will compile the source and generate an executable named micronode.

### Usage
```bash
./micronode index.js
```

