

# KFS
Welcome to **KFS, Krazy File System**

<img src="Data/KFS.png">

This is a tiny file system project based on the Linux file system, with some of my own interpretation and style. The project adds color and organization to the file system.

## Getting Started

To get started with the project, clone the repository and build the project using the provided Makefile.


```bash
git clone https://github.com/yourusername/KrazyFileSystem.git
cd KrazyFileSystem
```

## Build

🗔 Windows 
```cmd
./build.bat
```
🗔 Linux
```bash
make
```

Usage
After building the project, you can run the application using the following command:

```bash
./build/main.exe {file}.bin [optional]flags
```

### Runing a KFS first time.

Stating you system every time will load the section formated to KFS (must have at least one).

> [!WARNING]
> Bloack Device must be formated with PiPS or without by using the flag **-nP** at start.


```
Loading KFS...
        ,--.
    ,--/  /|     ,---,.  .--.--.
 ,---,': / '   ,'  .' | /  /    '.
 :   : '/ /  ,---.'   ||  :  /`. /
 |   '   ,   |   |   .';  |  |--`
 '   |  /    :   :  :  |  :  ;_
 |   ;  ;    :   |  |-, \  \    `.
 :   '   \   |   :  ;/|  `----.   \
 |   |    '  |   |   .'  __ \  \  |
 '   : |.  \ |   :  '   /  /`--'  /
 |   | '_\.' |   |  |  '--'.     /
 '   : |     |   :  \    `--'---'
 ;   |,'     |   | ,'
 '---'       `----'
Welcome to KFS, Krazy File System

C/:$>
```

The system will start at the first parition found with KFS. the partition can be change using ***mnt {parition letter}***

This KFS support the following commands:

- **touch** - create a file in a given path
  - Usage: touch {path}\\{file_name}
- **mkdir** - create a folder in a given path
  - Usage: mkdir {path}\\{folder_name}
- **ls** - list all the file in a given path
  - Usage: ls {path}
- **edit** - edit the content of a file in a given path
  - Usage: edit {path}\\{file_name}
- **cat** - print the content of a file in a given path
  - Usage: cat {path}\\{file_name}
- **cd** - change current dir to a given path
  - Usage: cd {path}
- **tree** - show all file in the corrent partition in a tree like display.
  - Usage: tree
- **mnt** - change mount (parition).
  - Usage: mnt {partiton name}.
  - Note: PiPS must be enabled.
- **exit** - exit the system.
  - Usage: exit

### FLAGS:

| Command  |  Flag     | Output                                   |
| -------- | :-------: |------------------------------------------|
| LS       | -a        | show files and folders starting with .   |
| LS       | -l        | show files and folder more information . |

## Folder Structer

    🗁Build
      ⚙ main.exe
    🗁Data
      🖽 FeaNix.png
      🖽 KFS.png
      🖽 PiPS.png
    🗁Src
        🗎 FileTree.c
        🗎 KFS.c
        🗎 Main.c 
        🗁Header
          🗎 FileTree.h
          🗎 KFS.h

### KFS

I now this is the name of the project lol, how ever KFS is the code file load the kfs and update all need information to maintain the KFS in the binary file.

### FileTree
Code file for generating a nTree (n Children), and manage the tree adding, new child to the tree.
This code let the user call  ***Tree*** to display all the files and folder as a tree.

```output
C/:$> tree
├🗁 /
├─🗁 folderA
├───🗁 folderB
└─────🗎 fileA
```

# PiPS

<img src="Data/PiPS.png"><br>
This project use **PiPS, Pizza Partition Center**

A partition system. PiPs let the user define the layout of his Disk and create a partition


# FeaNix

<img src="Data/FeaNix.png"><br>
This project use <strong>FeaNix</strong>

A light weight rubust text editor.

Contributing
Contributions are welcome! Please read the CONTRIBUTING.md file for guidelines on how to contribute to this project.


License
This project is licensed under the MIT License - see the LICENSE file for details.
The project is organized as follows:

