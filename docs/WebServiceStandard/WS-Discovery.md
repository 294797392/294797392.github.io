## WS-Discovery
Web Services Dynamic Discovery
服务发现协议，主要功能是当客户端不知道Web服务地址的时候，通过客户端探测的方式动态获取要调用的服务的地址。由结构化信息标准促进组织（OASIS：Organization for the Advancement of Structured Information Standards）制定。WS-Discovery协议有两种工作模式，Ad-hoc和Managed，下面分别介绍这两种模式。
客户端和服务端通过UDP协议发送SOAP消息格式去实现该协议。
客户端查找一个目标服务的步骤分两步：
1. 客户端根据目标服务类型查找目标服务
2. 客户端定位目标服务的位置

## WS-Addressing
WS-Addressing规范定义了一种将消息寻址信息综合到Web services消息中的标准。WS-Discovery基于WS-Addressing。
WS-Addressing规范定义了两个新的元素：EndpointReference
<wsa:EndpointReference>
    <wsa:Address>xs:anyURI</wsa:Address>
    [<wsa:ReferenceProperties>... </wsa:ReferenceProperties>]
    [<wsa:ReferenceParameters>... </wsa:ReferenceParameters>]
    [<wsa:PortType>xs:QName</wsa:PortType>]
    [<wsa:ServiceName [PortName="xs:NCName"]>xs:QName</wsa:ServiceName>]
    [<wsp:Policy> ... </wsp:Policy>]
</wsa:EndpointReference>
/wsa:EndpointReference/wsa:Address：服务的逻辑地址或者网络地址，必须是唯一的。逻辑地址可以使用UUID表示，网络地址可以使用HTTP/HTTPS地址表示



## 服务端加入和离开网络
服务端在启动或者网络接口发生变化，或者改变SOAP元数据的版本信息时要发送一个Hello多播消息，表示要加入网络。多播地址是固定的239.255.255.250（IPV4）或者FF02::C（IPV6），端口是固定的3702，为了弥补UDP协议的不可靠传输，需要使用一种算法来发送多播。为了将网络风暴的风险降至最低，必须在等待APP_MAX_DELAY毫秒后再发送Hello消息。
服务端通过发送Bye多播消息离开一个多播组。
服务端必须在SOAP消息的<a:EndpointReference>元素里指定服务的唯一标识符，这个标识符必须是唯一的，推荐使用使用uuid:xxxxxxxxxxxx的格式作为唯一标识符的格式，如果服务端的网络地址（例如http，udp，tcp）可以保证是唯一的，那么在<a:EndpointReference>元素里也可以直接使用网络地址当做唯一标识符。
服务端还必须需要提供一个可以访问到该服务的可以解析的网络地址（例如http，udp，tcp地址），这个地址在<d:XAddrs>元素里定义。

#### Hello
<s:Envelope xmlns:a="http://schemas.xmlsoap.org/ws/2004/08/addressing" 
			xmlns:d="http://schemas.xmlsoap.org/ws/2005/04/discovery" 
			xmlns:s="http://www.w3.org/2003/05/soap-envelope"> 
	<s:Header> 
		<a:Action>http://schemas.xmlsoap.org/ws/2005/04/discovery/Hello</a:Action> 
		<a:MessageID>xs:anyURI</a:MessageID> 
		[<a:RelatesTo RelationshipType="d:Suppression">xs:anyURI </a:RelatesTo>]
		<a:To>urn:schemas-xmlsoap-org:ws:2005:04:discovery</a:To> 
		<d:AppSequence InstanceId="xs:unsignedInt" [SequenceId="xs:anyURI"] MessageNumber="xs:unsignedInt"/> 
		... 
	</s:Header> 
	<s:Body> 
		<d:Hello> 
			<a:EndpointReference>
				<a:Address>uuid:xxxxxxxxxxxx</a:Address>
			</a:EndpointReference> 
			[<d:Types>list of xs:QName</d:Types>]
			[<d:Scopes>list of xs:anyURI</d:Scopes>]
			[<d:XAddrs>list of xs:anyURI</d:XAddrs>]
			<d:MetadataVersion>xs:unsignedInt</d:MetadataVersion> 
			... 
		</d:Hello> 
	</s:Body> 
</s:Envelope> 

