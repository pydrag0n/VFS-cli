# VFS-cli

## User

```ps

user -c username passwd permission
user -l
user -s username passwd

```

## File

```ps

file filename -c
file filename -r
file filename -w mode(w or a) "content"
file filename -i

```

## Build

```ps
gcc main.c commands.c include/user.c include/file.c -o vfs_cli
```

## Example

```ps

<vfs:root> user -c user1 1234 3
User created successfully
<vfs:root> user -l
root root 4
user1 1234 3
<vfs:root> user -s user1 1234
User authenticated successfully. User index: 1
<vfs:user1> file t -i
<vfs:user1> file f -c
File created successfully: f
<vfs:user1> file f -w w "hello world ! "
File written successfully: f
<vfs:user1> file f -r
hello world !
<vfs:user1> file f -w a "new text"
File written successfully: f
<vfs:user1> file f -l
argument not found
<vfs:user1> file f -r
hello world ! new text
<vfs:user1> file f -i
f Dec  7 2024  15:45 23
<vfs:user1>

```
