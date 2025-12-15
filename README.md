### Multi-Thread-Chat-Server



<br>

## 1. 프로젝트 소개 
Memory Pool과 Object Pool을 도입하여 메모리 할당/해제 오버헤드를 최소화하고, 시스템 리소스를 효율적으로 관리하는 멀티스레드 채팅 서버입니다.


### 🎯 핵심 개발 목표
- **Memory Pool:** 잦은 `new/delete`로 인한 오버헤드 감소 및 힙 단편화 방지
- **Object Pool:** 세션, 패킷 등 빈번히 생성/파괴되는 객체의 재사용
- **DB Connection Pool:** 데이터베이스 연결/해제 비용 절감 및 쿼리 처리 속도 향상
- **Layered Architecture**: Spring Framework의 3-Tier 구조를 벤치마킹하여, 네트워크 처리와 비즈니스 로직을 명확히 분리하고 유지보수성을 강화

<br>

## 2. 기술 스택
| 분류 | 기술 |
| :-- | :-- |
| **Language** | C++17 |
| **Server** | WinSock2 |
| **Database** | MySQL |
| **Tools** | Visual Studio 2022, Git |
| **client**| C# WinForm |

Client -> https://github.com/choiHyeongSun/WinClient

## 3. 핵심 기능 및 구현

### Memory Pool 

대용량 메모리 블록(Page)을 미리 할당받은 뒤, 이를 고정된 크기의 청크로 나누어 관리하는 방식을 구현했습니다.
<br>

구현 원리: 런타임에 OS에 메모리를 요청하지 않고, 미리 확보된 포인터 주소만 반환하여 할당 속도를 O(1)로 유지합니다.

<br>
<img width="1597" height="980" alt="Image" src="https://github.com/user-attachments/assets/0573b13c-c908-4eaf-b3c3-858f4f2eab17">
<br>
<br>

- 소스코드
```cpp
void* MemoryPool::CreateMemory()
{
    //오브젝트 풀에서 레이스 컨디션을 관리하기 때문에 MemryPool만 사용시 MutiThread환경에서 안전하지 않습니다.


    // 1. 대용량 메모리 블록 사전 할당 (Alignment 보장)
	void* memory = _aligned_malloc(Size, MEMORY_ALLOCATION_ALIGNMENT);
    // 할당 실패 시 예외 처리
	if (memory == nullptr)
	{
		std::cout << "allocate error" << std::endl;
		exit(-1);
	}
    // 2. 메모리 초기화
	memset(memory, 0, Size);

    // 3. Group Size 단위로 청크 분할 (Slicing)
	size_t index = 0;
	while (index < Size)
	{
		UnAllocateMemory.push(static_cast<char*>(memory) + index);
		index += GroupSize;
	}
	return memory;
}

void* MemoryPool::Allocate()
{
    // 가용 메모리가 없을 경우 새로운 블록 확장
	if (UnAllocateMemory.empty())
	{
		Memories.push_back(CreateMemory());
	}
    // O(1) 접근: 큐에서 즉시 반환
	void* memory = UnAllocateMemory.front(); UnAllocateMemory.pop();
	return memory;
}
```


###  ObjectPool

Memory Pool 위에서 동작하며, 생성과 소멸 오버헤드 줄이기 위해 구현하였습니다.
<br>
SList 활용: 일반적인 std::mutex 대신 Windows의 Interlocked SList를 사용하여, 멀티스레드 환경에서도 안전하게 객체를 Pop/Push 할 수 있습니다.
<br>
사용이 끝난 객체는 메모리를 해제하지 않고, 초기화 과정만 거친 후 다시 Pool로 반환됩니다.

<br>
<img width="1969" height="1028" alt="Image" src="https://github.com/user-attachments/assets/4ae6139e-2a08-41af-9d58-a487abfa8d0f" />

<br>
<br>


- 소스코드