2. Bye消息格式
<s:Envelope xmlns:a="http://schemas.xmlsoap.org/ws/2004/08/addressing" 
			xmlns:d="http://schemas.xmlsoap.org/ws/2005/04/discovery" 
			xmlns:s="http://www.w3.org/2003/05/soap-envelope"> 
	<s:Header> 
		<a:Action>http://schemas.xmlsoap.org/ws/2005/04/discovery/Bye</a:Action> 
		<a:MessageID>xs:anyURI</a:MessageID> 
		<a:To>urn:schemas-xmlsoap-org:ws:2005:04:discovery</a:To> 
		<d:AppSequence InstanceId="xs:unsignedInt" [SequenceId="xs:anyURI"] MessageNumber="xs:unsignedInt" /> 
		... 
	</s:Header> 
	<s:Body> 
		<d:Bye> 
			<a:EndpointReference>、
				<a:Address>uuid:xxxxxxxxxxxx</a:Address>
			</a:EndpointReference> 
			... 
		</d:Bye> 
	</s:Body> 
</s:Envelope>

3. 参数解释
#### Header
Actuion：必须，消息类型，固定URL，表示Hello消息
MessageID：必须，消息ID，格式是“uuid:0a6dc791-2be6-4991-9af1-454778a1917a”，可以使用gSOAP的soap_wsa_rand_uuid函数生成，每条消息的ID都不一样。
RelatesTo：可选，服务端要回复的客户端消息ID，格式是“uuid:0a6dc791-2be6-4991-9af1-454778a1917a”，如果没有要回复的消息，可以忽略不写
To：必须，固定URL
AppSequence：必须，消息排序使用
	InstanceId：必选，实例ID，每次服务宕机、丢失状态和重新恢复时，都必须增加>= 1。否则不应该增加 
	SequenceId：可选，序列ID，必须是唯一的，如果不写，默认是NULL
	MessageNumber：必选，消息ID，每次加1，必须是唯一的，但重发消息的时候需要使用上一次的值

#### Body
Hello/EndpointReference：必选，服务端的唯一标识符，推荐使用uuid:xxxxxxxx的格式。如果服务端的URL可以保证是唯一的，那么也可以直接使用URL。
Hello/Type：可选，对于服务来说，是服务的实现类型，默认为空。如果是发现代理，必须是d:DiscoveryProxy和d:TargetService，表示是一个服务代理。








## 客户端根据目标服务类型查找目标服务
客户端通过UDP多播发送探测（Probe）消息，服务端收到Probe消息后，根据客户端的Scope和Type判断（Scope和Type是否都匹配）是否是客户端请求的服务类型，如果匹配成功，则发送单播ProbeMatches消息给客户端，如果匹配失败，则不执行任何操作。
如果是发现代理收到了Probe消息，发现代理先发送Hello单播消息给客户端，然后客户端发送Probe单播给代理，然后代理发送ProbeMatches单播给服务端。无论有没有匹配的目标服务，发现代理总是会发送ProbeMatches单播消息给客户端。

#### Probe
1. 客户端实现

2. 消息格式
<s:Envelope xmlns:a="http://schemas.xmlsoap.org/ws/2004/08/addressing" 
			xmlns:d="http://schemas.xmlsoap.org/ws/2005/04/discovery" 
			xmlns:s="http://www.w3.org/2003/05/soap-envelope"> 
	<s:Header> 
		<a:Action>http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe</a:Action> 
		<a:MessageID>xs:anyURI</a:MessageID> 
		[<a:ReplyTo>endpoint-reference</a:ReplyTo>]? 
		<a:To>xs:anyURI</a:To> 
		... 
	</s:Header> 
	<s:Body> 
		<d:Probe> 
			[<d:Types>list of xs:QName</d:Types>]? 
			[<d:Scopes [MatchBy="xs:anyURI"]? ... >list of xs:anyURI </d:Scopes>]? 
			... 
		</d:Probe> 
	</s:Body> 
</s:Envelope> 

#### ProbeMatches
1. 服务端实现
服务端收到客户端的Probe组播消息，并匹配成功后，发送ProbeMatches单播消息给客户端，如果匹配失败，则不执行任何操作。服务端在收到Probe消息后，至少要等待APP_MAX_DELAY毫秒才能发送ProbeMatches消息。
如果是发现代理收到了组播消息，那么发现代理发送一个Hello单播消息给客户端。发现代理发送ProbeMatches消息不用等待。

2. 消息格式
<s:Envelope xmlns:a="http://schemas.xmlsoap.org/ws/2004/08/addressing" 
			xmlns:d="http://schemas.xmlsoap.org/ws/2005/04/discovery" 
			xmlns:s="http://www.w3.org/2003/05/soap-envelope"> 
	<s:Header> 
		<a:Action>http://schemas.xmlsoap.org/ws/2005/04/discovery/ProbeMatches</a:Action> 
		<a:MessageID>xs:anyURI</a:MessageID> 
		<a:RelatesTo>xs:anyURI</a:RelatesTo> 
		<a:To>xs:anyURI</a:To> 
		<d:AppSequence /> 
		... 
	</s:Header> 
	<s:Body> 
		<d:ProbeMatches> 
			[<d:ProbeMatch> 
				<a:EndpointReference> ... </a:EndpointReference> 
				[<d:Types>list of xs:QName</d:Types>]? 
				[<d:Scopes>list of xs:anyURI</d:Scopes>]? 
				[<d:XAddrs>list of xs:anyURI</d:XAddrs>]? 
				<d:MetadataVersion>xs:unsignedInt</d:MetadataVersion> 
				... 
			</d:ProbeMatch>]* 
			... 
		</d:ProbeMatches> 
	</s:Body> 
