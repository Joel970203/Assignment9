#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseBallGameMode.generated.h"

UCLASS()
class SAMPLECHAT_API ABaseBallGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    FTimerHandle ClearUITimerHandle;

    UFUNCTION()
    void ClearAllClientChatBox();

    UFUNCTION(BlueprintCallable, Category = "BaseBall")
    FString ProcessPlayerInput(APlayerController* Player, const FString& Message);

    UFUNCTION(BlueprintCallable, Category = "BaseBall")
    void ProcessPlayerInputFromUI(APlayerController* Player, const FString& Message);

    void BroadcastMessage(const FString& Msg);

    int32 GetScore(const FString& Role) const;

protected:
    TArray<int32> AnswerCode;
    TMap<APlayerController*, int32> TryCounts;
    TSet<APlayerController*> FinishedPlayers;
    TMap<FString, int32> WinCounts;

    UPROPERTY()
    APlayerController* CurrentTurnPlayer;

    FTimerHandle CountdownTimerHandle;
    int32 RemainingTime = 10;

    void ResetGame();
    void StartTurnTimer();
    void UpdateCountdown();

    TArray<int32> GenerateRandomNumber();
    bool ParseInputToArray(const FString& Input, TArray<int32>& OutArray);
    FString CalculateResult(const TArray<int32>& Guess);
};
