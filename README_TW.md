# Im-Select-Imm

## 介紹

原版 im-select 可以切換輸入法，但是不能切換中英文。
這個版本可以在第二個參數中切換中英文和全形半形。
二進位檔位於 `此目錄/out/` 中

## 使用方式

### 取得目前輸入法的 key

```shell
/path/to/im-select-imm.exe
Out: [目前輸入法]-[輸入法的目前模式]
```

### 切換輸入法

```shell
/path/to/im-select-imm.exe [目標輸入法]
Or
/path/to/im-select-imm.exe [目標輸入法] [目標輸入法的目標模式]
Or
/path/to/im-select-imm.exe [目標輸入法]-[目標輸入法的目標模式] # 部分插件下只允許一個參數
Or
/path/to/im-select-imm.exe -d 50 [目標輸入法]-[目標輸入法的目標模式] # 在切換輸入法和模式之間插入延遲，預設 30ms，可能可以改善部分人無效的情況
```

### Shift 按鍵模式（`-s`）

預設情況下，模式切換使用 IMM32 `IMC_SETCONVERSIONMODE` API，適用於標準微軟輸入法。

對於純 TSF 架構的輸入法（例如嘸蝦米 J），請使用 `-s` 參數改以模擬 Shift 按鍵來切換模式：

```shell
/path/to/im-select-imm.exe -s [目標輸入法]-[目標輸入法的目標模式]
```

### 診斷模式

```shell
/path/to/im-select-imm.exe -v [目標輸入法]-[目標輸入法的目標模式] # 輸出診斷資訊
```

## 已測試輸入法

1. 新舊微軟注音
2. 新微軟日語輸入法
3. 嘸蝦米 J

### 對於微軟輸入法

```
Microsoft 舊版中文輸入法 (Win10 及更早版本)：
    0: 英文
    1: 中文
Microsoft 新版中文輸入法 (Win11)：
    0: 英文 / 半形
    1: 中文 / 半形
    1024: 英文 / 全形（實際上仍然為半形）
    1025: 中文 / 全形
```

## 搭配 im-select.nvim 使用

本工具可搭配 [keaising/im-select.nvim](https://github.com/keaising/im-select.nvim) 使用。以下為 Neovim 設定範例（lazy.nvim）：

### 微軟輸入法（預設 IMM32 模式）

```lua
{
    "keaising/im-select.nvim",
    opts = {
        default_im_select = "1033-0",
        default_command = { "/path/to/im-select-imm.exe" },
    },
}
```

### 嘸蝦米 J / 純 TSF 輸入法（Shift 按鍵模式）

```lua
{
    "keaising/im-select.nvim",
    opts = {
        default_im_select = "1028-0",
        default_command = { "/path/to/im-select-imm.exe", "-s" },
    },
}
```

> `-s` 參數會同時傳遞給 GET 和 SET 呼叫。對 GET（讀取目前模式）無影響，僅在 SET（切換模式）時生效。

## 備註

- 預設情況下，模式切換使用 IMM32 `IMC_SETCONVERSIONMODE` API，適用於標準微軟輸入法。
- 加上 `-s` 參數後，模式切換改為模擬 Shift 按鍵，適用於純 TSF 架構的輸入法（例如嘸蝦米 J）。
- 使用 `-v` 參數可以查看目前使用的切換方式，方便除錯。
