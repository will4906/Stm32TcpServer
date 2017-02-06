# Stm32   TCP客户端通信例程

## 串口

* `u8 SendUartData( u8 port, u8 *pBuf, u8 len )` : 发送数据
* `u8 ReadUartData( u8 port, u8 *pBuf )`：接收数据

注：port为串口号的宏定义，例程中只使能了串口2(UART_PORT_COM2)



## TCP通信

* `err_t SendTcpDataAsClient(u8 *buff, u16 length)`：发送数据
* `void OnTcpClientReceiveData(void *recvData)`：接收数据的回调函数。处理接收到数据的时候需要做的事情，相当于中断。

## 其他

其他功能请自行参照代码和注释进行使用。