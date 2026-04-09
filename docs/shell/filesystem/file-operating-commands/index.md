# File Operating Commands

`ls-drive` or `dir-drive` lists the available drives.

```console
L:/>ls-drive
 Drive  Format        Total
*L:     FAT12        524288
```

`mkdir` or `md` creates new directories. You can create multiple directories at once.

```text
L:/>mkdir dir1 dir2 dir3
L:/>mkdir dir1/dir1-1 dir1/dir1-2
L:/>mkdir dir2/dir2-1 dir2/dir2-2
```

Use `ls` or `dir` to display a list of directories.

```text
L:/>ls
d---- 2000-01-01 00:00:00        dir1/
d---- 2000-01-01 00:00:00        dir2/
d---- 2000-01-01 00:00:00        dir3/
-a--- 2000-01-01 00:00:00     77 README.txt
L:/>ls dir1
d---- 2000-01-01 00:00:00        dir1-1/
d---- 2000-01-01 00:00:00        dir1-2/
L:/>ls dir2
d---- 2000-01-01 00:00:00        dir2-1/
d---- 2000-01-01 00:00:00        dir2-2/
```

If all the timestamps of the files and directories are `2000-01-01 00:00:00`, it means that the Pico board does not have an RTC (Real-Time Clock) connected.

`cat` displays the contents of a file.

```text
L:/> cat README.txt
Welcome to pico-jxgLABO!
Type 'help' in the shell to see available commands.
```

`dump` or `d` displays the contents of a file in hexadecimal format.

```text
L:/>d README.txt
00  57 65 6C 63 6F 6D 65 20 74 6F 20 70 69 63 6F 2D
10  6A 78 67 4C 41 42 4F 21 0A 54 79 70 65 20 27 68
20  65 6C 70 27 20 69 6E 20 74 68 65 20 73 68 65 6C
30  6C 20 74 6F 20 73 65 65 20 61 76 61 69 6C 61 62
40  6C 65 20 63 6F 6D 6D 61 6E 64 73 2E 0A
```

The dump command can also display the contents of memory when specified with an address.

```text
L:/>d 0x10000000
10000000  00 B5 32 4B 21 20 58 60 98 68 02 21 88 43 98 60
10000010  D8 60 18 61 58 61 2E 4B 00 21 99 60 02 21 59 61
10000020  01 21 F0 22 99 50 2B 49 19 60 01 21 99 60 35 20
10000030  00 F0 44 F8 02 22 90 42 14 D0 06 21 19 66 00 F0
```

After executing the dump command with an address, the following execution of the dump command without an address continues to display the contents of memory from the next address.

```text
L:/>d
10000040  34 F8 19 6E 01 21 19 66 00 20 18 66 1A 66 00 F0
10000050  2C F8 19 6E 19 6E 19 6E 05 20 00 F0 2F F8 01 21
10000060  08 42 F9 D1 00 21 99 60 1B 49 19 60 00 21 59 60
10000070  1A 49 1B 48 01 60 01 21 99 60 EB 21 19 66 A0 21
```

Use `tree` to display the directory hierarchy in a tree format.

```text
L:/>tree
./
├── dir1/
│   ├── dir1-1/
│   └── dir1-2/
├── dir2/
│   ├── dir2-1/
│   └── dir2-2/
├── dir3/
└── README.txt
```

Use `cd` to change the current directory.

```text
L:/>cd dir1
L:/dir1>
```

`touch` updates the timestamp of an existing file or directory. If you specify a file that does not exist, it creates an empty file.

```text
L:/dir1>touch file1-1.txt file1-2.txt file1-3.txt
L:/dir1>ls
d---- 2000-01-01 00:00:00        dir1-1/
d---- 2000-01-01 00:00:00        dir1-2/
-a--- 2000-01-01 00:00:00      0 file1-1.txt
-a--- 2000-01-01 00:00:00      0 file1-2.txt
-a--- 2000-01-01 00:00:00      0 file1-3.txt
```

You can use the redirection feature to save standard output to a file. Using `>>` with redirection appends to the end of the file. In the following example, the output of the dump command is saved to a file.

