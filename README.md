# Network-services(高并发网络通信服务框架)

## Version:select(v1.1.0_Beat)  
 + select模型  
 + 实现了多人群聊功能
    - Code
      * [server_Tcp_select_1.0](https://github.com/Acmen-Team/Network-services/blob/dev/src/services/v_server_2/server_Tcp_select_1.0.cpp)
      + [client_Tcp_select_1.0](https://github.com/Acmen-Team/Network-services/blob/dev/src/Client/v_client_2/client_Tcp_select_1.0.cpp)
### 其他
**docs**
```
正在制作完善中...
```
**dockerfile使用(待更新...)**
```
docker build . -t server
docker run -p 8989:8989 -d --name=server server
```
