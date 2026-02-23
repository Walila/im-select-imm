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

### Shift Key Mode (`-s`)

By default, mode switching uses the IMM32 `IMC_SETCONVERSIONMODE` API, which works for standard Microsoft IMEs.

For pure TSF IMEs (e.g. Boshiamy J), use the `-s` flag to switch mode via Shift key simulation instead:

```shell
/path/to/im-select-imm.exe -s [target IME]-[target IME Mode]
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

## Usage with im-select.nvim

This tool works with [keaising/im-select.nvim](https://github.com/keaising/im-select.nvim). Example configuration in Neovim (lazy.nvim):

### Microsoft IME (default IMM32 mode)

```lua
{
    "keaising/im-select.nvim",
    opts = {
        default_im_select = "1033-0",
        default_command = { "/path/to/im-select-imm.exe" },
    },
}
```

### Boshiamy J / Pure TSF IMEs (Shift key mode)

```lua
{
    "keaising/im-select.nvim",
    opts = {
        default_im_select = "1028-0",
        default_command = { "/path/to/im-select-imm.exe", "-s" },
    },
}
```

> The `-s` flag is passed to both GET and SET calls. It has no effect on GET (reading current mode) and only affects SET (mode switching).

## Notes

- By default, mode switching uses the IMM32 `IMC_SETCONVERSIONMODE` API, which works for standard Microsoft IMEs.
- With the `-s` flag, mode switching is done by simulating a Shift key press, which is required for pure TSF IMEs (e.g. Boshiamy J).
- Use `-v` flag to see which approach is being used for debugging.