```cpp 
template <typename TYPE>
TYPE* ObjectPool<TYPE>::AllocateObject()
{
    /*
        slist는 16바이트로 정렬 되어야 하기 때문에 
        template<class TYPE>
        struct alignas(16) PoolingObject
        {
        	TYPE Data;
        };
        로 맵핑해서 사용합니다.
    */
	PSLIST_ENTRY element = nullptr; 
	while (true)
	{
		try
		{
            // Lock-Free: SList에서 원자적(Atomic)으로 객체 추출
			element = InterlockedPopEntrySList(&UnAllocateObjectList);
			if (element != nullptr) break;
            // Pool이 비어있을 경우 동기화 처리 후 메모리 확장
			MUTEX_CHECK(WaitForSingleObject(AllocationMutex, INFINITE));
			USHORT depth = QueryDepthSList(&UnAllocateObjectList);
			if (depth == 0)
			{
				AllocateMemory();
			}
			ReleaseMutex(AllocationMutex);
		}
		catch (std::exception e)
		{
			std::cout << e.what() << std::endl;
		}
	}
    // Placement New를 사용하여 할당된 메모리에 생성자 호출
	TYPE* resultObject = new(element) TYPE;
	return resultObject;
}

template <typename TYPE>
void ObjectPool<TYPE>::FreeObject(TYPE** InObject)
{
    // 1. 소멸자 명시적 호출
	(*InObject)->~TYPE();
    // 2. memset 함수 호출로 메모리 초기화
	memset(*InObject, 0, TypeSize);
    // 3. SList에 원자적으로 반환 (Push)
	InterlockedPushEntrySList(&UnAllocateObjectList, reinterpret_cast<PSLIST_ENTRY>(*InObject));
    // 4. Dangling Pointer 방지
	*InObject = nullptr;
}

```

<br>


###  Memory Pool과 ObjectPool 활용한 성능 테스트

Memory/Object Pool 적용 전후의 성능을 비교 테스트하였습니다.

<img width="1088" height="214" alt="Image" src="https://github.com/user-attachments/assets/cc65ae50-563a-42dc-9294-46c841290cb3" />

결과: 기본 new/delete 대비 약 3배의 처리량 향상을 확인했습니다.


###  DB Connection Pool 

DB 쿼리 요청마다 발생하는 TCP 연결 비용을 절감하기 위해 구현했습니다.
서버 구동 시 N개의 MySQL 연결을 미리 맺어 큐(Queue)에 보관합니다.
Semaphore를 사용하여 가용 가능한 커넥션 수를 제어하고, 리소스 고갈 시 스레드가 효율적으로 대기하도록 설계했습니다.

### 소스 코드

DB Pool 초기화

```cpp
void DatabasePool::Initialization(const char* ip, const char* user, const char* password, const char* db, INT32 port)
{
	for (int i = 0; i < MAX_POOL_SIZE; i++)
	{
        // DB Wrapper 생성 및 사전 연결
		std::shared_ptr<DatabaseWrapper> wrapper = DatabaseWrapper::CreateDatabase();
		wrapper->ConnectDB(ip, user, password, db, port);
		Databases.push(wrapper);
	}
    // 커넥션 수만큼 세마포어 카운트 초기화
	DBSemaphore = CreateSemaphore(nullptr, MAX_POOL_SIZE, MAX_POOL_SIZE, nullptr);
	std::cout << "Created Database Pool" << std::endl;
}

```

```cpp
std::shared_ptr<DatabaseWrapper> DatabasePool::PopWrapper()
{
    // 가용 커넥션이 생길 때까지 대기 (Resource Counting)
	WaitForSingleObject(DBSemaphore, INFINITE);
    // 임계 영역 보호 (Race Condition 방지)
	std::unique_lock<std::shared_mutex> lock(DatabasesMutex);
	std::shared_ptr<DatabaseWrapper> result = Databases.front(); Databases.pop();
	return result;
}
```


