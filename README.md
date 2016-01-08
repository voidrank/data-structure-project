# Project Report

**Shiyi Lan**   
**Fudan University**

## Abstract

The shortest path problems is widely used in the real world 
	map. Also real-time traffic information and high-frequency queries
	from client are popular problems people concentrates in. To solve the
	the two problems, we introduce some good algorithms which perform 
	very well in the Shanghai map and are also state-of-art methods in
	algorithms competition. We use Cellular Automaton Algorithms, Split 
	of Balance, Dijkstra with heap. We apply those 
	algorithms on Shanghai Map and achieve that the algorithm complexity 
	of prework limits to O(n^2\*log(n)), the algorithm complexity of query
	limits to O(1) in static map (without real-time traffic information)
	and the algorithm complexity of prework limits to O(n\*sqrt(n)\*log(n)), 
	the algorithm complexity of query limits to O(sqrt(n)\*log(n)) in dynamic, 
	map (with real-time traffic information).



## Static Map
	
#### Background

In most cases, we only want to get an overview of where we want to go and
	which road we will go through. We only need a static map without traffic
	information. In this part, we will introduce the fastest method of shortest path
	with algorithm complexity limiting to O(1).
	
As we extract the information of shanghai map, we found that there are about
	500,000 points in the map. But actually, there are only about 30,000 points
	we really need to consider in the shortest path problems. And reason why such
	few points are useful is that they are not cross points with degree greater 
	than 2.(figure1)
	
![image](media/figure1.png)
	
(figure1. lots of points in the road but only several cross points)
	
I
#### Algorithm
	
	
So the points in the road can be combined to a big edge between two points 
	around them and we can only calculate all pairs of shortest path in the
	cross points set in prework.
	
A shortest path query of two points (u,v) in the real map consists of 3 conditions we will
	meet:
	
1. Both u and v are cross points. As we have calculated all pairs of shortest
	path, we can easily get the answer.

2. Neither u nor v is not cross point. According to the definition of not cross
	points, they are both on the same edge. We define **end nodes** of each not cross 
	points the cross points **around them** (figure 2). The end nodes are cross nodes. 
	So we can get the shortest path between them. The end nodes of u are u_a, u_b, 
	and the end nodes of v are v_a, v_b. The value of shortest path between them is
				
		min{
			dis(u_a, v_a) + dis(u_a, u) + dis(v_a, v),
			dis(u_a, v_b) + dis(u_a, u) + dis(v_b, v),
			dis(u_b, v_a) + dis(u_b, u) + dis(v_a, v),
			dis(u_b, v_b) + dis(u_b, u) + dis(v_b, v)
		}
				
	All of them are calculated in the prework.
	
	3. Node u is the cross point, but v is not cross point. Similar to condition(2),
	we use the shortest distance between cross points and the shortest distance between
	not cross points and their end nodes. The value is 
	
		min{
			dis(u_a, v) + dis(u, u_a),
			dis(u_b, v) + dis(u, u_b)
		}
				
	All the above data is calculated in the prework. So the complexity of query is O(1).
	
	
![image](media/figure2.png)
I
(figure2. 7 not cross points in the road are colored yellow, and their **end nodes** are colored red. )

	
	
### Analysis of Algorithm Complexity &  Experiment Records.
	

Dijkstra with heap is a popular shortest path algorithm with complexity of O(m\*logm), where the m 
indicates the number of edges (which are twice greater than the number of nodes). To 
calculate all pairs of shortest path, we perform SPFA for each cross node, so the 
complexity of algorithm is O(n^2*log(n)).
	
In this experiment we extract 29713 cross nodes from the map, and the number of total
nodes is 504192. The prework cost about 20min on Mac OSX El Capitan, Processor 2.6 GHz 
Intel Core i5, Memory 8 GB 1600 MHz DDR3. And it doesn't get to its extreme performance,
due to not enough memory(it will take up about 12G, the rest 4G data is stored in tmp 
provided by SSD) and with lots of I/O operations and going through layers (socket -> 
nodejs -> c++), but still can deal with 30,000 queries per second w.r.t. 10,000,000 per
second.
	
	
	
## Dynamic Map

### background
	
Congested traffic has become a big trouble in modern city, especially in such cities
with a large number of popularity like Beijing, Shanghai. To help people avoid traffic
jam, we need to take traffic information into consideration when calculating the shortest
path for clients.

