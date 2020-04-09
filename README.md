# Network-services
## 高并发网络服务(Network services)  
**最新版1.1**  
  c/s模型 一对一  
	服务端使用端口：8989  
  ### 优化
  * 数据包传输优化

### dockerfile使用
```
docker build . -t server
docker run -p 8989:8989 -d --name=server server
```