</s:Envelope> 

3. 参数解释
/s:Envelope/s:Header/a:Action：必须，固定值
/s:Envelope/s:Header/a:MessageID：必须，消息ID，唯一值
/s:Envelope/s:Header/a:RelatesTo：必须，Probe消息的MessageID
/s:Envelope/s:Header/a:To：必须，如果Probe消息里的Endpoint是IP地址和端口的组合，那么这个值必须是http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous
/s:Envelope/s:Header/d:AppSequence：必须
/s:Envelope/s:Body/d:ProbeMatches：可选，匹配的服务信息。如果是TargetService发送，则包含了自己的服务信息（如果TargetService不匹配，则不发送ProbeMatches消息）；如果是发现代理发送，则有可能包含0个或多个匹配的服务信息（发送代理总是会发送ProbeMatches消息）
/s:Envelope/s:Body/d:ProbeMatches/d:ProbeMatch/a:EndpointReference：目标服务的终结点信息




## 客户端定位目标服务的位置
客户端通过发送Resolve消息来定位目标服务（获取可以调用服务的网络地址）

#### Resolve
客户端通过发送Resolve消息来获取目标服务的网络传输地址。
如果客户端没检测到发现代理，则发送Resolve组播消息。
如果客户端检测到了发现代理，则发送Resolve单播消息给发现代理。
Resolve消息格式：
<s:Envelope xmlns:a="http://schemas.xmlsoap.org/ws/2004/08/addressing" 
			xmlns:d="http://schemas.xmlsoap.org/ws/2005/04/discovery" 
			xmlns:s="http://www.w3.org/2003/05/soap-envelope"> 
	<s:Header> 
		<a:Action>http://schemas.xmlsoap.org/ws/2005/04/discovery/Resolve</a:Action> 
		<a:MessageID>xs:anyURI</a:MessageID> 
		[<a:ReplyTo>endpoint-reference</a:ReplyTo>]? 
		<a:To>xs:anyURI</a:To> 
		... 
	</s:Header> 
	<s:Body> 
		<d:Resolve> 
			<a:EndpointReference>...</a:EndpointReference>
			... 
		</d:Resolve> 
	</s:Body> 
</s:Envelope> 


#### ResolveMatch
如果目标服务收到了匹配的Resolve消息，目标服务必须回应一个ResolveMatch消息。
如果目标服务收到了不匹配的Resovle消息，目标服务不做任何操作。
如果发现代理收到了Resolve消息，则回复Hello单播消息给目标服务。
如果发现代理没有匹配的Resolve消息，则发送0个匹配的服务给客户端；如果发现代理有匹配的Resolve消息，则发送1个或多个匹配的服务给客户端。
ResolveMatch消息格式：
<s:Envelope xmlns:a="http://schemas.xmlsoap.org/ws/2004/08/addressing" 
			xmlns:d="http://schemas.xmlsoap.org/ws/2005/04/discovery" 
			xmlns:s="http://www.w3.org/2003/05/soap-envelope"> 
	<s:Header> 
		<a:Action>http://schemas.xmlsoap.org/ws/2005/04/discovery/ResolveMatches</a:Action> 
		<a:MessageID>xs:anyURI</a:MessageID> 
		<a:RelatesTo>xs:anyURI</a:RelatesTo> 
		<a:To>xs:anyURI</a:To> 
		<d:AppSequence/> 
		 ... 
	</s:Header> 
	<s:Body> 
		<d:ResolveMatches> 
			[<d:ResolveMatch>
				<a:EndpointReference> ... </a:EndpointReference> 
				[<d:Types>list of xs:QName</d:Types>]? 
				[<d:Scopes>list of xs:anyURI</d:Scopes>]? 
				<d:XAddrslist of xs:anyURI</d:XAddrs> 
				<d:MetadataVersion>xs:unsignedInt</d:MetadataVersion> 
				... 
			</d:ResolveMatch>]? 
			... 
		</d:ResolveMatches> 
	</s:Body> 
</s:Envelope> 

/s:Envelope/s:Header/a:RelatesTo：必须，Resolve消息ID