To meet the requirement, we should reduce the complexity of calculating shortest path in
the prework. Meanwhile, the query excutive time must keep as quick as possible. It's significant to find the balance point.

### Algorithm

On the base of previous work, we can reduce the map scale to about 30,000 nodes, although it
only need 5 mins to recalculate to do prework, it can't work on China map because it takes up too much memory. In order to perform this map engine on larger map, we introduce Cellular Automaton Algorithm to split the map into sqrt(n) parts for faster prework. We divide all the points into
around O(sqrt(n)) groups according to the geography. (see figure3)

![image](media/figure3.png)

(green: entrance point, red: internal point. All those points are cross points)

In a block, there are some entrance points and internal points. Similar to previous work, we want to take the block and its entrance points into consideration instead of its internal points in shortest path problem. Before we classify query(u,v), we introduce 2 important concepts in our algorithms: **shortest paths between blocks**, **out path of internal points**.

1. Shortest paths between blocks. There are 2 blocks a and b, a has a_n entrances, b has b_n entrances. The shortest path between blocks is a matrix m size of a_n x b_n, where m_a_b(i, j) stands for the shortest distance between the ith entrance in a and the jth entrance in b. 

2. Out path of internal points. There is an internal point u and its block a, a has a_n entrances. Out path of u is a vector v, where v_i stands for the shortest distance between v_i and u in the subgraph of block a.

Now let's solve the shortest path probles. There are 2 conditions of query (u,v).


1. Both u and v are in the same block. In this case, we will calculate the shortest path in the block a whice means that we only consider the points in this block. And then we calculate min{ m_a_a(i,j) + v_u(i) + v_v(j) }.

2. u and v are not in the same block. Calculate min{ m_a_b(i,j) + v_u(i) + v_v(j)}


In the prework, we calculate the matrix m_a_b and v_i in O(n\*log(n)). 

1. v_i. It's easy to calculate v_i in O(sqrt(n)*log(n)). And for each block, we need perform this algorithm for O(sqrt(n)) times. So the total complexity is O(nlog(n)).

2. m_i_j. If we know the shortest path value between each entrance, the work is done. So we define initial value of m_i_j min{v_u(i) + v_v(i)} and then perform dijstra with heap on the matrix whosh complexity is O(n\*sqrt(n)).


