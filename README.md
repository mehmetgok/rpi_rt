# rpi_rt

preempt-rt çekirdekli raspberry pi ile gerçek zamanlı thread testi için geliştirilen uygulamalar.

----------

**rttest.c:**


Bu program dosyası ile üzerinde çalışılan çekirdeğin realtime olup olmadığı sorgulanmaktadır.

**rt_sqlite.c**

Bu program ile rt ile çalışan bir thread oluşturulmuştur. Bu thread içindeki döngü 5 ms aralıkla dönmektedir. Her dönüşte Linux microsaniye olarak zaman olarak alınmakta ve sqlite tablosuna bu süre yazılmaktadır. sqlite veri yazma işleminin uzun sürmemesi için **transaction** ve **prepared statement** kullanılmaktadır.



Bu programı derlemek için gereken komut satırı:

gcc rt_sqlite.c -o rt_sqlite -lpthread -lrt -lsqlite3

**Referanslar:**

http://stackoverflow.com/questions/18776812/problems-linking-to-sqlite3-h-with-gcc