DB Pool 반환
```cpp
void DatabasePool::PushWrapper(std::shared_ptr<DatabaseWrapper>& database)
{
	std::unique_lock<std::shared_mutex> lock(DatabasesMutex);
	Databases.push(database);
    // 반환 후 포인터 초기화 및 세마포어 반환
	database = nullptr;
	if (!ReleaseSemaphore(DBSemaphore, 1, nullptr))
	{
		std::cout << "DBSemaphore Release Error" << std::endl;
		return;
	}
}
```

### 패킷 설계 

패킷
<br>
<img width="717" height="332" alt="Image" src="https://github.com/user-attachments/assets/876c6eb2-7e9e-45c2-bca6-bddd00486f88" />

메시지 패킷
<br>
<img width="717" height="332" alt="Image" src="https://github.com/user-attachments/assets/241a5d55-d17d-4454-b23b-3df7a4febec7" />

룸멤버 패킷
<br>
<img width="717" height="332" alt="Image" src="https://github.com/user-attachments/assets/b7459e6e-f7e0-45bd-8311-826ef7794e90" />

룸 패킷
<br>
<img width="717" height="332" alt="Image" src="https://github.com/user-attachments/assets/7ef35bb9-0249-4f6b-8e73-0281d5f57bca" />

패킷 생성 
```cpp
	template<class T>
	static PoolingObject<T>* PacketBuild()
	{
        //오브젝트 풀을 이용한 패킷 생성
		return PoolingManager<PoolingObject<T>>::GenerateObject();
	}

	template<class T>
	static void ReleasePacket(PoolingObject<T>** release)
	{
        //생성 했던 Packet Release하기
		return PoolingManager<PoolingObject<T>>::FreeObject(release);
	}
```



### 서버 통신 설계 

Spring Framework의 3-Tier Layered Architecture (Controller - Service - Repository) 구조를 벤치마킹하여, 
네트워크 처리 로직과 비즈니스 로직을 분리하였습니다.

<img width="501" height="548" alt="Image" src="https://github.com/user-attachments/assets/8787bc60-70c7-411a-816f-96da989842c8" />

1. Packet Dispatcher (RecvManager)
    <br>
    네트워크로부터 수신된 패킷의 Type을 분석하여 적절한 Controller로 진입점 역할을 수행합니다.
2. Controller
    <br>
    Request 데이터를 파싱하고 검증한 뒤, 구체적인 비즈니스 로직 수행을 위해 Service 계층을 호출합니다.
3. Service
    <br>
    핵심 비즈니스 로직(채팅 처리 등)을 수행하며, 트랜잭션의 단위를 관리합니다.
4. Repository
    <br>
    DB Connection Pool을 통해 데이터베이스에 접근하며, SQL 쿼리 실행 및 데이터 매핑을 담당합니다.
5. Architecture
    <br>
    a. 모든 계층(Controller, Service, Repository)은 Singleton Pattern으로 관리되어 메모리 낭비를 방지했습니다.
    <br>
    b. 레이스 컨디션을 방지하기 위해 모든계층은 멤버변수를 최소화 하였으며 멤버변수를 사용시 Mutex를 활용하여 레이스 컨디션을 방지하였습니다.
    <br>
    c. Controller, Service, Repository Manager를 만들어 생성 순서를 명시적으로 지정해줬습니다.

<br>
<br>

Controller, Service, Repository ,Manager
```cpp
//Application 실행시 처음 호출되는 함수 
ServerApp::ServerApp() 
{
    //Manager 생성
	DatabaseManager::CreateInstance();
	RepositoryManager::CreateInstance();
	ServiceManager::CreateInstance();
	ControllerManager::CreateInstance();

    
    //Controller, Service, Repository 싱글톤 생성
	RepositoryManager::getInstance()->InitializeRepository();
	ServiceManager::getInstance()->InitializationService();
	ControllerManager::getInstance()->InitializeControllers();
    ...
}
```



