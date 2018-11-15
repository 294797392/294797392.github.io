using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace NetworkMonitor
{
    /// <summary>
    /// 监控TCP网络的状态
    /// 调试TCP通信的工具
    /// </summary>
    public class TCPNetworkMonitor : NetworkMonitor
    {
        private static log4net.ILog logger = log4net.LogManager.GetLogger("TCPNetworkMonitor");

        private const int ListenPort = 8888;

        private Socket socket;

        public override void SendMessage(byte[] data)
        {
            throw new NotImplementedException();
        }

        public override void Start()
        {
            this.socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            this.socket.Bind(new IPEndPoint(IPAddress.Any, ListenPort));
            this.socket.Listen(10);

            Task.Factory.StartNew(() => 
            {
                Socket clientSocket = this.socket.Accept();
            });
        }

        public override void Stop()
        {
            throw new NotImplementedException();
        }
    }
}