```text
L:/dir1>cd /dir2
L:/dir2>d 0x10000000 > file2-1.txt
L:/dir2>d > file2-2.txt
L:/dir2>d >> file2-2.txt
L:/dir2>ls
d---- 2000-01-01 00:00:00        dir2-1/
d---- 2000-01-01 00:00:00        dir2-2/
-a--- 2000-01-01 00:00:00    232 file2-1.txt
-a--- 2000-01-01 00:00:00    464 file2-2.txt
```

Use `cat` to display the contents of a file.

```text
L:/dir2>cat file2-1.txt
10000000  00 B5 32 4B 21 20 58 60 98 68 02 21 88 43 98 60
10000010  D8 60 18 61 58 61 2E 4B 00 21 99 60 02 21 59 61
10000020  01 21 F0 22 99 50 2B 49 19 60 01 21 99 60 35 20
10000030  00 F0 44 F8 02 22 90 42 14 D0 06 21 19 66 00 F0
L:/dir2>cat file2-2.txt
10000040  34 F8 19 6E 01 21 19 66 00 20 18 66 1A 66 00 F0
10000050  2C F8 19 6E 19 6E 19 6E 05 20 00 F0 2F F8 01 21
10000060  08 42 F9 D1 00 21 99 60 1B 49 19 60 00 21 59 60
10000070  1A 49 1B 48 01 60 01 21 99 60 EB 21 19 66 A0 21
10000080  19 66 00 F0 12 F8 00 21 99 60 16 49 14 48 01 60
10000090  01 21 99 60 01 BC 00 28 00 D0 00 47 12 48 13 49
100000A0  08 60 03 C8 80 F3 08 88 08 47 03 B5 99 6A 04 20
100000B0  01 42 FB D0 01 20 01 42 F8 D1 03 BD 02 B5 18 66
```

If you specify multiple files with `cat`, their contents are concatenated and displayed (the name "cat" comes from "concatenate").

```text
L:/dir2>cat file2-1.txt file2-2.txt
10000000  00 B5 32 4B 21 20 58 60 98 68 02 21 88 43 98 60
10000010  D8 60 18 61 58 61 2E 4B 00 21 99 60 02 21 59 61
10000020  01 21 F0 22 99 50 2B 49 19 60 01 21 99 60 35 20
10000030  00 F0 44 F8 02 22 90 42 14 D0 06 21 19 66 00 F0
10000040  34 F8 19 6E 01 21 19 66 00 20 18 66 1A 66 00 F0
10000050  2C F8 19 6E 19 6E 19 6E 05 20 00 F0 2F F8 01 21
10000060  08 42 F9 D1 00 21 99 60 1B 49 19 60 00 21 59 60
10000070  1A 49 1B 48 01 60 01 21 99 60 EB 21 19 66 A0 21
10000080  19 66 00 F0 12 F8 00 21 99 60 16 49 14 48 01 60
10000090  01 21 99 60 01 BC 00 28 00 D0 00 47 12 48 13 49
100000A0  08 60 03 C8 80 F3 08 88 08 47 03 B5 99 6A 04 20
100000B0  01 42 FB D0 01 20 01 42 F8 D1 03 BD 02 B5 18 66
```

If you run `cat` without arguments, it outputs keyboard input to standard output. By using redirection like `cat > filename`, you can easily create a text file on the Pico board. When you are done entering text, press `Ctrl-C` to finish.


```text
L:/dir2>cat > file2-3.txt
This is the first human-written file.
^C
L:/dir2>cat > file2-4.txt
This is the second human-written file.
^C
L:/dir2>ls
d---- 2000-01-01 00:00:00        dir2-1/
d---- 2000-01-01 00:00:00        dir2-2/
-a--- 2000-01-01 00:00:00    232 file2-1.txt
-a--- 2000-01-01 00:00:00    464 file2-2.txt
-a--- 2000-01-01 00:00:00     38 file2-3.txt
-a--- 2000-01-01 00:00:00     39 file2-4.txt
```

By default, `ls` displays files and directories sorted by name. With the `ls --sort=WORD` option (`WORD` can be `name`, `size`, or `time`), you can sort by date or size. The `ls -r` option sorts in reverse order.

