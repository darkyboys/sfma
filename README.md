# SFMA — Simple File Memory Allocator

A lightweight C++ library that provides file-backed, block-based memory allocation and access.

SFMA (Simple File Memory Allocator) allows you to treat a regular file as a simple memory storage system. It provides an interface for reading and writing fixed-size blocks using block indices, while handling file offsets, binary I/O, padding, and storage expansion internally.

## Features

- **File-backed storage:** Uses regular files as the underlying storage medium.
- **Block-based allocation:** Organizes storage into fixed-size blocks.
- **Simple interface:** Provides straightforward `get()` and `set()` operations.
- **Automatic file creation:** Creates the backing file if it does not exist.
- **Binary storage:** Supports storing arbitrary binary data.
- **Automatic padding:** Pads data with null bytes (`\0`) when it is smaller than the configured block size.
- **Automatic truncation:** Ignores excess input when data exceeds the block size.
- **Automatic expansion:** Initializes missing regions with null bytes when writing beyond the current end of the file.
- **Memory validation:** Detects invalid `MemoryFile` instances and reports errors.
- **Lightweight implementation:** Uses the C++ standard library without external dependencies.

## Project Structure

```text
.
├── LICENSE
├── README.md
├── sfma.cc
├── sfma.hh
└── test.cc
```

| File | Description |
|---|---|
| `LICENSE` | CC0 1.0 Universal license. |
| `README.md` | Project documentation. |
| `sfma.cc` | Library implementation. |
| `sfma.hh` | Public header and API declarations. |
| `test.cc` | Test and usage code. |

## Requirements

- C++17 or later.
- A compiler with C++17 support (GCC, Clang, or MSVC).
- Standard C++ filesystem support.

## Installation and Compilation

Clone the repository:

```bash
git clone <repository-url>
cd <repository-directory>
```

Compile SFMA with your application:

```bash
g++ -std=c++17 sfma.cc test.cc -o sfma
```

Run the executable:

```bash
./sfma
```

To integrate SFMA into another project, include `sfma.hh` and compile `sfma.cc` alongside your application source files.

## Usage

### 1. Include the Header

```cpp
#include "sfma.hh"
#include <iostream>
```

### 2. Configure the Block Size

SFMA provides a global block-size variable:

```cpp
sfma::SFMA_BLOCK_SIZE = 16;
```

The block size determines how many bytes each block occupies.

The default block size is `1` byte.

**Important:** Configure the block size before performing read or write operations. All `MemoryFile` instances share this global setting.

### 3. Create a MemoryFile

```cpp
sfma::MemoryFile memory("memory.dat");
```

The constructor associates a `MemoryFile` instance with the specified file.

- If the file exists and is a regular file, SFMA uses it.
- If the file does not exist, SFMA attempts to create it.
- If the file cannot be created or is not a regular file, the constructor throws `std::runtime_error`.

### 4. Write Data

```cpp
memory.set(0, "Hello, SFMA!");
```

Writes data to block `0`.

SFMA automatically adjusts the input to match the configured block size:

- If the input is shorter than the block size, it is padded with null bytes.
- If the input exceeds the block size, the excess data is discarded.
- If the target block is beyond the current end of the file, the required space is initialized with null bytes.
- If the target block already exists, its contents are overwritten.

### 5. Read Data

```cpp
sfma::Data result = memory.get(0);

if (result.exists) {
    std::cout << "Data: " << result.data << '\n';
} else {
    std::cout << "Block does not exist.\n";
}
```

The `get()` function returns an `sfma::Data` object containing the existence status and contents of the requested block.

## API Reference

### `SFMA_BLOCK_SIZE`

```cpp
extern int SFMA_BLOCK_SIZE;
```

Controls the size of each storage block in bytes.

Default value:

```cpp
int SFMA_BLOCK_SIZE = 1;
```

This variable is global and shared between all `MemoryFile` instances.

### `Data`

```cpp
struct Data {
    bool exists = false;
    std::string data;
};
```

Represents the result of a block read.

| Member | Description |
|---|---|
| `exists` | Indicates whether the requested block exists. |
| `data` | Contains the contents of the requested block. |

### `MemoryFile`

```cpp
class MemoryFile {
public:
    MemoryFile(const std::string& path);

    Data get(unsigned long long index);
    void set(unsigned long long index, const std::string& data);

    ~MemoryFile();
};
```

Represents a file-backed memory storage instance.

#### Constructor

```cpp
MemoryFile(const std::string& path);
```

Creates a `MemoryFile` associated with the specified file path.

The constructor attempts to create the backing file if it does not exist.

Throws `std::runtime_error` if the backing file cannot be created or accessed as a regular file.

#### `get()`

```cpp
Data get(unsigned long long index);
```

Reads a block at the specified index.

