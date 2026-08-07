#pragma once

UENUM(BlueprintType)
enum class ESessionType : uint8
{
    Login,
    Game,
    Chat
};

UENUM(BlueprintType)
enum class ELoginMode : uint8
{
    Login,
    CheckId,
    SignUp
};

UENUM(BlueprintType)
enum class EChatChannel : uint8
{
    All      UMETA(DisplayName = "일반"),
    Party    UMETA(DisplayName = "파티"),
    Guild    UMETA(DisplayName = "길드"),
    Whisper  UMETA(DisplayName = "귓속말"),
    System   UMETA(DisplayName = "시스템"),
};