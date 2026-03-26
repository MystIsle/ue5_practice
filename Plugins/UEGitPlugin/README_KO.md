# Unreal Engine Git 플러그인

이 플러그인은 [SRombauts의 Git LFS 2 플러그인](https://github.com/SRombauts/UE4GitPlugin)을 리팩토링한 것으로, 프로덕션 환경에서 얻은 교훈을 바탕으로 성능 최적화, 새로운 기능 및 워크플로우 개선을 포함합니다.

## 기능

* 멀티스레드 잠금/해제 — 다수의 파일을 잠금/해제할 때 성능이 크게 향상됩니다
* 원격 잠금 상태는 필요한 경우에만 확인합니다
* 로컬 잠금 캐시를 추가하여 로컬 작업 속도를 향상시켰습니다
* 리포지토리 파일 탐색 성능 개선
* 초기화 로직 개선
* 전반적인 문구 및 UX 개선
* 에디터 내 Pull 기능 대폭 개선
  * 변경된 파일만 새로고침하여 대규모 프로젝트에서의 크래시를 방지합니다
  * rebase 워크플로우를 사용하여 로컬 작업을 적절히 관리합니다
* Status Branches 지원 추가 — 여러 브랜치에 걸쳐 원격의 오래된 파일을 확인합니다
* 주기적인 백그라운드 원격 새로고침으로 원격 파일 상태를 최신으로 유지합니다
* 오래된 로컬 복사본에서의 푸시를 자동으로 처리합니다
* 성공적인 작업에 대한 상태 업데이트 최적화
* 에디터에서 잠금 가능한 파일(에셋, 맵)과 잠금 불가능한 파일(설정, 프로젝트 파일)을 모두 관리합니다
* 에디터 내 상태 표시 개선
* [PBSync](https://github.com/ProjectBorealis/PBSync) 바이너리 동기화와의 통합
* 전반적인 성능 및 메모리 사용량 개선

## 설치

프로젝트의 `Plugins/` 폴더에 설치하거나, 엔진에 설치하려면 `Engine/Plugins/Developer/GitSourceControl.uplugin`을 `Engine/Plugins/Developer/GitSourceControl.uplugin.disabled`로 이름을 변경한 후 이 플러그인을 `Engine/Plugins` 폴더에 설치합니다.

현재 사전 컴파일된 바이너리를 제공하지 않으므로, Visual Studio를 사용하여 플러그인을 컴파일해야 합니다.

### .gitattributes 및 .gitignore에 대한 참고사항

이 플러그인은 콘텐츠 폴더에 대한 와일드카드(`Content/**`) 대신, `*.umap` 및 `*.uasset`에 대한 **명시적인 파일 속성**을 요구합니다.

예시는 [프로젝트의 .gitattributes](https://github.com/ProjectBorealis/PBCore/blob/main/.gitattributes)를 참조하세요.

[프로젝트의 .gitignore](https://github.com/ProjectBorealis/PBCore/blob/main/.gitignore)도 확인해 보시기 바랍니다.

### 인증에 대한 참고사항

Git 리포지토리에 **HTTPS 인증**을 사용하는 것을 강력히 권장합니다.

이를 통해 단일 자격 증명 경로를 사용할 수 있으며, LFS의 강력하고 빠른 HTTPS 지원을 활용할 수 있습니다.

[Git Credential Manager](https://github.com/GitCredentialManager/git-credential-manager)를 사용하면 GUI를 통해 모든 Git 제공자에 쉽게 인증할 수 있어 HTTPS 인증이 훨씬 간편해집니다.

### Unreal 설정에 대한 참고사항

#### 필수 설정

* 이 플러그인은 파일이 항상 명시적으로 추가된다고 가정합니다. 이 결정은 성능과 워크플로우에 유리하기 때문입니다. `Config/DefaultEditorPerProjectUserSettings.ini`에 설정:

```ini
[/Script/UnrealEd.EditorLoadingSavingSettings]
bSCCAutoAddNewFiles=False
```

#### 권장 설정

* 수정 시 자동 체크아웃 — OFPA 등의 워크플로우에 적합합니다 (단, 사용자가 과도한 잠금에 주의해야 합니다). `Config/DefaultEditorPerProjectUserSettings.ini`에 설정:

```ini
[/Script/UnrealEd.EditorLoadingSavingSettings]
bAutomaticallyCheckoutOnAssetModification=True
bPromptForCheckoutOnAssetModification=False
```

* 또는, 수정 시 자동 프롬프트 — 사용자 흐름에 다소 방해가 되지만 잠금에 더 보수적입니다:

```ini
[/Script/UnrealEd.EditorLoadingSavingSettings]
bAutomaticallyCheckoutOnAssetModification=False
bPromptForCheckoutOnAssetModification=True
```

---

* 에디터에서 체크아웃된 패키지를 자동으로 로드하도록 설정. `Config/DefaultEditorPerProjectUserSettings.ini`에 설정:

```ini
[/Script/UnrealEd.EditorPerProjectUserSettings]
bAutoloadCheckedOutPackages=True
```

---

* Git에서 불필요한 기능을 비활성화하여 성능을 개선. `Config/DefaultEngine.ini`에 설정:

```ini
[SystemSettingsEditor]
r.Editor.SkipSourceControlCheckForEditablePackages=1
```

## Status Branches - 필수 코드 변경사항

Epic Games는 4.20에서 Status Branches를 추가했으며, 이 플러그인은 이를 지원합니다. 자세한 내용은 [Fortnite의 워크플로우](https://youtu.be/p4RcDpGQ_tI?t=1443)를 참조하세요. 다음은 자체 게임에 적용하는 예시입니다.

1. `UUnrealEdEngine` 서브클래스를 만듭니다. 에디터 전용 모듈에 만들거나, `WITH_EDITOR` 가드를 사용하세요.
2. 다음 코드를 추가합니다:

```cpp
#include "ISourceControlModule.h"
#include "ISourceControlProvider.h"

void UMyEdEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);

	// 상태 브랜치 등록
	const ISourceControlModule& SourceControlModule = ISourceControlModule::Get();
	{
		ISourceControlProvider& SourceControlProvider = SourceControlModule.GetProvider();
		// 순서가 중요합니다. 낮은 값이 계층 구조에서 하위에 위치합니다.
		// 즉, 상위 브랜치의 변경사항이 자동으로 하위로 병합됩니다.
		// (자동 병합은 적절히 구성된 CI 파이프라인이 필요합니다)
		// 이 패러다임에서 브랜치가 높을수록 더 안정적이며, 변경사항은 수동으로 상위로 승격됩니다.
		const TArray<FString> Branches {"origin/develop", "origin/promoted"};
		SourceControlProvider.RegisterStateBranches(Branches, TEXT("Content"));
	}
}
```

3. `Config/DefaultEngine.ini`에서 에디터 엔진을 설정합니다 (`UMyUnrealEdEngine` 클래스의 경우 클래스명이 `MyUnrealEdEngine`이어야 합니다!):

```ini
[/Script/Engine.Engine]
UnrealEdEngine=/Script/MyModule.MyEdEngine
```

5. 이 예시에서 `origin/promoted`는 가장 높은 테스트 브랜치입니다. 이 브랜치의 변경사항은 테스트가 필요 없는 에셋 변경사항이며, 자동으로 `origin/develop`로 병합됩니다. `origin/trunk`, `origin/main` 등 여러 브랜치로 확장할 수 있으며, 가장 안정적인 브랜치에서 가장 불안정한 브랜치로 변경사항을 자동 전파할 수 있습니다. 이 패러다임에서 불안정한 브랜치의 변경사항은 병합 검토 후 수동으로 안정적인 브랜치로 승격됩니다.
**참고**: `RegisterStateBranches`의 두 번째 인자는 Perforce 전용이며 무시되지만, 상대 콘텐츠 경로를 가리키기 위한 것입니다.

6. Status Branch 코드를 에디터 전용 모듈에 구현하는 경우, .uproject 설정에서 에디터 모듈의 로딩 단계를 `Default`로 설정해야 합니다 (그렇지 않으면 에디터가 서브클래스된 UUnrealEdEngine 클래스를 찾는 데 어려움을 겪을 수 있습니다):
```json
		{
			"Name": "MyTestProjectEditor",
			"Type": "Editor",
			"LoadingPhase": "Default"
		}
```

## Status Branches - 개념적 개요

이 기능은 오래된(out-of-date) 파일을 잠금하고 수정하지 않도록 도와줍니다.

사용자가 **어떤** 브랜치에 있든, 'status branch' 목록에 포함된 브랜치를 추적 중이든 아니든, **원격 서버에 로컬 브랜치보다 더 최근 변경사항**이 있는 파일을 **체크아웃할 수 없습니다** — 단, 해당 변경사항이 **'status branch' 목록**의 브랜치에 있어야 합니다.
* 원격 브랜치의 변경사항이 status branch 목록에 **없으면**, 사용자는 원격 변경사항에 대해 **알림을 받지 않습니다.**
* 사용자가 **로컬 브랜치**에 변경을 가한 후 **다른 로컬 브랜치**로 **전환**하면, 다른 브랜치에 대한 **자신의 변경사항**에 대해 알림을 받지 **않습니다** — status branch 목록 포함 여부와 **무관합니다** **(이 기능은 원격 브랜치만 확인합니다!)**
* 사용자가 status branch 목록에 있는 원격 브랜치를 추적 중이면, **오래된 파일(상위 스트림에서 변경된 파일)을 잠글 수 없습니다**.

![Status Branch 개요](https://i.imgur.com/bY3igQI.png)

#### 참고:

파일 잠금은 변경사항이 서버에 푸시된 후에만 해제하는 것이 중요합니다. 시스템은 파일에 로컬 변경사항이 있는지 판단할 수 없으므로, 잠긴 파일을 수정한 경우 반드시 'status branch' 목록에 포함된 원격 브랜치에 변경사항을 푸시해야 다른 사용자가 해당 변경사항을 확인하고 오래된 파일 수정을 피할 수 있습니다. 그렇지 않으면 파일을 계속 잠금 상태로 유지해야 합니다!

또한, 두 개 이상의 브랜치를 번갈아 사용하는 경우 잠긴 파일에 대해 어떤 브랜치에서 변경했는지 직접 추적해야 합니다. 시스템은 여러 브랜치에서 같은 잠긴 파일을 수정하는 것을 방지하지 않습니다!

#### 'Status Branch' 기능의 실제 사용 예시:

* 사용자가 `develop` 브랜치를 체크아웃했지만, `origin/develop`에 `FirstPersonProjectileMaterial`에 대한 상위 스트림 변경사항이 있습니다. **노란색** 느낌표로 표시됩니다.
* `promoted` 브랜치에도 더 최근 상위 스트림 변경사항이 있으며, **빨간색** 느낌표로 표시됩니다. (참고: 플러그인은 현재 변경사항이 있는 브랜치 이름을 보고하지 않습니다.)

![Status Branch 기능 실제 모습](https://iili.io/1HqPhg.webp)

## 에디터 내 일반 사용법

### 소스 컨트롤 연결:

일반적으로 `Uses Git LFS 2 File Locking workflow` 옆의 필드는 Git 서버의 `User Name`과 일치해야 합니다:
(파일 체크아웃 직후 체크마크가 파란색으로 변하면 LFS 이름이 올바르지 않은 것이므로, 파일을 체크아웃한 것으로 표시된 이름으로 업데이트하세요)

![소스 컨트롤 연결](https://iili.io/1HzKep.webp)

### 하나 이상의 에셋 체크아웃(잠금):

개별 파일을 잠그거나 `Shift`를 눌러 여러 파일을 선택하고 한 번에 잠글 수 있으며, 개별적으로 잠그는 것보다 훨씬 빠릅니다.

![여러 에셋 체크아웃](https://iili.io/1HYog9.webp)

### 변경되지 않은 에셋 잠금 해제:

개별 파일을 해제하거나 `Shift`를 눌러 여러 파일을 선택하고 한 번에 해제할 수 있으며, 개별적으로 해제하는 것보다 훨씬 빠릅니다.

![여러 에셋 잠금 해제](https://iili.io/1HYzJe.webp)

### 폴더 내 모든 에셋 잠금:

폴더를 우클릭하고 `Check Out`을 클릭하여 폴더 내 모든 파일을 잠글 수 있습니다.

![폴더 내 모든 에셋 잠금](https://iili.io/1HYCfS.webp)

### 잠금 상태 보기:

에셋 아이콘에 마우스를 올리면 파일 잠금 소유자를 확인할 수 있습니다. 내가 잠근 파일은 **빨간색** 체크마크, 다른 사용자의 잠금은 **파란색** 체크마크로 표시됩니다.

![파일 잠금 보기](https://iili.io/1HYn07.webp)

### 에디터 내에서 최신 변경사항 Pull:

현재 체크아웃된 브랜치의 최신 변경사항을 에디터 내에서 Pull할 수 있습니다. 항상 원활하게 작동하지는 않지만, 이 프로세스를 개선하기 위한 노력이 이루어졌습니다. 그래도 Pull 전에 항상 변경사항을 저장하는 것을 권장합니다.

![최신 변경사항 Pull](https://iili.io/1HhumN.webp)

### 변경사항 업스트림 제출:

`Submit to revision control`은 로컬 커밋을 생성하고, 푸시한 후, 파일 잠금을 해제합니다.
(플러그인 내에서 브랜치를 체크아웃할 수는 없지만, 완전한 브랜치 인식이 가능합니다! 이 시나리오에서 사용자가 `develop` 브랜치를 체크아웃한 경우, 변경사항은 `origin/develop`로 푸시됩니다.)

![소스 컨트롤에 제출](https://iili.io/1HhI7R.webp)

## 추가 자료

Git 리포지토리 설정에 대한 자세한 내용은 [PBCore 위키](https://github.com/ProjectBorealis/PBCore/wiki)에서 확인할 수 있습니다.