```text
L:/dir2>ls --sort=size
d---- 2000-01-01 00:00:00        dir2-1/
d---- 2000-01-01 00:00:00        dir2-2/
-a--- 2000-01-01 00:00:00     38 file2-3.txt
-a--- 2000-01-01 00:00:00     39 file2-4.txt
-a--- 2000-01-01 00:00:00    232 file2-1.txt
-a--- 2000-01-01 00:00:00    464 file2-2.txt
L:/dir2>ls --sort=size -r
d---- 2000-01-01 00:00:00        dir2-1/
d---- 2000-01-01 00:00:00        dir2-2/
-a--- 2000-01-01 00:00:00    464 file2-2.txt
-a--- 2000-01-01 00:00:00    232 file2-1.txt
-a--- 2000-01-01 00:00:00     38 file2-4.txt
-a--- 2000-01-01 00:00:00     37 file2-3.txt
```

Use `cp` to copy files. You can copy multiple files and use wildcards.

```text
L:/dir2>cd /
L:/>cp dir1/file1-1.txt dir1/file1-2.txt dir3
L:/>cp dir2/*.txt dir3
L:/>ls dir3
-a--- 2000-01-01 00:00:00      0 file1-1.txt
-a--- 2000-01-01 00:00:00      0 file1-2.txt
-a--- 2000-01-01 00:00:00    232 file2-1.txt
-a--- 2000-01-01 00:00:00    464 file2-2.txt
-a--- 2000-01-01 00:00:00     37 file2-3.txt
-a--- 2000-01-01 00:00:00     38 file2-4.txt
```

With the `cp -r` option, you can copy entire directories.

```text
L:/>cp -r dir1 dir2
L:/>tree dir2
dir2/
├── dir1/
│   ├── dir1-1/
│   ├── dir1-2/
│   ├── file1-1.txt
│   ├── file1-2.txt
│   └── file1-3.txt
├── dir2-1/
├── dir2-2/
├── file2-1.txt
├── file2-2.txt
├── file2-3.txt
└── file2-4.txt
L:/>cp -r dir1 dir4
L:/>ls
d---- 2000-01-01 00:00:00        dir1/
d---- 2000-01-01 00:00:00        dir2/
d---- 2000-01-01 00:00:00        dir3/
d---- 2000-01-01 00:00:00        dir4/
a--- 2000-01-01 00:00:00     77 README.txt
```

Use `mv` to rename or move files and directories.

```text
L:/>mv dir1/file1-1.txt dir2
L:/>ls dir1
d---- 2000-01-01 00:00:00        dir1-1/
d---- 2000-01-01 00:00:00        dir1-2/
-a--- 2000-01-01 00:00:00      0 file1-2.txt
-a--- 2000-01-01 00:00:00      0 file1-3.txt
L:/>ls dir2
d---- 2000-01-01 00:00:00        dir2-1/
d---- 2000-01-01 00:00:00        dir2-2/
-a--- 2000-01-01 00:00:00      0 file1-1.txt
-a--- 2000-01-01 00:00:00    232 file2-1.txt
-a--- 2000-01-01 00:00:00    464 file2-2.txt
-a--- 2000-01-01 00:00:00     38 file2-3.txt
-a--- 2000-01-01 00:00:00     39 file2-4.txt
```

Use `rm` to delete files or directories. You can specify multiple files and use wildcards. The `rm -r` option deletes entire directories.

```text
L:/>rm dir1/file1-2.txt
L:/>ls dir1
d---- 2000-01-01 00:00:00        dir1-1/
d---- 2000-01-01 00:00:00        dir1-2/
-a--- 2000-01-01 00:00:00      0 file1-3.txt
L:/>rm dir2/*.txt
L:/>ls dir2
d---- 2000-01-01 00:00:00        dir1/
d---- 2000-01-01 00:00:00        dir2-1/
d---- 2000-01-01 00:00:00        dir2-2/
L:/>rm -r dir1 dir2
L:/>ls
d---- 2000-01-01 00:00:00        dir3/
d---- 2000-01-01 00:00:00        dir4/
-a--- 2000-01-01 00:00:00     77 README.txt
```