클라이언트에서 수신 받은 패킷 처리
```cpp
DWORD WINAPI RecvProc(LPVOID LpParam)
{
    ...
	while (!ServerApp::isFinish)
	{
		//builder 헤더 생성
		void* data = PacketBuilder::PacketBuild(EMESSAGE_TYPE::NONE);
		PoolingObject<PacketHeader>* header = static_cast<PoolingObject<PacketHeader>*>(data);
		//헤더 확인
		sockWrapper->RecvMessageHeader(userId, &header->Data);
		header->Data.PacketType = PacketBuilder::NtoHPacketType(header->Data.PacketType);
		header->Data.PacketLen = ntohl(header->Data.PacketLen);
		header->Data.UserLocalId = ntohl(header->Data.UserLocalId);
		INT32 len = header->Data.PacketLen;
		if (len < 0)
		{
			std::cout << "Message Size is too Large. User ID : " << userId << ", Size : " << len << std::endl;
			break;
		}
		//전체 Packet 받아오기
		std::vector<char> buffer(header->Data.PacketLen);
		UINT32 totalRecvSize = 0;
		while (totalRecvSize < len)
		{
			int sub = len - totalRecvSize;
			sub = min(sub, bufferMaxSize);
			
			INT32 recvLen = sockWrapper->RecvMessage(userId, buffer.data() + totalRecvSize, sub);
			if (recvLen <= 0)
			{
				std::cout << "Recv Error code : " << GetLastError() << std::endl;
				break;
			}
			totalRecvSize += recvLen;
		}
		...
    }
	...
	return 0;
}

```

RecvManager(Packet Dispatcher)
```cpp
void RecvManager::ExecuteCallback(EMESSAGE_TYPE type, UINT32 localUserID, const PacketHeader* packetHeader,
	const char* packet, INT32 MsgLen, ISocketWrapper* socketWrapper)
{
    std::vector<IRecvMessage*> executeCallback;
	// Critical Section: 빠르게 핸들러 목록만 복사
	{
		std::unique_lock<std::shared_mutex> lock(instance->mutex);
		if (instance->RecvMessageCallbacks.find(type) == instance->RecvMessageCallbacks.cend())
			return;
		for (const auto& callback : instance->RecvMessageCallbacks[type])
		{
			executeCallback.push_back(callback.get());
		}
	}// Lock 해제
    // 실제 로직 실행
	for (const auto& callback : executeCallback)
	{
		callback->OnRecvMessage(localUserID, packetHeader, packet, MsgLen, socketWrapper);
	}
}
```

Define을 통해서 RecvManager로 등록 시키도록 만들어 주었습니다.
```cpp

#define CONTROLLER_CLASS_H(CLASS_NAME, TYPE) \
private: \
	CLASS_NAME() {} \
	static std::shared_ptr<CLASS_NAME> instance; \
	const EMESSAGE_TYPE messageType = TYPE;\
public: \
	static std::shared_ptr<CLASS_NAME> getInstance() \
	{ \
		return instance; \
	}\
	static void CreateInstance(); \
	static void DestroyInstance(); \


#define CONTROLLER_CLASS_CPP(CLASS_NAME) \
	std::shared_ptr<CLASS_NAME> CLASS_NAME::instance = nullptr; \
		\
	void CLASS_NAME::CreateInstance()\
	{ \
		if (instance == nullptr) \
		{ \
			instance = std::shared_ptr<CLASS_NAME>(new CLASS_NAME()); \
			RecvManager::RegisterCallback(instance->messageType, instance);\
		} \
	} \
	void CLASS_NAME::DestroyInstance()\
	{ \
		instance.reset(); \
	} \

```
```cpp
// ExitRoomController.h
class ExitRoomController : public Controller, public IRecvMessage
{
	CONTROLLER_CLASS_H(ExitRoomController, EMESSAGE_TYPE::EXIT_ROOM);
	...
};

// ExitRoomController.cpp

#include "MainPCH.h"
#include "Header/Controllers/ExitRoomController.h"

#include <string>
#include "Utility.h"
#include "Interfaces/ISocketWrapper.h"
#include "Managers/DatabaseManager.h"
#include "Managers/RecvManager.h"

CONTROLLER_CLASS_CPP(ExitRoomController);
...
```














