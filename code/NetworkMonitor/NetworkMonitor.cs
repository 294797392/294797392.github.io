using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NetworkMonitor
{
    public abstract class NetworkMonitor
    {
        public event Action<object, SessionBase> SessionCreated;

        public abstract void Start();

        public abstract void Stop();

        public abstract void SendMessage(byte[] data);
    }
}