**Possible outcomes:**

- Returns `Data{false, ""}` if the requested block starts beyond the end of the file.
- Returns `Data{true, data}` if the requested block exists.
- Throws `sfma::CORRUPTED_BLOCK` if the file ends in the middle of the requested block.
- Throws `sfma::MEMORY_FILE_NOT_FOUND` if the backing file cannot be opened.
- Throws `sfma::INVALID_MEMORY` if the `MemoryFile` instance is invalid.

#### `set()`

```cpp
void set(unsigned long long index, const std::string& data);
```

Writes data to the specified block.

**Behavior:**

- Truncates input that exceeds the configured block size.
- Pads shorter input with null bytes.
- Extends the file with null bytes when writing beyond its current end.
- Overwrites existing blocks when the target block is already present.

Throws `sfma::INVALID_MEMORY` if the `MemoryFile` instance is invalid.

#### Destructor

```cpp
~MemoryFile();
```

Releases the object's internal state and invalidates the instance.

## Error Handling

### Error Codes

```cpp
enum Error {
    CORRUPTED_BLOCK,
    MEMORY_FILE_NOT_FOUND,
    INVALID_MEMORY
};
```

SFMA defines three error codes:

| Error | Description | Function |
|---|---|---|
| `CORRUPTED_BLOCK` | The requested block is only partially present in the backing file. | `get()` |
| `MEMORY_FILE_NOT_FOUND` | The backing file cannot be opened for reading. | `get()` |
| `INVALID_MEMORY` | The `MemoryFile` instance is invalid (`is_valid == false`). | `get()`, `set()` |

### Error Conditions

**1. CORRUPTED_BLOCK**

Thrown by `get()` when a requested block begins within the file but does not contain enough remaining bytes to represent a complete block.

**2. MEMORY_FILE_NOT_FOUND**

Thrown by `get()` when the backing file cannot be opened for reading.

**3. INVALID_MEMORY**

Thrown by both `get()` and `set()` when the `MemoryFile` instance is invalid.

This prevents read and write operations from being performed on an invalid memory instance.

### Error Handling Example

```cpp
try {
    sfma::Data result = memory.get(0);

    if (result.exists) {
        std::cout << "Data: " << result.data << '\n';
    }

    memory.set(1, "Hello");
}
catch (sfma::Error error) {
    switch (error) {
        case sfma::CORRUPTED_BLOCK:
            std::cerr << "Corrupted block.\n";
            break;

        case sfma::MEMORY_FILE_NOT_FOUND:
            std::cerr << "Memory file not found.\n";
            break;

        case sfma::INVALID_MEMORY:
            std::cerr << "Invalid memory instance.\n";
            break;
    }
}
```

**Note:** SFMA errors are thrown as enum values rather than standard C++ exception objects. The constructor separately uses `std::runtime_error`.

## How Block Addressing Works

SFMA calculates the byte offset of a block using:

```cpp
block_offset = index * SFMA_BLOCK_SIZE;
```

For example, with a block size of 4 bytes:

| Block Index | Byte Offset | Byte Range |
|---|---:|---|
| 0 | 0 | 0–3 |
| 1 | 4 | 4–7 |
| 2 | 8 | 8–11 |
| 3 | 12 | 12–15 |
| 4 | 16 | 16–19 |

This allows storage to be accessed through fixed-size blocks instead of manually calculated byte offsets.

## Important Notes and Limitations

- `SFMA_BLOCK_SIZE` is global and shared between all `MemoryFile` instances.
- The block size should remain consistent when accessing an existing storage file.
- Data is stored in fixed-size blocks rather than variable-length records.
- Null bytes are used for padding, so returned strings may contain embedded null characters.
- SFMA does not currently provide thread-safety or concurrent access protection.
- The implementation does not provide transactions, checksums, encryption, or automatic recovery.
- The backing file should be treated as binary data.
- There is no explicit flush or synchronization API for applications requiring crash-safe persistence.
- Applications should handle invalid memory instances and catch the errors thrown by SFMA.

## Testing

The repository includes `test.cc` for testing SFMA.

Compile and run:

```bash
g++ -std=c++17 sfma.cc test.cc -o sfma
./sfma
```

## License

SFMA is released under **CC0 1.0 Universal**.

The software is dedicated to the public domain to the greatest extent permitted by law.

You are free to:

- Use the software for personal or commercial purposes.
- Copy and redistribute the software.
- Modify and adapt the source code.
- Incorporate it into other projects.

No permission or attribution is required, although attribution is appreciated.

See [LICENSE](LICENSE) for the complete legal text.

More information: https://creativecommons.org/publicdomain/zero/1.0/

---

**SFMA — Simple File Memory Allocator**

*A simple approach to file-backed memory allocation in C++.*