And how can I get a good cut of the map? see my [distributed-system-project](https://github.com/voidrank/distributed-system-project).


(Due to the time limit, I didn't implement this part. And I think my previous algorithm is good enough for Shanghai map. And this algorithm is just a bonus.)


## 实现细节

Project使用了浏览器作为前端，NodeJS&C++Extension作为后台进行计算服务。

### web前端

采用了浏览器端作为展示UI。优点如下

#### 跨平台
HTML+JS+CSS技术可以运行在任何平台(Windows, Mac OSX, Linux, Android, iOS)下。由于我直接使用了后台渲染技术，所以即使在微信上地图也能够运行。充分利用了当下web和移动客户端IT浪潮中带来的便利。大家课可以不需要预装任何东西，也不对设备进行任何要求，只需要打开浏览器，进入网站，即可以看到。如果需要开发一款工业级的应用，此架构也能够胜任。

#### 预渲染
比起实时渲染，后台预渲染可以预先把图片渲染好。用户需要加载的时候直接读取文件。这样使得渲染不再卡在计算能力上，同时也共享了服务器上的计算能力，使得计算能力羸弱的手机和一些低端机在运行地图时也不卡。

#### JavaScript脚本语言

##### 后台
使用JavaScript脚本语言可以加速开发的效率，用在一些对计算能力要求很低的业务操作（用户登录/拖取用户信息等）上。这样的开发十分高效，敏捷。对于需要快速上线的业务或者一些实验性的业务有着很高的利用价值。NodeJS框架在2011年夺过Ruby on Rails在Github年度最流行框架的桂冠，成为Web后台开发的又一大攻坚利器。

NodeJS使用了和客户端一样的单进程无阻塞异步任务的方式。其特点为完全没有阻塞，同时也没有切换进程/线程的消耗，在一台普通服务器上就能轻松达到10K级的服务。这是Rails，PHP，Java on Apache框架需要下一定功夫达到benchmark。

同时，NodeJS近乎完整地把浏览器中JS的特性继承过来，使得拥有相关经验的程序员都能够加入到后台开发中来。

NodeJS的开发效率也是惊人的。PayPal曾做过一个实验，用NodeJS和Java同时写一个项目，人数相同的情况下，NodeJS组只用了3个星期，而Java组用了3个月。跑了正确性和效率测试，结果发现，两组的产品相同。至此PayPal决定将所有Java上跑的业务全部用Javascript重写。直至2012年，PayPal已经完成了前端逻辑业务的重构。

##### C++ Extension
NodeJS使用的V8引擎是用c++写的，他同时提供了编写C++ Extension的接口。在一些比较重要的，比较需要效率，比较占用内存的服务上，我们可以通过C++来编写（本工程中所有关于算法和地图存储的部分全部使用C++编写）。它能够提供原生C++的高效率，低内存占用，架构的便捷。例如在本实验中，如果使用JS来编写地图，会极大降低地图的效率，那么所有优化的算法就完全派不上用场。

同时这个Extension被封装的很好，编译也很快，由于依赖的是node-gyp，这个extension可以被轻松地编译在Windows， Linux， Mac OSX上，兼容性也是非常好的。

##### C++ Extension Architecture (MapDB)

这个C++ Extension Architecture被设计成一个内存数据库。近年来NOSQL发展迅速，特别是对于特殊用途的数据库，此类NoSQL发展非常迅速。由于NoSQL在分片，效率上有很多优点。我写的这个C++ Extension Architecture是一个内存存储NoSQL，(常见的内存数据库有:Redis, Memcache)。众所周知，大部分应用的瓶颈都在数据库查询上，特别在地图服务上，如果每次都要从硬盘上读取预处理信息（12G数据），会导致一个极其低下的查询效果，那么同样会导致之前O（1）的查询达不到应有的效果。所以我们选用内存存储。同时对于一个地图服务，我们不需要SQL严密的逻辑，同时，使用SQL严密的逻辑也会严重影响效率，我们需要一个短平快的接口和实现，所以我们使用NoSQL.

分为store和map_algorithm两部分，还有封装了高效c的解析库rapidXML的parser。分为store和map_algorithm的理由是想把算法和数据库储存分开（类似于SQLAlchemy的设计）iterator迭代器设计模式。

store的部分实现了数据的储存，由于没有此服务偏向于算法，我们尽可能让它在保证效率的前提下，而不是纠结于完整的数据库的概念，所以我们直接使用C++标准库的高效的vector数据结构。同时，现在版本的store中还保存有对数据转化的部分，暂时认为转换数据算是数据相关的操作而不是算法相关的操作，此部分还可以继续思考如何安排。不过我们优先考虑实现和效率的问题，架构的问题可以在项目重构时慢慢思考。

map_algorithm包括了所有算法的实现，由于我们尽可能想让map_algorithm部分不要考虑store结构的问题，所以使用了很多模板(template)泛型编程，试图在C++中实现duck type（鸭子类型）的设计，这样做可以让store和map_algorithm解耦合，在今后对store修改的时候，只要保证会影响map_algorithm的API不变化，那么在map_algorithm中也不需要修改。同时，需要提到的是，c++的template泛型编程是远远优于Java的泛型的，这是由于c++在编译时就已经确定了template中的类型，编译器把需要使用的类型的函数全都编译出来，这样所有的操作全是静态操作，而Java泛型则会涉及一些运行时的操作（例如反射），这样会导致效率下降，而在C++使用template则不会影响运行的效率。（我极其建议不要在C++特别是C++11之后使用虚函数和继承，相同可以通过namespace，template和trait实现，oop在此处并不是一个高效易读的设计模式）。

由于本pj主要要实现的东西并不是设计模式，所以我只在架构上做了宏观的涉及，保证具体实现细节的时候不会太坑，而没有把所有的细节全都实现。本架构遵从以下优先度（从高到低）: 程序原型效率，架构科学性，开发效率，代码可读性。

在接口部分，由于时间比较少，在算法优化部分放的时间太多，我没有太多时间进行设计，仅仅是按照一个Object的set & get操作进行。所以从API上来看，我反倒是像实现了Map Single Object（单一模式），而不是一个数据库。实现的部分有几个主要的部分

1. init
2. 给出两个点的index或者是id，求出最短路(输出路径可选)
3. 给出一个一个坐标，输出最近点（KD-Tree）
4. 给出点的index或者是id，给出坐标
5. 给出边的index或者是id，给出边所连的点的编号

6. 是不是关键点(out point)
7. 是不是关注点(非highway的部分不关注)    
8. 获取总点数
9. 获取总路数
10. 获取总边数
11. 获取边界(4个API)

（6-11为测试接口）

前面提到过如果在JS中把所有点都存下来，使用的内存是很大的，所以我们需要存在c++中。同时又需要在JS中获取应该有的信息（最短路，绘图的信息）。


##### 绘图

使用了node-canvas库，这个库同样是c++写的，效率非常高，提供的是js接口（有c++的接口但需要hack，但是优化空间较小）。我们可以先从mapdb(也就是项目中的c++ extension)提取出需要画的点和边，把这些边和点通过js传给node-canvas，绘制地图。

地图采用了5个scale的规模，实际上可以绘制得更精细，scale也可以更多，但本项目主要是做高效率算法，达到这个可视化的程度已经足够测试。




##### 前端
随着Web技术的今年的高速发展，V8引擎(Chrome的js引擎)，MonkeySpider(Firefox的js引擎)以及Chakra(IE&Edge的js引擎)的效率已经是十年前的js引擎数十倍。JS作为一个脚本语言已经获得极高的运行效率。最近出现的Web Assembly更是把js作为最底层的字节码来处理。Mozilla开发的asmjs能运行llvm上的代码，并且能够达到直接运行速度的一半。

同时由于js引擎越来越高效，程序员越来越多，现在很多在iOS，Android下的应用都采用了Hybrid技术。原理是利用原生平台的JavaScript引擎来执行逻辑代码，再对原生的UI和底层接口进行封装，套成一个运行时，这样JS通过通信接口向原生UI和底层接口发送消息，原生UI和底层接口接受到消息之后执行规定的任务。这样，就好像暴露在JS程序员前的就是一个抽象到JS层的原生接口。这样做的效率是非常高的。原因是：1.之前提到的JS引擎技术的突飞猛进，使得JS的运行和内存占用并不成为App瓶颈。2.除去通讯的消耗，调用原生接口的效率等同于原生Java(on Android), Swift&Objective C(on iOS)的效率。不仅如此，随着Android和iOS对WebUI的优化原来越好（Android M已经开始使用GPU渲染UI，而iPhone早就在6.0时期实现了这一技术），一些App和框架直接使用了WebView来实现App界面。也就是说，这些应用其实都是网页。甚至，通过Hybrid方式调用高效UI的办法被移植到了游戏开发上。常见的WebView框架有ionic，原生UI框架有React, cocos2d-js。

本项目中的前端使用了平台兼容性非常好的jQuery，显示地图直接使用img标签，而没有使用canvas，这对于可以让使用不兼容html5的浏览器的用户同样使用地图，从而达到更好的兼容性。同时，前端部分没有加太多和算法无关的业务逻辑，也没有上类如React&Redux, AngularJS之类的框架，还可以有很多拓展的空间。


#### 前后端中间

地图最好的传输的办法应该是用tile的技术，把不同scale的部分预先绘制出来，再通过网络传输这些tiles到前端，前端通过计算用户当前所观察的坐标向服务器索要相关的tiles。

可以知道这样做的办法是非常好的。我们可以比较这个方法和其他方法的优势所在。首先对比同类项目最常用的实时渲染。在之前的篇幅中我们已经介绍过，这样的渲染是会严重依赖于设备的计算能力，并且会产生会的计算资源的浪费。在对于实时切图的做法（向服务器发送屏幕的所在的位置和尺寸，服务器返回整张图片，本项目的做法），实时切图的办法的一个弊病在于网络传输的浪费实在太大。移动地图一次就会产生一个实时切图的请求，这样会导致极大的网络传输请求，达到1mB/s。而tile的做法则利用缓存，把之生成过的图直接显示在前端。这样不仅对设备的计算能力基本没有要求，而且网络传输仅仅依赖于你所在看的部分是否已经被加载过。这是一个非常好的做法。所以此处也存在一个可以优化的地方。

为了同步前后端的配置文件，我们将一个json配置文件放在一个前后台都可以访问的地方。这样充分了利用前后端语言相同的优势。



