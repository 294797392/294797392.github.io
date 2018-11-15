using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;

namespace NetworkMonitor.Client
{
    public class TCPClient : NetworkClient
    {
        private TcpClient tcpClient;

        public void Start()
        {
            this.tcpClient = new TcpClient();
        }
    }
}