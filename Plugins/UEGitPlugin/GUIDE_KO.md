# UEGitPlugin 가이드

> **리포지토리**: [ProjectBorealis/UEGitPlugin](https://github.com/ProjectBorealis/UEGitPlugin)
> **기반**: [SRombauts/UE4GitPlugin](https://github.com/SRombauts/UE4GitPlugin) 리팩토링 | **라이선스**: MIT

---

## 1. 이게 뭔가요?

UE5 에디터 안에서 **Git을 소스 컨트롤(버전 관리)로 쓸 수 있게 해주는 플러그인**입니다.

보통 UE 프로젝트에서 팀 협업을 하려면 Perforce(P4V)를 많이 쓰는데, 이건 서버 비용이 들고 설정이 복잡합니다. 이 플러그인을 쓰면 무료인 Git + GitHub로 비슷한 워크플로우를 구현할 수 있습니다.

---

## 2. 핵심 원리: Git LFS 파일 잠금

### 왜 일반 Git만으로는 안 되나?

UE의 에셋 파일(`.uasset`, `.umap`)은 **바이너리 파일**이라 Git의 merge가 불가능합니다. 두 사람이 같은 블루프린트를 동시에 수정하면 둘 중 하나는 버려야 합니다.

### 해결 방법: 잠금(Lock) 워크플로우

Git LFS 2의 파일 잠금 기능을 사용합니다. 흐름은 이렇습니다:

```
1. 체크아웃(잠금) → 내가 이 파일 쓸게! (다른 사람 수정 차단)
2. 수정 → 에디터에서 에셋 작업
3. Submit → 커밋 + 푸시 + 잠금 해제를 한 번에
4. 다른 사람이 이제 이 파일 사용 가능
```

**먼저 잠금 건 사람이 우선**입니다. Perforce의 체크아웃과 동일한 개념이에요.

### 잠금 상태 확인

에디터에서 에셋 아이콘 위에 마우스를 올리면 누가 잠금했는지 보입니다.
- **빨간 체크마크**: 내가 잠금한 파일
- **파란 체크마크**: 다른 사람이 잠금한 파일

잠금 상태는 **30초마다 백그라운드에서 자동 갱신**됩니다. 즉시 확인하려면 수동 Refresh를 누르면 됩니다.

---

## 3. Status Branches란?

"잠금은 안 걸려있지만, 원격에 더 최신 버전이 있으니 **먼저 pull 받고 작업해라**"는 안전장치입니다.

예를 들어: A가 파일을 수정하고 잠금 해제 → B가 아직 pull을 안 받은 상태에서 같은 파일을 체크아웃하려 하면 → **차단됨** (노란/빨간 느낌표로 표시)

오래된 파일 위에서 작업하는 실수를 방지합니다.

**주의**: 원격 브랜치만 감시합니다. 로컬 브랜치끼리 전환하며 작업하는 것은 추적하지 않습니다.

---

## 4. UE5 에디터에서 소스 컨트롤 연동하기

### 사전 준비

플러그인 설치 전에 아래가 필요합니다:
- Git 설치 ([git-scm.com](https://git-scm.com))
- Git LFS 설치 (`git lfs install`)
- GitHub 계정 + HTTPS 인증 (GitHub Desktop 쓰면 자동 설정됨)

### Step 1: 플러그인 설치

프로젝트의 `Plugins/` 폴더에 UEGitPlugin을 넣습니다. **사전 컴파일 바이너리가 없으므로** Visual Studio로 빌드해야 합니다.

### Step 2: 에디터에서 소스 컨트롤 연결

1. UE5 에디터 상단 → **Source Control** 아이콘 클릭 (또는 메뉴 > Revision Control)
2. **Provider**: `Git LFS 2` 선택
3. **User Name**: Git 서버의 사용자 이름 입력 (GitHub 사용자명)
4. `Uses Git LFS 2 File Locking workflow` 체크
5. **Accept Settings**

> **팁**: 체크아웃 후 체크마크가 파란색으로 바뀌면 User Name이 잘못된 것입니다. 파일이 표시하는 이름으로 수정하세요.

### Step 3: 기본 사용법

| 작업 | 방법 |
|------|------|
| 파일 잠금 (체크아웃) | 에셋 우클릭 → Check Out |
| 여러 파일 잠금 | Shift 선택 후 Check Out |
| 폴더 전체 잠금 | 폴더 우클릭 → Check Out |
| 잠금 해제 | 에셋 우클릭 → Check In / Revert |
| 최신 받기 (Pull) | Source Control 메뉴 → Sync/Pull |
| 커밋 + 푸시 | Submit to Revision Control |

> **중요**: Submit은 로컬 커밋 → 원격 푸시 → 잠금 해제를 한 번에 처리합니다. 브랜치를 에디터 내에서 변경할 수는 없지만, 현재 브랜치를 인식하고 올바른 원격 브랜치로 푸시합니다.

---

## 5. 필수 프로젝트 설정

### .gitattributes (LFS 추적 대상 지정)

와일드카드가 아닌 **명시적 확장자**로 지정해야 합니다:
```
*.uasset filter=lfs diff=lfs merge=lfs -text lockable
*.umap filter=lfs diff=lfs merge=lfs -text lockable
```
참조: [PBCore .gitattributes](https://github.com/ProjectBorealis/PBCore/blob/main/.gitattributes)

### INI 설정

`Config/DefaultEditorPerProjectUserSettings.ini`에 추가:

```ini
[/Script/UnrealEd.EditorLoadingSavingSettings]
; [필수] 새 파일 자동 추가 끄기 — 성능 향상 + 의도치 않은 파일 추가 방지
bSCCAutoAddNewFiles=False

; [권장] 에셋 수정 시 "잠금 걸까요?" 팝업 표시 (아래 팝업 모드 참고)
bAutomaticallyCheckoutOnAssetModification=False
bPromptForCheckoutOnAssetModification=True
```

```ini
[/Script/UnrealEd.EditorPerProjectUserSettings]
; [권장] 잠금한 에셋을 에디터 시작 시 자동 로드
bAutoloadCheckedOutPackages=True
```

`Config/DefaultEngine.ini`에 추가:

```ini
[SystemSettingsEditor]
; [권장] Git에서 불필요한 편집 가능 여부 체크 건너뛰기 — 에디터 반응 속도 향상
r.Editor.SkipSourceControlCheckForEditablePackages=1
```

### 각 설정이 하는 일 (쉬운 설명)

| 설정 | 의미 |
|------|------|
| `bSCCAutoAddNewFiles=False` | 에셋 만들 때 자동 git add 안 함. 직접 골라서 추가 |
| `bAutomaticallyCheckoutOnAssetModification=False` | 에셋 수정해도 자동 잠금 안 걸림 (팝업 모드와 함께 사용) |
| `bPromptForCheckoutOnAssetModification=True` | 에셋 수정 시 "잠금 걸까요?" 팝업 표시 (아래 설명 참고) |
| `bAutoloadCheckedOutPackages=True` | 에디터 재시작 시 내가 잠근 에셋 자동 로드 |
| `r.Editor.SkipSourceControlCheckForEditablePackages=1` | Git에서 의미 없는 파일 상태 체크 생략 → 속도 향상 |

### 체크아웃 팝업 모드에 대해

위 설정(`bPromptForCheckoutOnAssetModification=True`)을 켜면, 에셋을 수정하려는 순간 아래와 같은 팝업이 뜹니다:

```
"이 파일을 체크아웃(잠금)하시겠습니까?"
[Check Out] [Make Writable] [Cancel]
```

- **Check Out**: 파일에 잠금을 걸고 수정을 시작합니다. 다른 팀원은 이 파일을 수정할 수 없게 됩니다.
- **Make Writable**: 잠금 없이 로컬에서만 수정합니다. 다른 사람과 충돌 위험이 있으므로 비권장.
- **Cancel**: 수정을 취소합니다.

> **왜 팝업 모드를 권장하나?**: VCS 연동이 처음이라면 매번 팝업을 통해 "지금 내가 이 파일을 잠그고 있구나"를 인식하는 것이 좋습니다. 잠금의 개념이 몸에 익으면 나중에 자동 체크아웃(`bAutomaticallyCheckoutOnAssetModification=True`)으로 전환해도 됩니다. 5~6명 규모 팀이라면 팝업이 뜨는 빈도도 부담스럽지 않고, Perforce의 체크아웃 경험과도 유사합니다.

---

## 6. 인증: 왜 HTTPS인가?

Git LFS는 대용량 파일을 전송하는데, **SSH보다 HTTPS가 LFS 전송 성능이 빠르고 안정적**입니다. 인증 경로도 하나로 통일되어 설정이 간단합니다.

**GitHub Desktop을 쓰면 HTTPS가 기본**이라 별도 설정 없이 바로 됩니다.

터미널에서 직접 쓰는 경우 [Git Credential Manager](https://github.com/GitCredentialManager/git-credential-manager)를 설치하면 GUI로 로그인할 수 있어 편합니다.

---

## 7. 추천 워크플로우: 코드와 에셋 나눠서 관리하기

### 왜 나눠야 하나?

UE 프로젝트에는 두 종류의 파일이 있습니다:

- **코드** (`.cpp`, `.h`, `.ini`, `.Build.cs` 등) — 텍스트 파일이라 Git merge가 가능하고, 잠금이 필요 없습니다.
- **에셋** (`.uasset`, `.umap`) — 바이너리 파일이라 merge가 불가능하고, 반드시 잠금(Lock)으로 충돌을 방지해야 합니다.

에셋은 잠금 관리가 핵심이고, 에디터의 Submit은 커밋 → 푸시 → **잠금 해제까지 한 번에** 처리해줍니다. 이게 GitHub Desktop에서는 안 되기 때문에, 에셋은 에디터에서 올리는 게 훨씬 편합니다.

### 추천: 역할 분리 방식

```
코드 (.cpp, .h, .ini 등)  → GitHub Desktop에서 커밋/푸시 (익숙한 방식 그대로)
에셋 (.uasset, .umap)     → UE 에디터에서 Submit (잠금 해제 자동)
```

| 파일 종류 | 도구 | 잠금 | 이유 |
|-----------|------|------|------|
| C++ 코드, 헤더, 설정 파일 | GitHub Desktop | 불필요 | 텍스트라 merge 가능, 잠금 필요 없음 |
| 블루프린트, 레벨, 머티리얼 등 | UE 에디터 Submit | 자동 관리 | 바이너리라 잠금 필수, Submit이 해제까지 처리 |

> **4주 팀프로젝트 팁**: C++ 위주 작업이라도 블루프린트 수정, 레벨 배치, 머티리얼 조정 등 에셋 작업이 빈번하게 발생합니다. 에셋은 반드시 에디터 Submit을 습관화하세요. "코드는 GitHub Desktop, 에셋은 에디터"로 기억하면 됩니다.

### 만약 GitHub Desktop에서 에셋을 커밋/푸시했다면?

잠금이 자동으로 풀리지 않습니다. 이 경우 아래 중 하나로 해제해주세요:

- 에디터에서 해당 에셋 우클릭 → **Check In**
- 터미널: `git lfs unlock <파일경로>`

실수로 한두 번 이렇게 되는 건 괜찮지만, 습관적으로 하면 잠금이 계속 쌓여서 다른 팀원이 작업을 못 하게 됩니다.

---

## 8. 주의사항

- **에디터 내 Pull 전 저장 필수**: 개선되었지만 완벽하지 않습니다. Pull 전에 반드시 저장하세요
- **브랜치 전환은 터미널에서**: 에디터 안에서는 브랜치를 바꿀 수 없습니다
- **멀티 브랜치 주의**: 여러 로컬 브랜치에서 같은 잠긴 파일을 수정할 수 있으므로, 어느 브랜치에서 수정했는지 직접 추적해야 합니다

---

## 9. Perforce vs Git LFS 비교

| 항목 | Perforce | Git + LFS (이 플러그인) |
|------|----------|------------------------|
| 비용 | 유료 (5인 초과 시) | 무료 |
| 서버 | 전용 서버 필요 | GitHub/GitLab 사용 가능 |
| 설정 난이도 | 높음 | 중간 |
| 대용량 파일 | 네이티브 지원 | Git LFS로 처리 |
| 파일 잠금 | 기본 기능 | LFS Lock으로 구현 |
| 에디터 통합 | Epic 공식 지원 | 커뮤니티 플러그인 |
| 브랜치 | 스트림 (복잡) | Git 브랜치 (익숙) |
| 적합 규모 | 대규모 팀 | 소~중규모 팀 |

---

## 10. 관련 리소스

- [PBCore 위키 (Git 저장소 설정 가이드)](https://github.com/ProjectBorealis/PBCore/wiki)
- [PBCore .gitattributes 예시](https://github.com/ProjectBorealis/PBCore/blob/main/.gitattributes)
- [PBCore .gitignore 예시](https://github.com/ProjectBorealis/PBCore/blob/main/.gitignore)
- [Fortnite 워크플로우 영상 (Status Branches 설명)](https://youtu.be/p4RcDpGQ_tI?t=1443)
- [Git Credential Manager](https://github.com/GitCredentialManager/git-credential-manager)
- [원본 플러그인 (SRombauts)](https://github.com/SRombauts/UE4GitPlugin)
