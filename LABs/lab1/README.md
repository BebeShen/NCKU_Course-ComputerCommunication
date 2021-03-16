# Lab1：Socket Programming

## Description

* 分別使用 UDP 和 TCP 實現 File Transfer的功能
* 程式碼需附上詳細的註解
* 傳送之檔案，大小需**大於200MB**
* 報告須包含以下記錄  (範例如下)
    * 每傳送25% 記錄一筆log
    * 總傳輸時間(ms)
    * 檔案大小
    * Tcp 和 udp 各執行過程

## Input Format

輸入為以下形式：
執行檔需有**5個參數**。

依序為`tcp/udp`, `傳送端/接收端`, `port`, `host`, `傳送檔案路徑`。

### example

```SHELL
$ ./lab1_file_transfer tcp send <ip> <port> test_input.txt
$./lab1_file_transfer tcp recv <ip> <port>

$./lab1_file_transfer udp send <ip> <port> test_input.txt
$./lab1_file_transfer udp recv <ip> <port>
```

# Information

## `TCP`

## `UDP`


