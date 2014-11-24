using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.Serialization;

namespace IGSMLib
{
    [DataContract]
    public class IGConnectionStatusDataContract
    {
        [DataMember(Name = "Type", IsRequired = true)]
    	public string Type { get; set; }

        [DataMember(Name = "User", IsRequired = false)]
    	public string User { get; set; }

        [DataMember(Name = "Status", IsRequired = false)]
    	public string Status { get; set; }

        [DataMember(Name = "SessionTime", IsRequired = false)]
    	public string SessionTime { get; set; }

        [DataMember(Name = "IdleTime", IsRequired = false)]
    	public string IdleTime { get; set; }

        [DataMember(Name = "Images", IsRequired = false)]
    	public string Images { get; set; }
    }

    [DataContract]
    public class IGConnectionListStatusDataContract
    {
        [DataMember(Name = "NbConnections", IsRequired = true)]
    	public string NbConnections { get; set; }

        [DataMember(Name = "List", IsRequired = true)]
    	public List<IGConnectionStatusDataContract> List { get; set; }

        [DataMember(Name = "NbMaxConnections", IsRequired = false)]
    	public string NbMaxConnections { get; set; }

        [DataMember(Name = "NbUserConnections", IsRequired = false)]
    	public string NbUserConnections { get; set; }

        [DataMember(Name = "NbAvailConnections", IsRequired = false)]
    	public string NbAvailConnections { get; set; }
    }

    [DataContract]
    public class IGServerStatusDataContract
    {
        [DataMember(Name = "Type", IsRequired = true)]
    	public string Type { get; set; }

        [DataMember(Name = "Address", IsRequired = true)]
    	public string Address { get; set; }

        [DataMember(Name = "Status", IsRequired = true)]
    	public string Status { get; set; }

        [DataMember(Name = "Connection", IsRequired = false)]
    	public IGConnectionStatusDataContract Connection { get; set; }

        [DataMember(Name = "ServerManager", IsRequired = false)]
        public IGSMStatusDataContract ServerManager { get; set; }
    }

    [DataContract]
    public class IGServerListStatusDataContract
    {
        [DataMember(Name = "NbServers", IsRequired = true)]
    	public string NbServers { get; set; }

        [DataMember(Name = "List", IsRequired = true)]
    	public List<IGServerStatusDataContract> List { get; set; }
    }

    [DataContract]
    public class IGSMStatusDataContract
    {
        [DataMember(Name = "Type", IsRequired = true)]
    	public string Type { get; set; }

        [DataMember(Name = "Address", IsRequired = true)]
    	public string Address { get; set; }

        [DataMember(Name = "Status", IsRequired = true)]
    	public string Status { get; set; }

        [DataMember(Name = "Servers", IsRequired = true)]
        public IGServerListStatusDataContract Servers { get; set; }

        [DataMember(Name = "Connections", IsRequired = true)]
    	public IGConnectionListStatusDataContract Connections { get; set; }

    }

    [DataContract]
    public class IGSMRootStatusDataContract
    {
        [DataMember(Name = "ServerManager", IsRequired = true)]
        public IGSMStatusDataContract ServerManager { get; set; }
    }

        /* EXAMPLE
    "Type": "Remote",
    "Address": "192.168.1.31p0",
    "Status": "IGSMSTATUS_READY",
    "Servers": {
        "NbServers": "1",
        "List": [
            {
                "Type": "Remote",
                "Address": "127.0.0.1p4062",
                "Status": "IGSMSTATUS_READY",
                "ServerManager": {
                    "Type": "Local",
                    "Address": "127.0.0.1p1000",
                    "Status": "IGSMSTATUS_READY",
                    "Servers": {
                        "NbServers": "1",
                        "List": [
                            {
                                "Type": "Local",
                                "Address": "127.0.0.1p1001",
                                "Status": "IGSMSTATUS_READY",
                                "Connection": {
                                    "Type": "Local",
                                    "User": "Demo@127.0.0.1",
                                    "Status": "IGSMSTATUS_READY",
                                    "SessionTime": "15s",
                                    "IdleTime": "8s",
                                    "Images": "1"
                                }
                            }
                        ]
                    },
                    "Connections": {
                        "NbConnections": "1",
                        "List": [
                            {
                                "Type": "Local",
                                "Server": "127.0.0.1p1001",
                                "User": "Demo@127.0.0.1"
                            }
                        ],
                        "NbMaxConnections": "100",
                        "NbUserConnections": "1",
                        "NbAvailConnections": "0"
                    }
                }
            }
        ]
    },
    "Connections": {
        "NbConnections": "1",
        "List": [
            {
                "Type": "Remote",
                "Server": "127.0.0.1p4062",
                "User": "None"
            }
        ]
    }
         */
}
