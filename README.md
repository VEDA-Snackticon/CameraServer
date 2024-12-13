# C++ 카메라 서버

## 사용 도구

### 1. Drogon c++ Server
### 2. MariaDB
### 3. Jenkins




## 서버 작동방식
<img width="617" alt="image" src="https://github.com/user-attachments/assets/7d1f2914-05b0-42c8-947e-76a294b7e80d" />



## Spring과의 차이


<img width="615" alt="image" src="https://github.com/user-attachments/assets/32139e27-939b-4d29-93d1-e07c174f692b" />


## 조금 더 찾아봐야 할 부분

### 이벤트가 도착하면 해당 이벤트를 전파하는 과정에서 트랜젝션 처리를 잘 하는 방법을 생각해 보는것이 필요해 보임
### 단위 테스트나 개인적인 테스트에서는 트렌젝션 관련 이슈가 없었지만 실제 카메라 테스트 시에 동시에 진행되는 트렌젝션 처리가 매우 어려운 것을 확인함
### 트렌젝션을 올바르게 격리하지 않으면 성능상 이슈가 생기거나 올바른 데이터가 DB에 쓰이지 않을 수 있다는 점을 발견함
### 카메라 테스트와 같이 여러대의 서버를 놓고 콜을 주고받는 경우를 시뮬레이션 해볼 예정
