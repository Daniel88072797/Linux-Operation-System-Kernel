## Linux-Operation-System-Kernel

### Homework1: Linux Kernel Upgrade
安裝任一個Linux發行版本(根據所安裝的Linux系統中的kernel版本下載相同major版本的source code並編譯安裝)，安裝完後登入系統執行，cat /etc/\*rele\* ; uname -a

### Homework2: Kernel Module
分別 編譯並載入  hello 與   hellop 這兩個kernel module ，並另外撰寫一個hellop3 可傳入 整數a與整數b和字串c 在載入時透過printk印出a*b次的c字串
以下列依序指令載入與卸除  
uname -a  
insmod hello.ko  
insmod hellop.ko howmany=5 whom="IIS"  
insmod hellop3.ko a=3 b=2 c=module_text  
lsmod | grep hello  
rmmod hellop  
rmmod hello  
rmmod hellop3  
lsmod | grep hello

### Homework3: Get process UID
參考process.c與kernel source內的資料結構  
列出每一個process的pid,uid,comm

### Homework4: mykpid module
參考myproc的寫入，撰寫一個mykpid的module讓他達成  
echo PID > /proc/mykpid  
echo INT > /proc/mykpid  
執行完上兩行後  
PID會收到SIGINT的signal

### Homework5: myfs
1.將檔案系統的跟目錄改為下列結構  
/--+ input (dir)  
   |   |  
   |   +-- a (file)  
   |   +-- b (file)  
   |  
   \+ output (dir)  
       |  
       +-- add (file)  
       +-- sub (file)  
2.可以透過 echo 數字 > /input/a 和 echo 數字 >/input/b 來設定a和b的值，數值大小0~255之間  
3.可以透過 cat /output/add取得a+b的值，透過cat /output/sub取得a-b的值

