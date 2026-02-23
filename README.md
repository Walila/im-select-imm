# Im-Select-Imm

## Introduction

The original im-select can only switch IME, but cannot toggle Chinese/English mode.
This version allows you to change IME mode via a second parameter.
The pre-built exe is under `thisdir/out/`.

[繁體中文介紹](./README_TW.md)

## Usage

### Get Current IME Key

```shell
/path/to/im-select-imm.exe
Out: [current IME]-[current Mode]
```

### Switch IME

```shell
/path/to/im-select-imm.exe [target IME]
Or
/path/to/im-select-imm.exe [target IME] [target IME Mode]
Or
/path/to/im-select-imm.exe [target IME]-[target IME Mode]
Or
/path/to/im-select-imm.exe -d 50 [target IME]-[target IME Mode] # add delay between switching IME and mode, default 30ms
```

### Verbose Mode

```shell
/path/to/im-select-imm.exe -v [target IME]-[target IME Mode] # print diagnostic info
```

## Tested IME

1. Microsoft Chinese IME (New, Windows 11)
2. Microsoft Chinese IME (Old, Windows 10)
3. Microsoft Japanese IME (New, Windows 11)
4. Boshiamy J (嘸蝦米 J)

## For Microsoft Chinese IME

```
For Microsoft Old Chinese IME (Win10 and Previous):
    0: English
    1: Chinese
For Microsoft New Chinese IME (Win11):
    0: English / Half Shape
    1: Chinese / Half Shape
    1024: English / Full Shape (in practice still half shape)
    1025: Chinese / Full Shape
```

## Notes

- For pure TSF IMEs (e.g. Boshiamy J), mode switching is done by simulating a Shift key press instead of using IMM32 API.
- Use `-v` flag to see which approach is being used for debugging.
